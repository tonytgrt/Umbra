// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UmbraInteractable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UUmbraInteractable : public UInterface
{
	GENERATED_BODY()
};

class IUmbraInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Umbra")
	void OnDragStart(FVector WorldPos);

	UFUNCTION(BlueprintNativeEvent, Category = "Umbra")
	void OnDragUpdate(FVector WorldPos);

	UFUNCTION(BlueprintNativeEvent, Category = "Umbra")
	void OnDragEnd();
};
