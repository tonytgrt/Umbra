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
	CollisionBox->SetBoxExtent(FVector(50.f, 50.f, 100.f));
	CollisionBox->SetCollisionProfileName(TEXT("BlockAll"));
	SetRootComponent(CollisionBox);

	// Visual mesh (offset upward so collision box sits lower relative to the mesh)
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(CollisionBox);
	ObstacleMesh->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PillarMesh(
		TEXT("/Game/FloatingIslandPack/Meshes/Pillars/SM_Pillar_01.SM_Pillar_01"));
	if (PillarMesh.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(PillarMesh.Object);
	}

	bCurrentlyActive = true;
}

void AUmbraObstacle::BeginPlay()
{
	Super::BeginPlay();

	// Start in the correct state
	SetObstacleActive(IsLit());
}

void AUmbraObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bShouldBeActive = IsLit();
	if (bShouldBeActive != bCurrentlyActive)
	{
		SetObstacleActive(bShouldBeActive);
	}
}

bool AUmbraObstacle::IsLit() const
{
	TArray<AActor*> Lanterns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUmbraLantern::StaticClass(), Lanterns);

	const FVector MyLocation = GetActorLocation();

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

	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
}
