// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UmbraPawnController.generated.h"

class UInputMappingContext;
class UInputAction;
class AUmbraPawn;
class IUmbraInteractable;

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
};
