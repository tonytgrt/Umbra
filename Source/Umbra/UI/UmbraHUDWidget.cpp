// Umbra - Light & Shadow Puzzle Game

#include "UmbraHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UmbraPuzzleGameMode.h"
#include "UmbraTutorialGameMode.h"

void UUmbraHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (PauseButton)
    {
        PauseButton->OnClicked.AddDynamic(this, &UUmbraHUDWidget::OnPauseClicked);
    }
}

void UUmbraHUDWidget::SetLevelName(const FText& Name)
{
    if (Level)
    {
        Level->SetText(Name);
    }
}

void UUmbraHUDWidget::OnPauseClicked()
{
    if (AUmbraPuzzleGameMode* GM = GetWorld()->GetAuthGameMode<AUmbraPuzzleGameMode>())
    {
        GM->TogglePause();
    }
    else if (AUmbraTutorialGameMode* TutGM = GetWorld()->GetAuthGameMode<AUmbraTutorialGameMode>())
    {
        TutGM->TogglePause();
    }
}