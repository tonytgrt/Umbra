// Umbra - Light & Shadow Puzzle Game

#include "UmbraLevelClearedWidget.h"
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