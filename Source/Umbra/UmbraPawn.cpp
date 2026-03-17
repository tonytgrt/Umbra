// Umbra - Light & Shadow Puzzle Game

#include "UmbraPawn.h"
#include "UmbraLightSubsystem.h"
#include "UmbraShadowBridge.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "Umbra.h"

AUmbraPawn::AUmbraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Configure the capsule (inherited from ACharacter)
	GetCapsuleComponent()->InitCapsuleSize(30.f, 30.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Visible mesh — attach to root capsule
	PawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	PawnMesh->SetupAttachment(GetRootComponent());
	PawnMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PawnMesh->SetCastShadow(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		PawnMesh->SetStaticMesh(SphereMesh.Object);
		PawnMesh->SetRelativeScale3D(FVector(0.6f));
	}

	// Angled camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetRelativeRotation(FRotator(-55.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Configure CharacterMovement (inherited from ACharacter)
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->MaxWalkSpeed = 400.f;
	MoveComp->MaxAcceleration = 2000.f;
	MoveComp->BrakingDecelerationWalking = 4000.f;
	MoveComp->bCanWalkOffLedges = false;
	MoveComp->bCanWalkOffLedgesWhenCrouching = false;
	MoveComp->SetWalkableFloorAngle(50.f);
	MoveComp->GravityScale = 1.f;
	MoveComp->PerchRadiusThreshold = 30.f;
	MoveComp->PerchAdditionalHeight = 30.f;
	MoveComp->LedgeCheckThreshold = 10.f;

	// Don't rotate the pawn to face movement direction
	bUseControllerRotationYaw = false;
	MoveComp->bOrientRotationToMovement = false;

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}

void AUmbraPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsRespawning)
	{
		TickBridgeRespawn(DeltaSeconds);
		return;
	}

	if (!CurrentMoveInput.IsNearlyZero())
	{
		FVector Direction(CurrentMoveInput.X, CurrentMoveInput.Y, 0.f);
		Direction = Direction.GetClampedToMaxSize(1.f);
		AddMovementInput(Direction);
	}

	// Track safe ground before shadow check — only when on solid (non-bridge) ground
	bIsOnBridge = IsStandingOnBridge();
	if (!bIsOnBridge)
	{
		LastSafeLocation = GetActorLocation();
	}

	PerformShadowCheck();
}

void AUmbraPawn::SetMoveInput(FVector2D Input)
{
	CurrentMoveInput = Input;
}

void AUmbraPawn::PerformShadowCheck()
{
	UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>();
	if (!Sub)
	{
		bIsInShadow = false;
		return;
	}

	const TArray<TWeakObjectPtr<UPointLightComponent>>& Lights = Sub->GetLights();
	if (Lights.IsEmpty())
	{
		bIsInShadow = false;
		return;
	}

	// Trace from the pawn's feet, not the capsule center, so shadow detection
	// matches the visual shadows projected onto the ground plane.
	const FVector PawnLocation = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bAnyBlocked = false;

	for (const TWeakObjectPtr<UPointLightComponent>& LightPtr : Lights)
	{
		UPointLightComponent* Light = LightPtr.Get();
		if (!Light)
		{
			continue;
		}

		const FVector LightLocation = Light->GetComponentLocation();

		// Skip lights whose attenuation doesn't reach the pawn
		const float Distance = FVector::Dist(PawnLocation, LightLocation);
		if (Distance > Light->AttenuationRadius)
		{
#if ENABLE_DRAW_DEBUG
			DrawDebugLine(GetWorld(), PawnLocation, LightLocation,
				FColor::Orange, false, -1.f, 0, 1.f);
#endif
			continue;
		}

		// Trace from pawn towards the light
		FCollisionQueryParams LightQueryParams = QueryParams;
		LightQueryParams.AddIgnoredActor(Light->GetOwner());

		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit,
			PawnLocation,
			LightLocation,
			ECC_Visibility,
			LightQueryParams
		);

		if (!bHit)
		{
#if ENABLE_DRAW_DEBUG
			DrawDebugLine(GetWorld(), PawnLocation, LightLocation,
				FColor::Green, false, -1.f, 0, 2.f);
#endif
		}
		else
		{
			// Something blocks this light — pawn is in its shadow
			bAnyBlocked = true;
#if ENABLE_DRAW_DEBUG
			DrawDebugLine(GetWorld(), PawnLocation, Hit.ImpactPoint,
				FColor::Red, false, -1.f, 0, 2.f);
			DrawDebugLine(GetWorld(), Hit.ImpactPoint, LightLocation,
				FColor(255, 80, 80), false, -1.f, 0, 1.f);
			DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.f, FColor::Red, false, -1.f);
#endif
		}
	}

	const bool bWasInShadow = bIsInShadow;
	bIsInShadow = bAnyBlocked;

#if ENABLE_DRAW_DEBUG
	// Sphere around pawn: purple = in shadow, yellow = lit
	DrawDebugSphere(GetWorld(), PawnLocation, 50.f, 12,
		bIsInShadow ? FColor::Purple : FColor::Yellow,
		false, -1.f, 0, 2.f);
#endif

	if (bIsInShadow != bWasInShadow)
	{
		UE_LOG(LogUmbra, Log, TEXT("Pawn shadow state changed: %s"),
			bIsInShadow ? TEXT("IN SHADOW") : TEXT("LIT"));

		if (bIsInShadow)
		{
			SetAllBridgesEnabled(true);
		}
		else
		{
			SetAllBridgesEnabled(false);

			// If the player was on a bridge when shadow disappeared, animate back to safety
			if (bIsOnBridge)
			{
				StartBridgeRespawn();
			}
		}
	}
}

bool AUmbraPawn::IsStandingOnBridge() const
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 20.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		return Hit.GetActor() && Hit.GetActor()->IsA<AUmbraShadowBridge>();
	}

	return false;
}

void AUmbraPawn::StartBridgeRespawn()
{
	UE_LOG(LogUmbra, Log, TEXT("Lost shadow while on bridge — respawning to last safe location"));

	bIsRespawning = true;
	RespawnElapsed = 0.f;
	RespawnStartLocation = GetActorLocation();
	RespawnTargetLocation = LastSafeLocation;
	OriginalMeshScale = PawnMesh->GetRelativeScale3D();

	// Disable movement and collision so the pawn doesn't interact mid-animation
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AUmbraPawn::TickBridgeRespawn(float DeltaSeconds)
{
	RespawnElapsed += DeltaSeconds;

	if (RespawnElapsed <= RespawnShrinkDuration)
	{
		// Phase 1: shrink mesh to zero
		const float Alpha = RespawnElapsed / RespawnShrinkDuration;
		PawnMesh->SetRelativeScale3D(FMath::Lerp(OriginalMeshScale, FVector::ZeroVector, Alpha));
	}
	else if (RespawnElapsed <= RespawnShrinkDuration + RespawnMoveDuration)
	{
		// Phase 2: smoothly move to safe location (mesh stays at zero)
		PawnMesh->SetRelativeScale3D(FVector::ZeroVector);
		const float Alpha = (RespawnElapsed - RespawnShrinkDuration) / RespawnMoveDuration;
		const float Smooth = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);
		SetActorLocation(FMath::Lerp(RespawnStartLocation, RespawnTargetLocation, Smooth));
	}
	else if (RespawnElapsed <= RespawnTotalDuration)
	{
		// Phase 3: grow mesh back
		SetActorLocation(RespawnTargetLocation);
		const float Alpha = (RespawnElapsed - RespawnShrinkDuration - RespawnMoveDuration) / RespawnGrowDuration;
		PawnMesh->SetRelativeScale3D(FMath::Lerp(FVector::ZeroVector, OriginalMeshScale, Alpha));
	}
	else
	{
		// Done — restore everything
		PawnMesh->SetRelativeScale3D(OriginalMeshScale);
		SetActorLocation(RespawnTargetLocation);

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		bIsRespawning = false;
		bIsOnBridge = false;
	}
}

void AUmbraPawn::SetAllBridgesEnabled(bool bEnabled)
{
	UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>();
	if (!Sub)
	{
		return;
	}

	for (const TWeakObjectPtr<AUmbraShadowBridge>& BridgePtr : Sub->GetBridges())
	{
		if (AUmbraShadowBridge* Bridge = BridgePtr.Get())
		{
			if (bEnabled)
			{
				Bridge->EnableBridge();
			}
			else
			{
				Bridge->DisableBridge();
			}
		}
	}
}
