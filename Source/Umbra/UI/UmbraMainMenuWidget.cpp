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
    // Open the tutorial level
    UGameplayStatics::OpenLevel(this, FName(TEXT("Level_Tutorial")));
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