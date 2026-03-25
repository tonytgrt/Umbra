// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UmbraPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class AUmbraBattery;

UCLASS()
class AUmbraPawn : public ACharacter
{
	GENERATED_BODY()

public:
	AUmbraPawn();

	virtual void Tick(float DeltaSeconds) override;

	/** Called by controller each tick with WASD input */
	void SetMoveInput(FVector2D Input);

	// --- Battery carrying ---

	/** Called by AUmbraBattery when the player picks one up. */
	void PickUpBattery(AUmbraBattery* Battery);

	/** Called by AUmbraBatteryDropOff when the player delivers a battery.
	 *  Places the battery at DropLocation at its original size. */
	void DropBattery(const FVector& DropLocation);

	/** Is the pawn currently carrying a battery? */
	UFUNCTION(BlueprintPure, Category = "Umbra|Pawn")
	bool IsCarryingBattery() const { return bCarryingBattery; }

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PawnMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCamera;

private:

	virtual void BeginPlay() override;

	/** Cached input from controller */
	FVector2D CurrentMoveInput;

	// --- Level intro camera ---
	void TickIntroCam(float DeltaSeconds);

	bool bIntroCamActive = false;
	float IntroCamElapsed = 0.f;
	FVector IntroCamLocation;
	FRotator IntroCamRotation;
	float IntroCamHoldDuration = 3.0f;
	float IntroCamTransitionDuration = 2.0f;

	/** The camera's normal world transform (updated each frame during intro). */
	FVector GameplayCamLocation;
	FRotator GameplayCamRotation;

	// --- Shadow check (Phase 1) ---
	void PerformShadowCheck();

	/** True when every light source is blocked — pawn is fully in shadow */
	bool bIsInShadow = false;

	/** Is the pawn currently over a void volume? */
	bool bIsOverVoid = false;

	// --- Shadow bridge ---
	/** Last position on solid (non-bridge) ground — used for respawn */
	FVector LastSafeLocation;

	/** True when the pawn is standing on a shadow bridge */
	bool bIsOnBridge = false;

	/** Downward trace to check if the floor actor is a shadow bridge */
	bool IsStandingOnBridge() const;

	/** Enable or disable all registered shadow bridges */
	void SetAllBridgesEnabled(bool bEnabled);

	// --- Bridge respawn animation ---
	void StartBridgeRespawn();
	void TickBridgeRespawn(float DeltaSeconds);

	bool bIsRespawning = false;
	FVector RespawnStartLocation;
	FVector RespawnTargetLocation;
	float RespawnElapsed = 0.f;

	static constexpr float RespawnShrinkDuration = 0.2f;
	static constexpr float RespawnMoveDuration = 0.4f;
	static constexpr float RespawnGrowDuration = 0.2f;
	static constexpr float RespawnTotalDuration = RespawnShrinkDuration + RespawnMoveDuration + RespawnGrowDuration;

	/** Original mesh scale, captured at respawn start */
	FVector OriginalMeshScale;

	// --- Rolling effect ---
	/** Accumulated mesh rotation for rolling */
	FQuat RollingRotation = FQuat::Identity;

	/** Radius of the orb mesh (used to calculate roll speed) */
	UPROPERTY(EditAnywhere, Category = "Rolling")
	float OrbRadius = 30.f;

	// --- Battery state ---
	bool bCarryingBattery = false;

	/** The battery actor currently being carried (attached above pawn). */
	UPROPERTY()
	TObjectPtr<AUmbraBattery> CarriedBattery;

	/** Anchor point above the pawn where the battery floats. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BatteryAnchor;

	/** Rotation speed of the floating battery (degrees/sec). */
	float BatterySpinSpeed = 90.f;

	/** Original scale of the battery mesh before shrinking. */
	FVector OriginalBatteryScale;
};
