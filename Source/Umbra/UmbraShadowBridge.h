// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraShadowBridge.generated.h"

class UBoxComponent;
class UNiagaraComponent;

/**
 *  Walkable surface that only exists while the player is in shadow.
 *  Place over void gaps; collision is toggled by UmbraPawn's shadow check.
 *  Visualised with a Niagara particle effect that fades in/out with activation.
 */
UCLASS()
class AUmbraShadowBridge : public AActor
{
	GENERATED_BODY()

public:
	AUmbraShadowBridge();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	void EnableBridge();
	void DisableBridge();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BridgeBox;

	/** Niagara effect for the shadow bridge visual. Assign the system asset in editor. */
	UPROPERTY(VisibleAnywhere, Category = "Umbra|ShadowBridge")
	TObjectPtr<UNiagaraComponent> BridgeEffect;

	bool bBridgeEnabled = false;

	/** Current activation blend (0 = dissolved, 1 = fully visible). */
	float ActivationAlpha = 0.f;

	/** Target activation blend. */
	float TargetActivationAlpha = 0.f;

	/** Transition speed (units per second). ~0.4s for full transition. */
	static constexpr float ActivationSpeed = 2.5f;

	/** Grid spacing for shadow sampling (Unreal Units). */
	UPROPERTY(EditAnywhere, Category = "Umbra|ShadowBridge", meta = (ClampMin = "10"))
	float SampleSpacing = 30.f;

	/** How far below the bridge surface to trace for solid ground (UU). */
	UPROPERTY(EditAnywhere, Category = "Umbra|ShadowBridge", meta = (ClampMin = "50"))
	float GroundTraceDepth = 500.f;

	/** How many frames to skip between shadow resamples. */
	UPROPERTY(EditAnywhere, Category = "Umbra|ShadowBridge", meta = (ClampMin = "1"))
	int32 SampleInterval = 5;

	/** Frame counter for throttled sampling. */
	int32 FrameCounter = 0;

	/** World-space positions that are in shadow over void. */
	TArray<FVector> CachedShadowPositions;

	/** Interpolate activation and push Niagara user parameters. */
	void UpdateEffectParameters(float DeltaSeconds);

	/** Line-trace grid across the bridge surface, populating CachedShadowPositions. */
	void SampleShadowPositions();
};
