// Umbra - Light & Shadow Puzzle Game

#include "UmbraPillar.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Umbra.h"

AUmbraPillar::AUmbraPillar()
{
	PrimaryActorTick.bCanEverTick = true;

	// ©¤©¤ Root mesh ¡ª provides collision and casts shadows ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	SetRootComponent(PillarMesh);

	PillarMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	PillarMesh->SetSimulatePhysics(false);
	PillarMesh->SetCastShadow(true);  // shadow casting is critical for the puzzle

	// Placeholder cube mesh ¡ª you will swap this for a real pillar model in the editor
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PillarMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void AUmbraPillar::BeginPlay()
{
	Super::BeginPlay();

	// ©¤©¤ Convert relative StopPositions to world space ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	//
	// In the editor, StopPositions are entered as offsets from the actor's
	// placed location (because of MakeEditWidget). We convert them once at
	// BeginPlay so all runtime code uses world coordinates.

	FVector Origin = GetActorLocation();

	if (StopPositions.Num() == 0)
	{
		// Safety fallback: if the designer forgot to add stops, use current location
		WorldStopPositions.Add(Origin);
		UE_LOG(LogUmbra, Warning,
			TEXT("UmbraPillar '%s': No stop positions configured ¡ª using spawn location as sole stop."),
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

	// ©¤©¤ Slide toward target at constant speed ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	FVector Current = GetActorLocation();
	FVector Target = WorldStopPositions[TargetStopIndex];

	// VInterpConstantTo moves at exactly SlideSpeed UU/s, frame-rate independent
	FVector NewLocation = FMath::VInterpConstantTo(Current, Target, DeltaSeconds, SlideSpeed);

	// Sweep = true so the pillar stops if it hits unexpected geometry
	SetActorLocation(NewLocation, /*bSweep=*/true);

	// ©¤©¤ Check arrival ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	if (FVector::DistSquared(NewLocation, Target) < 1.f)
	{
		// Snap to exact position to avoid floating-point drift
		SetActorLocation(Target, /*bSweep=*/false);
		bIsSliding = false;
		CurrentStopIndex = TargetStopIndex;

		UE_LOG(LogUmbra, Log,
			TEXT("UmbraPillar '%s': Arrived at stop %d"),
			*GetName(), CurrentStopIndex);

		// Notify anyone listening (pressure plates, game mode, sound system, etc.)
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
			TEXT("UmbraPillar '%s': Already sliding ¡ª ignoring MoveToStop(%d)"),
			*GetName(), StopIndex);
		return;
	}

	if (StopIndex == CurrentStopIndex)
	{
		return; // already there, nothing to do
	}

	TargetStopIndex = StopIndex;
	bIsSliding = true;

	UE_LOG(LogUmbra, Log,
		TEXT("UmbraPillar '%s': Sliding from stop %d ¡ú stop %d"),
		*GetName(), CurrentStopIndex, StopIndex);
}

void AUmbraPillar::AdvanceToNextStop()
{
	// Wrap around: 0 ¡ú 1 ¡ú 2 ¡ú ... ¡ú N-1 ¡ú 0
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