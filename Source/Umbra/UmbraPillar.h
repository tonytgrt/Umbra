// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraPillar.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPillarSlideFinished, AUmbraPillar*, Pillar);

/**
 *  A blocking object that slides between designer-specified stop positions.
 *  Driven by pressure plates; not directly interactable by mouse.
 *  Casts shadows that form physical terrain in the Umbra puzzle system.
 */
UCLASS()
class AUmbraPillar : public AActor
{
	GENERATED_BODY()

public:
	AUmbraPillar();

	virtual void Tick(float DeltaSeconds) override;

	// ©¤©¤ Public API (called by pressure plates) ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤

	/** Command the pillar to slide to a specific stop index. Does nothing if already sliding. */
	UFUNCTION(BlueprintCallable, Category = "Umbra|Pillar")
	void MoveToStop(int32 StopIndex);

	/** Advance to the next stop position (wraps from last back to 0). */
	UFUNCTION(BlueprintCallable, Category = "Umbra|Pillar")
	void AdvanceToNextStop();

	/** Is the pillar currently mid-slide? */
	UFUNCTION(BlueprintPure, Category = "Umbra|Pillar")
	bool IsSliding() const { return bIsSliding; }

	/** Which stop index the pillar is currently at (or was last at before sliding). */
	UFUNCTION(BlueprintPure, Category = "Umbra|Pillar")
	int32 GetCurrentStopIndex() const { return CurrentStopIndex; }

	/** Immediately teleport back to stop 0 and reset all state. Used by level reset. */
	UFUNCTION(BlueprintCallable, Category = "Umbra|Pillar")
	void ResetToInitial();

	// ©¤©¤ Delegates ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤

	/** Broadcast when the pillar finishes arriving at a stop. */
	UPROPERTY(BlueprintAssignable, Category = "Umbra|Pillar")
	FOnPillarSlideFinished OnSlideFinished;

protected:
	virtual void BeginPlay() override;

private:
	// ©¤©¤ Components ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤

	UPROPERTY(VisibleAnywhere, Category = "Umbra|Pillar")
	TObjectPtr<UStaticMeshComponent> PillarMesh;

	// ©¤©¤ Designer-specified data (set per-instance in the editor) ©¤©¤©¤©¤

	/**
	 * Ordered list of stop positions.
	 * These are RELATIVE offsets from the actor's placed location.
	 * Entry 0 should usually be (0,0,0) = the actor's placed position.
	 * The MakeEditWidget meta tag renders each entry as a draggable 3D
	 * diamond widget in the viewport so you can position stops visually.
	 */
	UPROPERTY(EditInstanceOnly, Category = "Umbra|Pillar",
		meta = (MakeEditWidget = true))
	TArray<FVector> StopPositions;

	/** How fast the pillar slides in Unreal Units per second. */
	UPROPERTY(EditAnywhere, Category = "Umbra|Pillar",
		meta = (ClampMin = "50.0", ClampMax = "5000.0"))
	float SlideSpeed = 400.f;

	// ©¤©¤ Runtime state (not visible in editor) ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤

	/** World-space stop positions, computed from relative offsets in BeginPlay. */
	TArray<FVector> WorldStopPositions;

	int32 CurrentStopIndex = 0;
	int32 TargetStopIndex = 0;
	bool bIsSliding = false;
};