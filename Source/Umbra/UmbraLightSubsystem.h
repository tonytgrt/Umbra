// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UmbraLightSubsystem.generated.h"

class ULightComponent;
class AUmbraShadowBridge;

/**
 *  Tracks every shadow-casting light in the level.
 *  Lanterns register on BeginPlay and unregister on EndPlay.
 *  The pawn queries this subsystem each tick for shadow checks.
 */
UCLASS()
class UUmbraLightSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterLight(ULightComponent* Light);
	void UnregisterLight(ULightComponent* Light);

	const TArray<TWeakObjectPtr<ULightComponent>>& GetLights() const { return Lights; }

	void RegisterBridge(AUmbraShadowBridge* Bridge);
	void UnregisterBridge(AUmbraShadowBridge* Bridge);

	const TArray<TWeakObjectPtr<AUmbraShadowBridge>>& GetBridges() const { return Bridges; }

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<ULightComponent>> Lights;

	UPROPERTY()
	TArray<TWeakObjectPtr<AUmbraShadowBridge>> Bridges;
};
