// Umbra - Light & Shadow Puzzle Game

#include "UmbraObstacle.h"
#include "UmbraLightSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

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

	// Load grow/sink sound (same asset for both)
	static ConstructorHelpers::FObjectFinder<USoundBase> SunflowerGrowSoundAsset(
		TEXT("/Game/Audio/sunflower_grow.sunflower_grow"));
	if (SunflowerGrowSoundAsset.Succeeded())
	{
		GrowSound = SunflowerGrowSoundAsset.Object;
		SinkSound = SunflowerGrowSoundAsset.Object;
	}

	bCurrentlyActive = true;
	TargetZOffset = 0.f;
	CurrentZOffset = 0.f;
	PreviousTargetZOffset = 0.f;
}

void AUmbraObstacle::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetActorLocation();

	// Start in the correct state
	const bool bLit = IsLit();
	TargetZOffset = bLit ? 0.f : -SinkDepth;
	CurrentZOffset = TargetZOffset;
	PreviousTargetZOffset = TargetZOffset;
	SetActorLocation(OriginalLocation + FVector(0.f, 0.f, CurrentZOffset));
	SetObstacleActive(bLit);
}

void AUmbraObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bShouldBeActive = IsLit();
	TargetZOffset = bShouldBeActive ? 0.f : -SinkDepth;

	// Play sound when direction changes
	if (TargetZOffset != PreviousTargetZOffset)
	{
		USoundBase* SoundToPlay = (TargetZOffset > PreviousTargetZOffset) ? GrowSound : SinkSound;
		if (SoundToPlay)
		{
			UGameplayStatics::SpawnSound2D(this, SoundToPlay, 1.f, 1.f, 4.0f);
		}
		PreviousTargetZOffset = TargetZOffset;
	}

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
	UUmbraLightSubsystem* LightSub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>();
	if (!LightSub)
	{
		return true;
	}

	const FVector MyLocation = OriginalLocation;
	bool bInAnyLightRange = false;

	for (const TWeakObjectPtr<ULightComponent>& WeakLight : LightSub->GetLights())
	{
		ULightComponent* Light = WeakLight.Get();
		if (!Light)
		{
			continue;
		}

		const FVector LightLocation = Light->GetComponentLocation();
		const float Distance = FVector::Dist(LightLocation, MyLocation);

		// Resolve attenuation radius from concrete light type
		float Attenuation = 0.f;
		if (const UPointLightComponent* Point = Cast<UPointLightComponent>(Light))
		{
			Attenuation = Point->AttenuationRadius;
		}
		else if (const USpotLightComponent* Spot = Cast<USpotLightComponent>(Light))
		{
			Attenuation = Spot->AttenuationRadius;
		}

		if (Distance > Attenuation)
		{
			continue;
		}

		// Spotlight cone check
		if (const USpotLightComponent* Spot = Cast<USpotLightComponent>(Light))
		{
			const FVector LightForward = Spot->GetForwardVector();
			const FVector LightToPoint = (MyLocation - LightLocation).GetSafeNormal();
			const float ConeHalfAngleRad = FMath::DegreesToRadians(Spot->OuterConeAngle);
			if (FVector::DotProduct(LightForward, LightToPoint) < FMath::Cos(ConeHalfAngleRad))
			{
				continue;
			}
		}

		bInAnyLightRange = true;

		// Line trace to check for obstructions
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Light->GetOwner());
		Params.AddIgnoredActor(this);

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

		if (!bBlocked)
		{
			return true;
		}
	}

	// If not in any light's range, stay visible
	if (!bInAnyLightRange)
	{
		return true;
	}

	// In range but all blocked — in shadow
	return false;
}

void AUmbraObstacle::SetObstacleActive(bool bActive)
{
	bCurrentlyActive = bActive;
	SetActorEnableCollision(bActive);
}
