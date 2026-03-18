// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UmbraPuzzleGameMode.generated.h"

class UUmbraHUDWidget;
class UUmbraLevelClearedWidget;

/**
 *  Game mode for Umbra puzzle levels.
 *  Sets UmbraPawn as default pawn and UmbraPawnController as player controller.
 */
UCLASS()
class AUmbraPuzzleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUmbraPuzzleGameMode();

	void ResetLevel();

    /** Call this when the player reaches the level exit. */
    UFUNCTION(BlueprintCallable, Category = "Puzzle")
    void OnLevelCleared();

    /** Get the HUD widget (e.g., to show help text from triggers). */
    UFUNCTION(BlueprintCallable, Category = "UI")
    UUmbraHUDWidget* GetHUDWidget() const { return HUDWidget; }

protected:
    virtual void BeginPlay() override;

private:
    /** Widget Blueprint class for the in-game HUD. Set in the BP child. */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUmbraHUDWidget> HUDWidgetClass;

    /** Widget Blueprint class for the level-cleared screen. Set in the BP child. */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUmbraLevelClearedWidget> LevelClearedWidgetClass;

    UPROPERTY()
    TObjectPtr<UUmbraHUDWidget> HUDWidget;

    UPROPERTY()
    TObjectPtr<UUmbraLevelClearedWidget> LevelClearedWidget;
};
