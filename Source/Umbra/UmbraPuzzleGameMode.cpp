// Umbra - Light & Shadow Puzzle Game

#include "UmbraPuzzleGameMode.h"
#include "UmbraPawn.h"
#include "UmbraPawnController.h"
#include "UmbraHUDWidget.h"
#include "UmbraLevelClearedWidget.h"
#include "Umbra.h"
#include "Blueprint/UserWidget.h"


AUmbraPuzzleGameMode::AUmbraPuzzleGameMode()
{
	DefaultPawnClass = AUmbraPawn::StaticClass();
	PlayerControllerClass = AUmbraPawnController::StaticClass();
}

void AUmbraPuzzleGameMode::ResetLevel()
{
	// TODO: respawn pawn, reset all actors to initial state
	UE_LOG(LogUmbra, Log, TEXT("ResetLevel called"));
}

void AUmbraPuzzleGameMode::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    // Create the in-game HUD
    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<UUmbraHUDWidget>(PC, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport(0);
        }
    }

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    PC->bShowMouseCursor = true;
    PC->SetInputMode(InputMode);
}

void AUmbraPuzzleGameMode::OnLevelCleared()
{
    UE_LOG(LogUmbra, Log, TEXT("Level cleared!"));

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    // Hide the HUD
    if (HUDWidget)
    {
        HUDWidget->RemoveFromParent();
    }

    // Show the level-cleared screen
    if (LevelClearedWidgetClass)
    {
        LevelClearedWidget = CreateWidget<UUmbraLevelClearedWidget>(PC, LevelClearedWidgetClass);
        if (LevelClearedWidget)
        {
            LevelClearedWidget->AddToViewport(10);

            // Switch to UI input so the player can click the exit button
            PC->bShowMouseCursor = true;
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(LevelClearedWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
        }
    }
}
