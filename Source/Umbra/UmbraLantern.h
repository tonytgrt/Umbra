// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraInteractable.h"
#include "UmbraLantern.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class USphereComponent;

/**
 *  Draggable point light source for Umbra puzzles.
 *  Player clicks and drags to reposition on the XY plane.
 */
UCLASS()
class AUmbraLantern : public AActor, public IUmbraInteractable
{
	GENERATED_BODY()

public:
	AUmbraLantern();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// --- IUmbraInteractable ---
	virtual void OnDragStart_Implementation(FVector WorldPos) override;
	virtual void OnDragUpdate_Implementation(FVector WorldPos) override;
	virtual void OnDragEnd_Implementation() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> LanternMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> PointLight;

	/** Lock the lantern's Z position so it stays on the same plane */
	float LockedZ;

	/** If true, clamp lantern movement to the MoveBounds rectangle */
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bUseBounds = false;

	/** Min corner of the allowed movement area (X, Y) in world space */
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (EditCondition = "bUseBounds"))
	FVector2D BoundsMin = FVector2D(-500.f, -500.f);

	/** Max corner of the allowed movement area (X, Y) in world space */
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (EditCondition = "bUseBounds"))
	FVector2D BoundsMax = FVector2D(500.f, 500.f);

	/** Plane mesh that shows the movement boundary while dragging */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BoundsPlane;

	/** Material for the bounds plane (assign in Blueprint or editor) */
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (EditCondition = "bUseBounds"))
	TObjectPtr<UMaterialInterface> BoundsPlaneMaterial;

	/** Update the bounds plane position and scale to match BoundsMin/BoundsMax */
	void UpdateBoundsPlane();
};
