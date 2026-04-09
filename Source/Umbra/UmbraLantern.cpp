// Umbra - Light & Shadow Puzzle Game

#include "UmbraLantern.h"
#include "UmbraLightSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/PointLightComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

AUmbraLantern::AUmbraLantern()
{
	PrimaryActorTick.bCanEverTick = false;

	// Collision for mouse hit-testing
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(40.f);
	CollisionSphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionSphere->SetSimulatePhysics(false);
	SetRootComponent(CollisionSphere);

	// Visual mesh
	LanternMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LanternMesh"));
	LanternMesh->SetupAttachment(CollisionSphere);
	LanternMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LanternMesh->SetCastShadow(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/lantern/source/Lantern/StaticMeshes/Lantern_01.Lantern_01"));
	if (MeshFinder.Succeeded())
	{
		LanternMesh->SetStaticMesh(MeshFinder.Object);
		LanternMesh->SetRelativeScale3D(FVector(4.f));
		LanternMesh->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
	}

	// Point light
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(CollisionSphere);
	PointLight->SetIntensity(5000.f);
	PointLight->SetAttenuationRadius(600.f);
	PointLight->SetCastShadows(true);

	LockedZ = 0.f;

	// Bounds plane — hidden by default, shown while dragging
	BoundsPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoundsPlane"));
	BoundsPlane->SetupAttachment(CollisionSphere);
	BoundsPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoundsPlane->SetCastShadow(false);
	BoundsPlane->SetVisibility(false);
	BoundsPlane->SetAbsolute(true, true, true);  // use world transform so bounds match world space

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneFinder.Succeeded())
	{
		BoundsPlane->SetStaticMesh(PlaneFinder.Object);
	}

	// Load lantern sound
	static ConstructorHelpers::FObjectFinder<USoundBase> LanternSoundAsset(
		TEXT("/Game/Audio/lantern.lantern"));
	if (LanternSoundAsset.Succeeded())
	{
		LanternSound = LanternSoundAsset.Object;
	}
}

void AUmbraLantern::BeginPlay()
{
	Super::BeginPlay();

	if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
	{
		Sub->RegisterLight(PointLight);
	}
}

void AUmbraLantern::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
	{
		Sub->UnregisterLight(PointLight);
	}

	Super::EndPlay(EndPlayReason);
}

void AUmbraLantern::OnDragStart_Implementation(FVector WorldPos)
{
	if (LanternSound)
	{
		UGameplayStatics::SpawnSound2D(this, LanternSound, 1.f, 1.f, 0.35f);
	}

	LockedZ = GetActorLocation().Z;

	// Show bounds plane while dragging
	if (bUseBounds && BoundsPlane)
	{
		UpdateBoundsPlane();
		if (BoundsPlaneMaterial)
		{
			BoundsPlane->SetMaterial(0, BoundsPlaneMaterial);
		}
		BoundsPlane->SetVisibility(true);
	}
}

void AUmbraLantern::OnDragUpdate_Implementation(FVector WorldPos)
{
	FVector Desired(WorldPos.X, WorldPos.Y, LockedZ);

	// Clamp to rectangle bounds if enabled
	if (bUseBounds)
	{
		Desired.X = FMath::Clamp(Desired.X, BoundsMin.X, BoundsMax.X);
		Desired.Y = FMath::Clamp(Desired.Y, BoundsMin.Y, BoundsMax.Y);
	}

	SetActorLocation(Desired, true);  // sweep = true, stops at collision
}

void AUmbraLantern::OnDragEnd_Implementation()
{
	if (LanternSound)
	{
		UGameplayStatics::SpawnSound2D(this, LanternSound, 1.f, 1.f, 0.35f);
	}

	// Hide bounds plane when drag ends
	if (BoundsPlane)
	{
		BoundsPlane->SetVisibility(false);
	}
}

void AUmbraLantern::UpdateBoundsPlane()
{
	if (!BoundsPlane) return;

	// Center of the bounds rectangle
	const float CenterX = (BoundsMin.X + BoundsMax.X) * 0.5f;
	const float CenterY = (BoundsMin.Y + BoundsMax.Y) * 0.5f;

	// Size of the bounds rectangle
	const float SizeX = BoundsMax.X - BoundsMin.X;
	const float SizeY = BoundsMax.Y - BoundsMin.Y;

	// Engine plane is 100x100 units, so scale accordingly
	const float ScaleX = SizeX / 100.f;
	const float ScaleY = SizeY / 100.f;

	// Place slightly below the lantern so it doesn't z-fight with the island
	BoundsPlane->SetWorldLocation(FVector(CenterX, CenterY, LockedZ - 78.f));
	BoundsPlane->SetWorldScale3D(FVector(ScaleX, ScaleY, 1.f));
}
