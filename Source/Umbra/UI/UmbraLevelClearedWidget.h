// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraLevelClearedWidget.generated.h"

/**
 *  Widget shown when the player completes a puzzle level.
 *  Displays "Level Cleared" and an exit-to-menu button.
 */
UCLASS(Abstract)
class UUmbraLevelClearedWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    /** Called when the player clicks the Exit button to return to main menu. */
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OnExitToMenuClicked();
};