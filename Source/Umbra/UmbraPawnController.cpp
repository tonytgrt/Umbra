// Umbra - Light & Shadow Puzzle Game

#include "UmbraPawnController.h"
#include "UmbraPawn.h"
#include "UmbraInteractable.h"
#include "UmbraPuzzleGameMode.h"
#include "UmbraTutorialGameMode.h"
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

		if (PauseAction)
		{
			EIC->BindAction(PauseAction, ETriggerEvent::Started, this, &AUmbraPawnController::OnPauseTriggered);
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
			DragPlaneZ = Hit.GetActor()->GetActorLocation().Z;
			IUmbraInteractable::Execute_OnDragStart(Hit.GetActor(), Hit.Location);
		}
	}
}

void AUmbraPawnController::OnMouseClickTriggered()
{
	if (DragTarget)
	{
		// Project cursor onto the horizontal plane at the drag target's Z height
		// instead of tracing against world geometry, so fast mouse movement
		// can't cause the lantern to jump to a distant hit point.
		FVector WorldOrigin, WorldDirection;
		if (DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
		{
			// Ray-plane intersection: solve for t where (Origin + t*Dir).Z == DragPlaneZ
			if (FMath::Abs(WorldDirection.Z) > KINDA_SMALL_NUMBER)
			{
				const float T = (DragPlaneZ - WorldOrigin.Z) / WorldDirection.Z;
				if (T > 0.f)
				{
					const FVector PlaneHit = WorldOrigin + WorldDirection * T;
					IUmbraInteractable::Execute_OnDragUpdate(DragTarget.GetObject(), PlaneHit);
				}
			}
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

void AUmbraPawnController::OnPauseTriggered(const FInputActionValue& Value)
{
	if (AUmbraPuzzleGameMode* GM = GetWorld()->GetAuthGameMode<AUmbraPuzzleGameMode>())
	{
		GM->TogglePause();
	}
	else if (AUmbraTutorialGameMode* TutGM = GetWorld()->GetAuthGameMode<AUmbraTutorialGameMode>())
	{
		TutGM->TogglePause();
	}
}
