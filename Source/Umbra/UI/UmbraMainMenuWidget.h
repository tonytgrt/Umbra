// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraMainMenuWidget.generated.h"

/**
 *  Main menu widget for Umbra.
 *  Displays game title, Start, and Exit buttons.
 *  Layout is done in a Widget Blueprint child of this class.
 */
UCLASS(Abstract)
class UUmbraMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    /** Called when the player clicks Start. Override in Blueprint if needed. */
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OnStartClicked();

    /** Called when the player clicks Exit. Override in Blueprint if needed. */
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OnExitClicked();
};