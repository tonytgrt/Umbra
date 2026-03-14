// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UmbraPawn.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class AUmbraPawn : public APawn
{
	GENERATED_BODY()

public:
	AUmbraPawn();

	virtual void Tick(float DeltaSeconds) override;

	/** Called by controller each tick with WASD input */
	void SetMoveInput(FVector2D Input);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PawnMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCamera;

	/** Force applied per unit of input */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveForce = 800.f;

	/** Physics damping — tune in editor */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float LinearDamping = 2.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AngularDamping = 1.f;

	/** Cached input from controller */
	FVector2D CurrentMoveInput;

	// --- Shadow check (Phase 1) ---
	void PerformShadowCheck();

	/** Is the pawn currently over a void volume? */
	bool bIsOverVoid = false;
};
