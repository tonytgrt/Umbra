// Umbra - Light & Shadow Puzzle Game

#include "UmbraLantern.h"
#include "UmbraLightSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "UObject/ConstructorHelpers.h"

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
	LockedZ = GetActorLocation().Z;
}

void AUmbraLantern::OnDragUpdate_Implementation(FVector WorldPos)
{
	FVector Desired(WorldPos.X, WorldPos.Y, LockedZ);
	SetActorLocation(Desired, true);  // sweep = true, stops at collision
}

void AUmbraLantern::OnDragEnd_Implementation()
{
	// Nothing needed for now
}
