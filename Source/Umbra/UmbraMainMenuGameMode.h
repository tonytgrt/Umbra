// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UmbraMainMenuGameMode.generated.h"

class UUmbraMainMenuWidget;

/**
 *  Game mode for the main menu level.
 *  Creates and displays the main menu widget on BeginPlay.
 */
UCLASS()
class AUmbraMainMenuGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AUmbraMainMenuGameMode();

protected:
    virtual void BeginPlay() override;

private:
    /** Widget Blueprint class to spawn (set in BP child or DefaultPawnClass). */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUmbraMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UUmbraMainMenuWidget> MainMenuWidget;
};