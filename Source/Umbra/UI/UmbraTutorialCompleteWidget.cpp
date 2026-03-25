// Umbra - Light & Shadow Puzzle Game

#include "UmbraTutorialCompleteWidget.h"
#include "UmbraTutorialGameMode.h"
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
	FName Target = TEXT("Level_1");

	if (AUmbraTutorialGameMode* GM = Cast<AUmbraTutorialGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		Target = GM->GetNextLevelName();
	}

	UGameplayStatics::OpenLevel(this, Target);
}
