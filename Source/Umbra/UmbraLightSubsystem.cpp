// Umbra - Light & Shadow Puzzle Game

#include "UmbraLightSubsystem.h"
#include "Components/LightComponent.h"
#include "UmbraShadowBridge.h"

void UUmbraLightSubsystem::RegisterLight(ULightComponent* Light)
{
	if (Light)
	{
		Lights.AddUnique(Light);
	}
}

void UUmbraLightSubsystem::UnregisterLight(ULightComponent* Light)
{
	Lights.RemoveSwap(Light);
}

void UUmbraLightSubsystem::RegisterBridge(AUmbraShadowBridge* Bridge)
{
	if (Bridge)
	{
		Bridges.AddUnique(Bridge);
	}
}

void UUmbraLightSubsystem::UnregisterBridge(AUmbraShadowBridge* Bridge)
{
	Bridges.RemoveSwap(Bridge);
}
