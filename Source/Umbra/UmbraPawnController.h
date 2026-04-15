// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPawnController.generated.h"

class UInputMappingContext;
class UInputAction;
class AUmbraPawn;
class IUmbraInteractable;
class UUmbraTouchThumbstick;

/**
 *  Controller for Umbra gameplay.
 *  WASD moves the pawn, mouse click drags/rotates light sources.
 */
UCLASS()
class AUmbraPawnController : public APlayerController
{
	GENERATED_BODY()

public:
	AUmbraPawnController();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// --- Input assets (set via Blueprint or editor) ---

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> UmbraMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MouseClickAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ResetLevelAction;

	/** Input action for pausing the game. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	// --- Input handlers ---
	void OnMoveTriggered(const struct FInputActionValue& Value);
	void OnMoveCompleted(const struct FInputActionValue& Value);
	void OnMouseClickStarted();
	void OnMouseClickTriggered();
	void OnMouseClickReleased();
	void OnResetLevel();
	void OnPauseTriggered(const FInputActionValue& Value);

	// --- State ---
	FVector2D CachedMoveInput;
	TScriptInterface<IUmbraInteractable> DragTarget;

	/** Z height of the drag plane, captured when the drag starts */
	float DragPlaneZ = 0.f;

	// --- Mobile touch support ---

	/** Thumbstick widget (created at runtime on mobile). */
	UPROPERTY()
	TObjectPtr<UUmbraTouchThumbstick> Thumbstick;

	/** Thumbstick widget class (set in Blueprint, or defaults to UUmbraTouchThumbstick). */
	UPROPERTY(EditDefaultsOnly, Category = "Input|Mobile")
	TSubclassOf<UUmbraTouchThumbstick> ThumbstickClass;

	/** Thumbstick size in viewport pixels. */
	UPROPERTY(EditDefaultsOnly, Category = "Input|Mobile")
	float ThumbstickSize = 400.f;

	/** Padding from the bottom-left corner in viewport pixels. */
	UPROPERTY(EditDefaultsOnly, Category = "Input|Mobile")
	float ThumbstickPadding = 160.f;

	/** Whether we're running on a touch device. */
	bool bIsMobile = false;

	/** True while a touch drag is active on the right side (lantern drag). */
	bool bTouchDragging = false;

	/** Finger index used for the drag (to distinguish from thumbstick finger). */
	int32 DragFingerIndex = -1;

	void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
	void OnTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
	void OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);

	void TouchDragStart(const FVector2D& ScreenPos);
	void TouchDragUpdate(const FVector2D& ScreenPos);
	void TouchDragEnd();
};
