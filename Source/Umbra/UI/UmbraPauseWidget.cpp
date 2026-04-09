#include "UmbraPauseWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UUmbraPauseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UUmbraPauseWidget::OnResumeClicked);
    }
    if (ExitToMenuButton)
    {
        ExitToMenuButton->OnClicked.AddDynamic(this, &UUmbraPauseWidget::OnExitToMenuClicked);
    }
}

void UUmbraPauseWidget::OnResumeClicked()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    // Unpause
    UGameplayStatics::SetGamePaused(this, false);

    // Restore game + UI input
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);

    // Remove this widget
    RemoveFromParent();
}

void UUmbraPauseWidget::OnExitToMenuClicked()
{
    UGameplayStatics::SetGamePaused(this, false);
    UGameplayStatics::OpenLevel(this, FName(TEXT("Level_MainMenu")));
}