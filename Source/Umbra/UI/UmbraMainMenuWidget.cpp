// Umbra - Light & Shadow Puzzle Game

#include "UmbraMainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UUmbraMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UUmbraMainMenuWidget::OnStartClicked()
{
    // Open the first puzzle level
    UGameplayStatics::OpenLevel(this, FName(TEXT("Level_1")));
}

void UUmbraMainMenuWidget::OnExitClicked()
{
    UKismetSystemLibrary::QuitGame(
        this,
        GetOwningPlayer(),
        EQuitPreference::Quit,
        false
    );
}