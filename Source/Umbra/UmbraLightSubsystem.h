// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UmbraLightSubsystem.generated.h"

class UPointLightComponent;

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
	void RegisterLight(UPointLightComponent* Light);
	void UnregisterLight(UPointLightComponent* Light);

	const TArray<TWeakObjectPtr<UPointLightComponent>>& GetLights() const { return Lights; }

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UPointLightComponent>> Lights;
};
