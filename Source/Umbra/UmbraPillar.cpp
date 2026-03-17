// Umbra - Light & Shadow Puzzle Game

#include "UmbraPillar.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Umbra.h"

AUmbraPillar::AUmbraPillar()
{
	PrimaryActorTick.bCanEverTick = true;

	// Box collision (blocks visibility traces so it casts "shadow")
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetBoxExtent(FVector(30.f, 30.f, 100.f));
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

void AUmbraPillar::BeginPlay()
{
	Super::BeginPlay();

	// Convert relative StopPositions to world space
	FVector Origin = GetActorLocation();

	if (StopPositions.Num() == 0)
	{
		// Safety fallback: use current location as sole stop
		WorldStopPositions.Add(Origin);
		UE_LOG(LogUmbra, Warning,
			TEXT("UmbraPillar '%s': No stop positions configured  -  using spawn location as sole stop."),
			*GetName());
	}
	else
	{
		WorldStopPositions.Reserve(StopPositions.Num());
		for (const FVector& RelPos : StopPositions)
		{
			WorldStopPositions.Add(Origin + RelPos);
		}
	}

	// Snap to stop 0
	CurrentStopIndex = 0;
	TargetStopIndex = 0;
	SetActorLocation(WorldStopPositions[0]);
}

void AUmbraPillar::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsSliding)
	{
		return;
	}

	// Slide toward target at constant speed
	FVector Current = GetActorLocation();
	FVector Target = WorldStopPositions[TargetStopIndex];

	FVector NewLocation = FMath::VInterpConstantTo(Current, Target, DeltaSeconds, SlideSpeed);
	SetActorLocation(NewLocation, /*bSweep=*/true);

	// Check arrival
	if (FVector::DistSquared(NewLocation, Target) < 1.f)
	{
		SetActorLocation(Target, /*bSweep=*/false);
		bIsSliding = false;
		CurrentStopIndex = TargetStopIndex;

		UE_LOG(LogUmbra, Log,
			TEXT("UmbraPillar '%s': Arrived at stop %d"),
			*GetName(), CurrentStopIndex);

		OnSlideFinished.Broadcast(this);
	}
}

void AUmbraPillar::MoveToStop(int32 StopIndex)
{
	if (!WorldStopPositions.IsValidIndex(StopIndex))
	{
		UE_LOG(LogUmbra, Warning,
			TEXT("UmbraPillar '%s': Invalid stop index %d (have %d stops)"),
			*GetName(), StopIndex, WorldStopPositions.Num());
		return;
	}

	if (bIsSliding)
	{
		UE_LOG(LogUmbra, Log,
			TEXT("UmbraPillar '%s': Already sliding  -  ignoring MoveToStop(%d)"),
			*GetName(), StopIndex);
		return;
	}

	if (StopIndex == CurrentStopIndex)
	{
		return;
	}

	TargetStopIndex = StopIndex;
	bIsSliding = true;

	UE_LOG(LogUmbra, Log,
		TEXT("UmbraPillar '%s': Sliding from stop %d  ->  stop %d"),
		*GetName(), CurrentStopIndex, StopIndex);
}

void AUmbraPillar::AdvanceToNextStop()
{
	int32 NextIndex = (CurrentStopIndex + 1) % WorldStopPositions.Num();
	MoveToStop(NextIndex);
}

void AUmbraPillar::ResetToInitial()
{
	bIsSliding = false;
	CurrentStopIndex = 0;
	TargetStopIndex = 0;
	if (WorldStopPositions.IsValidIndex(0))
	{
		SetActorLocation(WorldStopPositions[0]);
	}
}
