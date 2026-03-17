// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UmbraPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class AUmbraPawn : public ACharacter
{
	GENERATED_BODY()

public:
	AUmbraPawn();

	virtual void Tick(float DeltaSeconds) override;

	/** Called by controller each tick with WASD input */
	void SetMoveInput(FVector2D Input);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PawnMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCamera;

	/** Cached input from controller */
	FVector2D CurrentMoveInput;

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
};
