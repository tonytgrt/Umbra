// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraPillar.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 *  Static pillar that blocks light from lanterns,
 *  casting "shadow" for the line-trace system.
 */
UCLASS()
class UMBRA_API AUmbraPillar : public AActor
{
	GENERATED_BODY()

public:
	AUmbraPillar();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PillarMesh;
};
