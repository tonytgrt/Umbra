// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraTutorialCompleteWidget.generated.h"

class UButton;

/**
 *  Widget shown when the player completes the tutorial.
 *  Displays "Tutorial Complete" and a Continue button to go to Level_1.
 *
 *  In the Widget Blueprint, create a Button named "ContinueButton".
 */
UCLASS()
class UUmbraTutorialCompleteWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UFUNCTION()
	void OnContinueClicked();
};
