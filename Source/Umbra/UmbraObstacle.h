// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraObstacle.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 *  Obstacle that exists when lit and disappears when in shadow.
 *  Each tick it checks whether any AUmbraLantern illuminates it
 *  (within attenuation radius + unobstructed line of sight).
 */
UCLASS()
class UMBRA_API AUmbraObstacle : public AActor
{
	GENERATED_BODY()

public:
	AUmbraObstacle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	/** Returns true if any lantern in the level is lighting this obstacle. */
	bool IsLit() const;

	/** Show or hide the obstacle (collision + visibility). */
	void SetObstacleActive(bool bActive);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ObstacleMesh;

	/** Current active state so we only toggle on change. */
	bool bCurrentlyActive;

	/** Original world location when fully raised. */
	FVector OriginalLocation;

	/** How far below ground the obstacle sinks. */
	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float SinkDepth = 400.f;

	/** How fast the obstacle rises/sinks (units per second). */
	UPROPERTY(EditAnywhere, Category = "Obstacle")
	float MoveSpeed = 200.f;

	/** Target Z offset: 0 when lit, -SinkDepth when in shadow. */
	float TargetZOffset;

	/** Current Z offset from original location. */
	float CurrentZOffset;
};
