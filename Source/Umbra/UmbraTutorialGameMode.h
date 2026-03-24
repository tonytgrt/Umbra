// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UmbraTutorialGameMode.generated.h"

class UUmbraTutorialHUDWidget;
class UUmbraTutorialCompleteWidget;

/**
 *  A single tutorial step: when the orb's Y position crosses MinY,
 *  the message is shown. It hides when Y passes MaxY.
 */
USTRUCT(BlueprintType)
struct FTutorialStep
{
	GENERATED_BODY()

	/** Y value where this message starts showing. */
	UPROPERTY(EditAnywhere, Category = "Tutorial")
	float MinY = 0.f;

	/** Y value where this message stops showing. */
	UPROPERTY(EditAnywhere, Category = "Tutorial")
	float MaxY = 0.f;

	/** The tutorial text to display. */
	UPROPERTY(EditAnywhere, Category = "Tutorial")
	FText Message;

	/** If true, reaching MinY completes the tutorial instead of showing text. */
	UPROPERTY(EditAnywhere, Category = "Tutorial")
	bool bCompleteTutorial = false;
};

/**
 *  Game mode for the tutorial level.
 *  Uses UmbraPawn with a horizontal side-scrolling camera.
 *  Triggers tutorial messages based on the orb's Y position.
 */
UCLASS()
class AUmbraTutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUmbraTutorialGameMode();

	virtual void Tick(float DeltaSeconds) override;

	/** Skip the tutorial and jump straight to Level_1. */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SkipTutorial();

	/** Called when the player reaches the tutorial exit. Goes to Level_1. */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void OnTutorialCompleted();

	/** Show a tutorial message on the HUD. */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void ShowTutorialText(const FText& Message);

	/** Hide the current tutorial message. */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void HideTutorialText();

	/** Get the tutorial HUD widget. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	UUmbraTutorialHUDWidget* GetTutorialHUD() const { return TutorialHUDWidget; }

protected:
	virtual void BeginPlay() override;

private:
	/** Widget Blueprint class for the tutorial HUD. Set in the BP child. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUmbraTutorialHUDWidget> TutorialHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUmbraTutorialHUDWidget> TutorialHUDWidget;

	/** Widget Blueprint class for the tutorial complete screen. Set in the BP child. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUmbraTutorialCompleteWidget> TutorialCompleteWidgetClass;

	UPROPERTY()
	TObjectPtr<UUmbraTutorialCompleteWidget> TutorialCompleteWidget;

	/** Ordered list of tutorial steps based on Y position. Configure in BP child. */
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial")
	TArray<FTutorialStep> TutorialSteps;

	/** Index of the currently active step (-1 = none). */
	int32 ActiveStepIndex = -1;
};
