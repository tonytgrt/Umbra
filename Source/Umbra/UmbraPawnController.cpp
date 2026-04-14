// Umbra - Light & Shadow Puzzle Game

#include "UmbraPawnController.h"
#include "UmbraPawn.h"
#include "UmbraInteractable.h"
#include "UmbraPuzzleGameMode.h"
#include "UmbraTutorialGameMode.h"
#include "UmbraTouchThumbstick.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
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

	// Detect mobile platform
	// TODO: revert to platform check after debugging thumbstick
	// #if PLATFORM_IOS || PLATFORM_ANDROID
	bIsMobile = true;
	// #else
	// bIsMobile = false;
	// #endif

	if (bIsMobile)
	{
		// bShowMouseCursor = false;  // TODO: re-enable after debugging

		if (ThumbstickClass)
		{
			Thumbstick = CreateWidget<UUmbraTouchThumbstick>(this, ThumbstickClass);
		}
		else
		{
			Thumbstick = CreateWidget<UUmbraTouchThumbstick>(this, UUmbraTouchThumbstick::StaticClass());
		}

		if (Thumbstick)
		{
			Thumbstick->AddToViewport(100);

			// Position in the bottom-left corner.
			// Viewport coordinates are from top-left, so compute Y from viewport height.
			int32 ViewX, ViewY;
			GetViewportSize(ViewX, ViewY);
			const float ThumbSize = 200.f;
			const float Padding = 160.f;
			Thumbstick->SetPositionInViewport(
				FVector2D(Padding, ViewY - ThumbSize - Padding), false);
			Thumbstick->SetDesiredSizeInViewport(FVector2D(ThumbSize, ThumbSize));
		}

		bEnableTouchEvents = true;
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

	// Touch input for lantern dragging on mobile
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this,
		&AUmbraPawnController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this,
		&AUmbraPawnController::OnTouchMoved);
	InputComponent->BindTouch(EInputEvent::IE_Released, this,
		&AUmbraPawnController::OnTouchReleased);
}

void AUmbraPawnController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AUmbraPawn* UmbraPawn = Cast<AUmbraPawn>(GetPawn()))
	{
		if (bIsMobile && Thumbstick)
		{
			UmbraPawn->SetMoveInput(Thumbstick->GetStickInput());
		}
		else
		{
			UmbraPawn->SetMoveInput(CachedMoveInput);
		}
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

// --- Mobile touch handlers ---

void AUmbraPawnController::OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (!bIsMobile) return;

	const FVector2D ScreenPos(Location.X, Location.Y);
	int32 ViewX, ViewY;
	GetViewportSize(ViewX, ViewY);

	// Left 40% of screen is reserved for the thumbstick widget
	if (ScreenPos.X < ViewX * 0.4f) return;

	if (!bTouchDragging)
	{
		DragFingerIndex = static_cast<int32>(FingerIndex);
		TouchDragStart(ScreenPos);
	}
}

void AUmbraPawnController::OnTouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bTouchDragging && static_cast<int32>(FingerIndex) == DragFingerIndex)
	{
		TouchDragUpdate(FVector2D(Location.X, Location.Y));
	}
}

void AUmbraPawnController::OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bTouchDragging && static_cast<int32>(FingerIndex) == DragFingerIndex)
	{
		TouchDragEnd();
	}
}

void AUmbraPawnController::TouchDragStart(const FVector2D& ScreenPos)
{
	FVector WorldOrigin, WorldDirection;
	if (UGameplayStatics::DeprojectScreenToWorld(this, ScreenPos, WorldOrigin, WorldDirection))
	{
		FHitResult Hit;
		FVector TraceEnd = WorldOrigin + WorldDirection * 10000.f;
		if (GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, TraceEnd, ECC_Visibility))
		{
			if (Hit.GetActor() && Hit.GetActor()->Implements<UUmbraInteractable>())
			{
				DragTarget.SetInterface(Cast<IUmbraInteractable>(Hit.GetActor()));
				DragTarget.SetObject(Hit.GetActor());
				DragPlaneZ = Hit.GetActor()->GetActorLocation().Z;
				IUmbraInteractable::Execute_OnDragStart(Hit.GetActor(), Hit.Location);
				bTouchDragging = true;
				return;
			}
		}
	}
	bTouchDragging = false;
}

void AUmbraPawnController::TouchDragUpdate(const FVector2D& ScreenPos)
{
	if (!DragTarget) return;

	FVector WorldOrigin, WorldDirection;
	if (UGameplayStatics::DeprojectScreenToWorld(this, ScreenPos, WorldOrigin, WorldDirection))
	{
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

void AUmbraPawnController::TouchDragEnd()
{
	if (DragTarget)
	{
		IUmbraInteractable::Execute_OnDragEnd(DragTarget.GetObject());
		DragTarget = nullptr;
	}
	bTouchDragging = false;
	DragFingerIndex = -1;
}
