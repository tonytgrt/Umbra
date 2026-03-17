// Umbra - Light & Shadow Puzzle Game

#include "UmbraLightSubsystem.h"
#include "Components/PointLightComponent.h"

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
