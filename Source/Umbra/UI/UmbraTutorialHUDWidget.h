// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraTutorialHUDWidget.generated.h"

class UTextBlock;
class UButton;

/**
 *  HUD widget for the tutorial level.
 *  Displays tutorial instructions and a skip button.
 *
 *  In the Widget Blueprint, create widgets with these exact names:
 *    - TutorialText  (Text block)
 *    - SkipButton    (Button)
 */
UCLASS()
class UUmbraTutorialHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Show a tutorial instruction message on screen. */
	void ShowTutorialText(const FText& Message);

	/** Hide the current tutorial message. */
	void HideTutorialText();

protected:
	virtual void NativeConstruct() override;

private:
	/** Bound to a TextBlock named "TutorialText" in the Widget Blueprint. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TutorialText;

	/** Bound to a Button named "SkipButton" in the Widget Blueprint. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SkipButton;

	/** Bound to a Button named "PauseButton" in the Widget Blueprint (optional). */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> PauseButton;

	UFUNCTION()
	void OnSkipClicked();

	UFUNCTION()
	void OnPauseClicked();
};
