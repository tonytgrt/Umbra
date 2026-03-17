// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraPillar.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPillarSlideFinished, AUmbraPillar*, Pillar);

/**
 *  A blocking pillar that casts "shadow" for the line-trace system.
 *  Slides between designer-specified stop positions when driven by pressure plates.
 */
UCLASS()
class UMBRA_API AUmbraPillar : public AActor
{
	GENERATED_BODY()

public:
	AUmbraPillar();

	virtual void Tick(float DeltaSeconds) override;

	// ── Public API (called by pressure plates) ──────────────────

	/** Command the pillar to slide to a specific stop index. */
	UFUNCTION(BlueprintCallable, Category = "Umbra|Pillar")
	void MoveToStop(int32 StopIndex);

	/** Advance to the next stop position (wraps from last back to 0). */
	UFUNCTION(BlueprintCallable, Category = "Umbra|Pillar")
	void AdvanceToNextStop();

	/** Is the pillar currently mid-slide? */
	UFUNCTION(BlueprintPure, Category = "Umbra|Pillar")
	bool IsSliding() const { return bIsSliding; }

	/** Which stop index the pillar is currently at. */
	UFUNCTION(BlueprintPure, Category = "Umbra|Pillar")
	int32 GetCurrentStopIndex() const { return CurrentStopIndex; }

	/** Immediately teleport back to stop 0 and reset all state. */
	UFUNCTION(BlueprintCallable, Category = "Umbra|Pillar")
	void ResetToInitial();

	// ── Delegates ───────────────────────────────────────────────

	/** Broadcast when the pillar finishes arriving at a stop. */
	UPROPERTY(BlueprintAssignable, Category = "Umbra|Pillar")
	FOnPillarSlideFinished OnSlideFinished;

protected:
	virtual void BeginPlay() override;

private:
	// ── Components ──────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, Category = "Umbra|Pillar")
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere, Category = "Umbra|Pillar")
	TObjectPtr<UStaticMeshComponent> PillarMesh;

	// ── Designer-specified data ─────────────────────────────────

	/**
	 * Ordered list of stop positions (RELATIVE offsets from placed location).
	 * Entry 0 should usually be (0,0,0).
	 */
	UPROPERTY(EditInstanceOnly, Category = "Umbra|Pillar",
		meta = (MakeEditWidget = true))
	TArray<FVector> StopPositions;

	/** How fast the pillar slides in Unreal Units per second. */
	UPROPERTY(EditAnywhere, Category = "Umbra|Pillar",
		meta = (ClampMin = "50.0", ClampMax = "5000.0"))
	float SlideSpeed = 400.f;

	// ── Runtime state ───────────────────────────────────────────

	TArray<FVector> WorldStopPositions;
	int32 CurrentStopIndex = 0;
	int32 TargetStopIndex = 0;
	bool bIsSliding = false;
};
