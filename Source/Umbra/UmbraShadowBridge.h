// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraShadowBridge.generated.h"

class UBoxComponent;

/**
 *  Invisible walkable surface that only exists while the player is in shadow.
 *  Place over void gaps; collision is toggled by UmbraPawn's shadow check.
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

	bool bBridgeEnabled = false;
};
