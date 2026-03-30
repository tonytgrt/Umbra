// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraShadowBridge.generated.h"

class UBoxComponent;
class UInstancedStaticMeshComponent;

/**
 *  Walkable surface that only exists while the player is in shadow.
 *  Place over void gaps; collision is toggled by UmbraPawn's shadow check.
 *  Visualised with small translucent tiles spawned only at shadowed grid cells.
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

	/** Instanced mesh that renders a small tile at each shadowed grid cell. */
	UPROPERTY(VisibleAnywhere, Category = "Umbra|ShadowBridge")
	TObjectPtr<UInstancedStaticMeshComponent> ShadowTiles;

	/** Base material for the bridge tiles (should be translucent with "Opacity" and "BaseColor" params). */
	UPROPERTY(EditAnywhere, Category = "Umbra|ShadowBridge")
	TObjectPtr<UMaterialInterface> BridgeMaterial;

	/** Dynamic material instance created at runtime. */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMaterial;

	/** Opacity of the shadow tiles. */
	UPROPERTY(EditAnywhere, Category = "Umbra|ShadowBridge", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float TileOpacity = 1.0f;

	/** Color tint of the shadow tiles. */
	UPROPERTY(EditAnywhere, Category = "Umbra|ShadowBridge")
	FLinearColor BridgeColor = FLinearColor(0.1f, 0.15f, 0.4f, 1.f);

	bool bBridgeEnabled = false;

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

	/** Rebuild instanced mesh tiles from CachedShadowPositions. */
	void RebuildTiles();

	/** Line-trace grid across the bridge surface, populating CachedShadowPositions. */
	void SampleShadowPositions();
};
