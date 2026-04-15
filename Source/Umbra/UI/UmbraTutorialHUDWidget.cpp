// Umbra - Light & Shadow Puzzle Game

#include "UmbraTutorialHUDWidget.h"
#include "UmbraTutorialGameMode.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UUmbraTutorialHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Start with tutorial text hidden
	if (TutorialText)
	{
		TutorialText->SetVisibility(ESlateVisibility::Hidden);
	}

	// Bind skip button click
	if (SkipButton)
	{
		SkipButton->OnClicked.AddDynamic(this, &UUmbraTutorialHUDWidget::OnSkipClicked);
	}

	if (PauseButton)
	{
		PauseButton->OnClicked.AddDynamic(this, &UUmbraTutorialHUDWidget::OnPauseClicked);
	}
}

void UUmbraTutorialHUDWidget::ShowTutorialText(const FText& Message)
{
	if (TutorialText)
	{
		TutorialText->SetText(Message);
		TutorialText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUmbraTutorialHUDWidget::HideTutorialText()
{
	if (TutorialText)
	{
		TutorialText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUmbraTutorialHUDWidget::OnSkipClicked()
{
	AUmbraTutorialGameMode* GM = Cast<AUmbraTutorialGameMode>(
		UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->SkipTutorial();
	}
}

void UUmbraTutorialHUDWidget::OnPauseClicked()
{
	if (AUmbraTutorialGameMode* GM = GetWorld()->GetAuthGameMode<AUmbraTutorialGameMode>())
	{
		GM->TogglePause();
	}
}
