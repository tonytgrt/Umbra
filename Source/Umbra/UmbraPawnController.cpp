// Umbra - Light & Shadow Puzzle Game

#include "UmbraPawnController.h"
#include "UmbraPawn.h"
#include "UmbraInteractable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Umbra.h"

AUmbraPawnController::AUmbraPawnController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	PrimaryActorTick.bCanEverTick = true;
}

void AUmbraPawnController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (UmbraMappingContext)
		{
			Subsystem->AddMappingContext(UmbraMappingContext, 0);
		}
	}
}

void AUmbraPawnController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUmbraPawnController::OnMoveTriggered);
			EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AUmbraPawnController::OnMoveCompleted);
		}

		if (MouseClickAction)
		{
			EIC->BindAction(MouseClickAction, ETriggerEvent::Started, this, &AUmbraPawnController::OnMouseClickStarted);
			EIC->BindAction(MouseClickAction, ETriggerEvent::Triggered, this, &AUmbraPawnController::OnMouseClickTriggered);
			EIC->BindAction(MouseClickAction, ETriggerEvent::Completed, this, &AUmbraPawnController::OnMouseClickReleased);
		}

		if (ResetLevelAction)
		{
			EIC->BindAction(ResetLevelAction, ETriggerEvent::Started, this, &AUmbraPawnController::OnResetLevel);
		}
	}
	else
	{
		UE_LOG(LogUmbra, Error, TEXT("UmbraPawnController: No Enhanced Input Component found."));
	}
}

void AUmbraPawnController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Forward cached move input to the pawn each tick
	if (AUmbraPawn* UmbraPawn = Cast<AUmbraPawn>(GetPawn()))
	{
		UmbraPawn->SetMoveInput(CachedMoveInput);
	}
}

// --- Input handlers ---

void AUmbraPawnController::OnMoveTriggered(const FInputActionValue& Value)
{
	CachedMoveInput = Value.Get<FVector2D>();
}

void AUmbraPawnController::OnMoveCompleted(const FInputActionValue& Value)
{
	CachedMoveInput = FVector2D::ZeroVector;
}

void AUmbraPawnController::OnMouseClickStarted()
{
	FHitResult Hit;
	if (GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		if (Hit.GetActor() && Hit.GetActor()->Implements<UUmbraInteractable>())
		{
			DragTarget.SetInterface(Cast<IUmbraInteractable>(Hit.GetActor()));
			DragTarget.SetObject(Hit.GetActor());
			IUmbraInteractable::Execute_OnDragStart(Hit.GetActor(), Hit.Location);
		}
	}
}

void AUmbraPawnController::OnMouseClickTriggered()
{
	if (DragTarget)
	{
		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		{
			IUmbraInteractable::Execute_OnDragUpdate(DragTarget.GetObject(), Hit.Location);
		}
	}
}

void AUmbraPawnController::OnMouseClickReleased()
{
	if (DragTarget)
	{
		IUmbraInteractable::Execute_OnDragEnd(DragTarget.GetObject());
		DragTarget = nullptr;
	}
}

void AUmbraPawnController::OnResetLevel()
{
	// TODO: tell GameMode to reset level
	UE_LOG(LogUmbra, Log, TEXT("Reset level requested"));
}
