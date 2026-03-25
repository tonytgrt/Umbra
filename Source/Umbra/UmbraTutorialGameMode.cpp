// Umbra - Light & Shadow Puzzle Game

#include "UmbraTutorialGameMode.h"
#include "UmbraTutorialHUDWidget.h"
#include "UmbraTutorialCompleteWidget.h"
#include "UmbraTutorialPawn.h"
#include "UmbraPawnController.h"
#include "Umbra.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AUmbraTutorialGameMode::AUmbraTutorialGameMode()
{
	DefaultPawnClass = AUmbraTutorialPawn::StaticClass();
	PlayerControllerClass = AUmbraPawnController::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
}

void AUmbraTutorialGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	// Create the tutorial HUD
	if (TutorialHUDWidgetClass)
	{
		TutorialHUDWidget = CreateWidget<UUmbraTutorialHUDWidget>(PC, TutorialHUDWidgetClass);
		if (TutorialHUDWidget)
		{
			TutorialHUDWidget->AddToViewport(0);
		}
	}

	// Game + UI input so player can move and click the skip button
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	PC->bShowMouseCursor = true;
	PC->SetInputMode(InputMode);
}

void AUmbraTutorialGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Pawn) return;

	const float PawnY = Pawn->GetActorLocation().Y;

	// Find which step the orb is currently inside (if any)
	int32 NewStepIndex = -1;
	for (int32 i = 0; i < TutorialSteps.Num(); ++i)
	{
		const FTutorialStep& Step = TutorialSteps[i];
		if (PawnY >= Step.MinY && PawnY <= Step.MaxY)
		{
			NewStepIndex = i;
			break;
		}
	}

	// No change — do nothing
	if (NewStepIndex == ActiveStepIndex) return;

	// Left the previous zone
	if (ActiveStepIndex >= 0)
	{
		HideTutorialText();
	}

	ActiveStepIndex = NewStepIndex;

	// Entered a new zone
	if (ActiveStepIndex >= 0)
	{
		const FTutorialStep& Step = TutorialSteps[ActiveStepIndex];
		if (Step.bCompleteTutorial)
		{
			OnTutorialCompleted();
		}
		else
		{
			ShowTutorialText(Step.Message);
		}
	}
}

void AUmbraTutorialGameMode::SkipTutorial()
{
	UE_LOG(LogUmbra, Log, TEXT("Tutorial skipped"));
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
}

void AUmbraTutorialGameMode::OnTutorialCompleted()
{
	UE_LOG(LogUmbra, Log, TEXT("Tutorial completed"));

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	// Hide the tutorial HUD
	if (TutorialHUDWidget)
	{
		TutorialHUDWidget->RemoveFromParent();
	}

	// Show the tutorial complete screen
	if (TutorialCompleteWidgetClass)
	{
		TutorialCompleteWidget = CreateWidget<UUmbraTutorialCompleteWidget>(PC, TutorialCompleteWidgetClass);
		if (TutorialCompleteWidget)
		{
			TutorialCompleteWidget->AddToViewport(10);

			// Switch to UI-only input so the player can click Continue
			PC->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TutorialCompleteWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);
		}
	}
}

void AUmbraTutorialGameMode::ShowTutorialText(const FText& Message)
{
	if (TutorialHUDWidget)
	{
		TutorialHUDWidget->ShowTutorialText(Message);
	}
}

void AUmbraTutorialGameMode::HideTutorialText()
{
	if (TutorialHUDWidget)
	{
		TutorialHUDWidget->HideTutorialText();
	}
}
