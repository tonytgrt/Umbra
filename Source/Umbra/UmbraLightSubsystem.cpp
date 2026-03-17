// Umbra - Light & Shadow Puzzle Game

#include "UmbraLightSubsystem.h"
#include "Components/PointLightComponent.h"
#include "UmbraShadowBridge.h"

void UUmbraLightSubsystem::RegisterLight(UPointLightComponent* Light)
{
	if (Light)
	{
		Lights.AddUnique(Light);
	}
}

void UUmbraLightSubsystem::UnregisterLight(UPointLightComponent* Light)
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
