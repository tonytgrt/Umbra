// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraHUDWidget.generated.h"

/**
 *  In-game HUD widget that displays contextual help text.
 *  The visual layout (font, position, animation) is done in a Widget Blueprint.
 */
UCLASS(Abstract)
class UUmbraHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     *  Show a help/hint message on screen.
     *  Implement the visual presentation (fade in, text block update) in Blueprint.
     *  @param Message  The help text to display.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void ShowHelpText(const FText& Message);

    /**
     *  Hide the currently displayed help text.
     *  Implement fade-out or instant hide in Blueprint.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void HideHelpText();

    /**
     *  Update the objective/instruction line shown persistently.
     *  @param Objective  Short objective string, e.g. "Reach the exit"
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void SetObjectiveText(const FText& Objective);
};