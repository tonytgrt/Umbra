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

    /** Get the next level name. Empty means there is no next level. */
    UFUNCTION(BlueprintCallable, Category = "Puzzle")
    FName GetNextLevelName() const { return NextLevelName; }

protected:
    virtual void BeginPlay() override;

private:
    /** Widget Blueprint class for the in-game HUD. Set in the BP child. */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUmbraHUDWidget> HUDWidgetClass;

    /** Widget Blueprint class for the level-cleared screen. Set in the BP child. */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUmbraLevelClearedWidget> LevelClearedWidgetClass;

    /** Name of the next level to load. Leave empty for the last level (menu only). */
    UPROPERTY(EditDefaultsOnly, Category = "Puzzle")
    FName NextLevelName;

    /** Display name shown on the HUD, e.g. "Level 1", "Level 2". */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    FText LevelDisplayName;

    UPROPERTY()
    TObjectPtr<UUmbraHUDWidget> HUDWidget;

    UPROPERTY()
    TObjectPtr<UUmbraLevelClearedWidget> LevelClearedWidget;
};
