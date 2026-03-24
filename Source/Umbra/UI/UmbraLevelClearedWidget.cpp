// Umbra - Light & Shadow Puzzle Game

#include "UmbraLevelClearedWidget.h"
#include "UmbraPuzzleGameMode.h"
#include "Kismet/GameplayStatics.h"

void UUmbraLevelClearedWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UUmbraLevelClearedWidget::OnExitToMenuClicked()
{
    // Return to the main menu level
    UGameplayStatics::OpenLevel(this, FName(TEXT("Level_MainMenu")));
}

void UUmbraLevelClearedWidget::OnNextLevelClicked()
{
    AUmbraPuzzleGameMode* GM = Cast<AUmbraPuzzleGameMode>(
        UGameplayStatics::GetGameMode(this));
    if (GM && !GM->GetNextLevelName().IsNone())
    {
        UGameplayStatics::OpenLevel(this, GM->GetNextLevelName());
    }
}