// Umbra - Light & Shadow Puzzle Game

#include "UmbraObstacle.h"
#include "UmbraLantern.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AUmbraObstacle::AUmbraObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision box (root)
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(FVector(40.f, 60.f, 100.f));
	CollisionBox->SetCollisionProfileName(TEXT("BlockAll"));
	SetRootComponent(CollisionBox);

	// Visual mesh (offset upward so collision box sits lower relative to the mesh)
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(CollisionBox);
	ObstacleMesh->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
	ObstacleMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	ObstacleMesh->SetRelativeScale3D(FVector(3.f));
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SunflowerMeshAsset(
		TEXT("/Game/Sunflower/Sunflower.Sunflower"));
	if (SunflowerMeshAsset.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(SunflowerMeshAsset.Object);
	}

	bCurrentlyActive = true;
	TargetZOffset = 0.f;
	CurrentZOffset = 0.f;
}

void AUmbraObstacle::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetActorLocation();

	// Start in the correct state
	const bool bLit = IsLit();
	TargetZOffset = bLit ? 0.f : -SinkDepth;
	CurrentZOffset = TargetZOffset;
	SetActorLocation(OriginalLocation + FVector(0.f, 0.f, CurrentZOffset));
	SetObstacleActive(bLit);
}

void AUmbraObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bShouldBeActive = IsLit();
	TargetZOffset = bShouldBeActive ? 0.f : -SinkDepth;

	// Smoothly move toward target
	if (!FMath::IsNearlyEqual(CurrentZOffset, TargetZOffset, 1.f))
	{
		CurrentZOffset = FMath::FInterpConstantTo(CurrentZOffset, TargetZOffset, DeltaTime, MoveSpeed);
		SetActorLocation(OriginalLocation + FVector(0.f, 0.f, CurrentZOffset));
	}
	else if (bShouldBeActive != bCurrentlyActive)
	{
		// Only toggle collision once the movement finishes
		CurrentZOffset = TargetZOffset;
		SetObstacleActive(bShouldBeActive);
	}
}

bool AUmbraObstacle::IsLit() const
{
	TArray<AActor*> Lanterns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUmbraLantern::StaticClass(), Lanterns);

	const FVector MyLocation = OriginalLocation;

	bool bInAnyLightRange = false;

	for (AActor* Actor : Lanterns)
	{
		AUmbraLantern* Lantern = Cast<AUmbraLantern>(Actor);
		if (!Lantern)
		{
			continue;
		}

		// Find the PointLightComponent on the lantern
		UPointLightComponent* Light = Lantern->FindComponentByClass<UPointLightComponent>();
		if (!Light)
		{
			continue;
		}

		const FVector LightLocation = Light->GetComponentLocation();
		const float Distance = FVector::Dist(LightLocation, MyLocation);

		// Skip if outside the light's attenuation radius
		if (Distance > Light->AttenuationRadius)
		{
			continue;
		}

		// This obstacle is within at least one light's range
		bInAnyLightRange = true;

		// Line trace to check for obstructions between the light and this obstacle
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Lantern);
		Params.AddIgnoredActor(this);
		Params.bIgnoreBlocks = false;

		// Ignore all child actors attached to this obstacle
		TArray<AActor*> ChildActors;
		GetAttachedActors(ChildActors);
		for (AActor* Child : ChildActors)
		{
			Params.AddIgnoredActor(Child);
		}

		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			Hit,
			LightLocation,
			MyLocation,
			ECC_Visibility,
			Params
		);

		// If the trace didn't hit anything, the light reaches us
		if (!bBlocked)
		{
			return true;
		}
	}

	// If not in any light's range, stay visible (only disappear when in shadow)
	if (!bInAnyLightRange)
	{
		return true;
	}

	// In range of light(s) but all are blocked — in shadow
	return false;
}

void AUmbraObstacle::SetObstacleActive(bool bActive)
{
	bCurrentlyActive = bActive;
	SetActorEnableCollision(bActive);
}
