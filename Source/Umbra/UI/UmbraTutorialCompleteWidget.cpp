// Umbra - Light & Shadow Puzzle Game

#include "UmbraTutorialCompleteWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UUmbraTutorialCompleteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UUmbraTutorialCompleteWidget::OnContinueClicked);
	}
}

void UUmbraTutorialCompleteWidget::OnContinueClicked()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("Level_1")));
}
