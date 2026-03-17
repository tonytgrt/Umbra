// Umbra - Light & Shadow Puzzle Game

#include "UmbraPillar.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AUmbraPillar::AUmbraPillar()
{
	PrimaryActorTick.bCanEverTick = false;

	// Box collision (blocks visibility traces so it casts "shadow")
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(FVector(30.f, 30.f, 120.f));
	CollisionBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SetRootComponent(CollisionBox);

	// Visual mesh
	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	PillarMesh->SetupAttachment(CollisionBox);
	PillarMesh->SetRelativeLocation(FVector(-20.f, 0.f, -110.f));
	PillarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
		TEXT("/Game/Pillar/Meshes/Pillar_001/SM_Pillar_001.SM_Pillar_001"));
	if (Mesh.Succeeded())
	{
		PillarMesh->SetStaticMesh(Mesh.Object);
	}
}
