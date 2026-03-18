// Umbra - Light & Shadow Puzzle Game

#include "UmbraMainMenuGameMode.h"
#include "UmbraMainMenuWidget.h"
#include "Blueprint/UserWidget.h"

AUmbraMainMenuGameMode::AUmbraMainMenuGameMode()
{
    // No default pawn needed on the menu screen
    DefaultPawnClass = nullptr;
}

void AUmbraMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuWidgetClass)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            MainMenuWidget = CreateWidget<UUmbraMainMenuWidget>(PC, MainMenuWidgetClass);
            if (MainMenuWidget)
            {
                MainMenuWidget->AddToViewport();

                // Show mouse cursor and set UI-only input mode
                PC->bShowMouseCursor = true;
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PC->SetInputMode(InputMode);
            }
        }
    }
}