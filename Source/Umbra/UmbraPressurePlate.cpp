// Umbra - Light & Shadow Puzzle Game

#include "UmbraPressurePlate.h"
#include "UmbraPillar.h"
#include "UmbraPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Umbra.h"

AUmbraPressurePlate::AUmbraPressurePlate()
{
	PrimaryActorTick.bCanEverTick = false; // no tick needed ¡ª event-driven

	// ©¤©¤ Trigger volume ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	//
	// This is the invisible collision box that detects the pawn.
	// It uses the OverlapAllDynamic profile, which means:
	//   - It generates overlap events (not blocking)
	//   - The pawn walks THROUGH it, not INTO it
	//   - The pawn's Capsule (profile "Pawn") overlaps with it by default

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(60.f, 60.f, 10.f)); // 120x120x20 UU flat box
	TriggerBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerBox->SetGenerateOverlapEvents(true);
	SetRootComponent(TriggerBox);

	// ©¤©¤ Visual mesh ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	//
	// Purely visual ¡ª no collision. Attached to the trigger box so it
	// moves with it if you reposition the actor.

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	PlateMesh->SetupAttachment(TriggerBox);
	PlateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlateMesh->SetCastShadow(false); // plates are flat on the ground, no meaningful shadow

	// Placeholder: a flattened cube. Replace with a proper plate mesh later.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PlateMesh->SetStaticMesh(CubeMesh.Object);
		// Scale X=1.2, Y=1.2 matches 120 UU footprint; Z=0.04 makes it very flat
		PlateMesh->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.04f));
	}
}

void AUmbraPressurePlate::BeginPlay()
{
	Super::BeginPlay();

	// ©¤©¤ Bind overlap events ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	TriggerBox->OnComponentBeginOverlap.AddDynamic(
		this, &AUmbraPressurePlate::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(
		this, &AUmbraPressurePlate::OnTriggerEndOverlap);

	// ©¤©¤ Validate designer setup ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	if (LinkedPillars.Num() == 0)
	{
		UE_LOG(LogUmbra, Warning,
			TEXT("UmbraPressurePlate '%s': No linked pillars assigned! "
				"Set LinkedPillars in the Details panel."),
			*GetName());
	}
}

void AUmbraPressurePlate::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32                OtherBodyIndex,
	bool                 bFromSweep,
	const FHitResult& SweepResult)
{
	// ©¤©¤ Only respond to the player pawn ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	// Cast to AUmbraPawn so that other actors (e.g. the pillar sliding
	// through) don't accidentally trigger the plate.

	if (!Cast<AUmbraPawn>(OtherActor))
	{
		return;
	}

	UE_LOG(LogUmbra, Log,
		TEXT("UmbraPressurePlate '%s': Pawn stepped on plate"), *GetName());

	ActivatePlate();
}

void AUmbraPressurePlate::OnTriggerEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32                OtherBodyIndex)
{
	if (!Cast<AUmbraPawn>(OtherActor))
	{
		return;
	}

	UE_LOG(LogUmbra, Log,
		TEXT("UmbraPressurePlate '%s': Pawn stepped off plate"), *GetName());

	// No action on exit for now.
	// A future "hold to keep active" mode could go here.
}

void AUmbraPressurePlate::ActivatePlate()
{
	// ©¤©¤ OneShot: already fired? ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	if (PlateMode == EPressurePlateMode::OneShot && bHasFired)
	{
		return;
	}

	bHasFired = true;

	// ©¤©¤ Tell each linked pillar to move ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
	for (AUmbraPillar* Pillar : LinkedPillars)
	{
		if (!Pillar)
		{
			UE_LOG(LogUmbra, Warning,
				TEXT("UmbraPressurePlate '%s': Null entry in LinkedPillars -- "
					"did you forget to assign it?"),
				*GetName());
			continue;
		}

		// Skip if this pillar is already mid-slide
		if (Pillar->IsSliding())
		{
			continue;
		}

		switch (PlateMode)
		{
		case EPressurePlateMode::Advance:
			Pillar->AdvanceToNextStop();
			break;

		case EPressurePlateMode::Toggle:
			if (!bIsToggled)
			{
				Pillar->MoveToStop(ToggleTargetStopIndex);
			}
			else
			{
				Pillar->MoveToStop(0);
			}
			break;

		case EPressurePlateMode::OneShot:
			Pillar->AdvanceToNextStop();
			break;
		}
	}

	// Flip toggle state AFTER dispatching moves
	if (PlateMode == EPressurePlateMode::Toggle)
	{
		bIsToggled = !bIsToggled;
	}
}

void AUmbraPressurePlate::ResetToInitial()
{
	bHasFired = false;
	bIsToggled = false;
}