// Umbra - Light & Shadow Puzzle Game

#include "UmbraMainMenuGameMode.h"
#include "UmbraMainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

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

                // Hide hardware cursor — the lantern replaces it
                PC->bShowMouseCursor = false;

                // GameAndUI so mouse position updates for the lantern
                // while still allowing button clicks
                FInputModeGameAndUI InputMode;
                InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
                InputMode.SetHideCursorDuringCapture(false);
                PC->SetInputMode(InputMode);
            }
        }
    }

    // Set the view to the CameraActor placed in the level
    APlayerController* CamPC = GetWorld()->GetFirstPlayerController();
    if (CamPC)
    {
        TArray<AActor*> Cameras;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);
        if (Cameras.Num() > 0)
        {
            CamPC->SetViewTargetWithBlend(Cameras[0], 0.f);
        }
    }
}