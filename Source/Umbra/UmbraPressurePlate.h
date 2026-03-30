// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraPressurePlate.generated.h"

class AUmbraPillar;
class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;

/**
 * How the plate behaves when the player steps on it.
 * Set per-instance in the editor Details panel.
 */
UENUM(BlueprintType)
enum class EPressurePlateMode : uint8
{
	/** Each time the pawn steps on, the pillar(s) advance to the next stop. */
	Advance,

	/** First step moves pillar(s) to TargetStopIndex. Second step moves them back to 0. Repeats. */
	Toggle,

	/** Fires once, then permanently deactivates for the rest of the level. */
	OneShot
};

/**
 * Floor trigger that moves linked UmbraPillar(s) when the player pawn walks over it.
 * The trigger uses a UBoxComponent overlap volume �� the pawn walks through it, not into it.
 */
UCLASS()
class AUmbraPressurePlate : public AActor
{
	GENERATED_BODY()

public:
	AUmbraPressurePlate();

	/** Reset the plate's fired/toggle state. Used by level reset. */
	UFUNCTION(BlueprintCallable, Category = "Umbra|PressurePlate")
	void ResetToInitial();

protected:
	virtual void BeginPlay() override;

	/** Called when any actor enters the trigger box. */
	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32                OtherBodyIndex,
		bool                 bFromSweep,
		const FHitResult& SweepResult);

	/** Called when any actor exits the trigger box. */
	UFUNCTION()
	void OnTriggerEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32                OtherBodyIndex);

private:
	// ���� Components ����������������������������������������������������������������������������������������������������

	/** Invisible overlap box that detects the pawn walking over the plate. */
	UPROPERTY(VisibleAnywhere, Category = "Umbra|PressurePlate")
	TObjectPtr<UBoxComponent> TriggerBox;

	/** Visible mesh representing the pressure plate on the ground. */
	UPROPERTY(VisibleAnywhere, Category = "Umbra|PressurePlate")
	TObjectPtr<UStaticMeshComponent> PlateMesh;

	// ���� Designer-specified data (set per-instance in the editor) ��������

	/**
	 * Which pillar(s) this plate controls.
	 * Use the eyedropper or dropdown in the Details panel to pick actors
	 * already placed in the level.
	 */
	UPROPERTY(EditInstanceOnly, Category = "Umbra|PressurePlate")
	TArray<TObjectPtr<AUmbraPillar>> LinkedPillars;

	/** How the plate behaves. See EPressurePlateMode for descriptions. */
	UPROPERTY(EditAnywhere, Category = "Umbra|PressurePlate")
	EPressurePlateMode PlateMode = EPressurePlateMode::Advance;

	/**
	 * (Toggle mode only) Which stop index to send pillars to on first activation.
	 * Second activation sends them back to stop 0.
	 * This field is only visible in the Details panel when PlateMode == Toggle.
	 */
	UPROPERTY(EditAnywhere, Category = "Umbra|PressurePlate",
		meta = (EditCondition = "PlateMode == EPressurePlateMode::Toggle",
			EditConditionHides))
	int32 ToggleTargetStopIndex = 1;

	// ���� Runtime state ����������������������������������������������������������������������������������������������

	bool bHasFired = false;
	bool bIsToggled = false;

	// ���� Internal ��������������������������������������������������������������������������������������������������������

	void ActivatePlate();

	/** Sound played when the pressure plate is activated. */
	UPROPERTY(EditAnywhere, Category = "Umbra|Audio")
	TObjectPtr<USoundBase> PressurePlateSound;
};