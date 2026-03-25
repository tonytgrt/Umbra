// Umbra - Light & Shadow Puzzle Game

#include "UmbraPawn.h"
#include "UmbraBattery.h"
#include "UmbraIntroCam.h"
#include "UmbraLightSubsystem.h"
#include "EngineUtils.h"
#include "UmbraShadowBridge.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
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

	// Visible mesh  -  attach to root capsule
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
	CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
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

	// Anchor point for the floating battery indicator
	BatteryAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("BatteryAnchor"));
	BatteryAnchor->SetupAttachment(GetRootComponent());
	BatteryAnchor->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
}

void AUmbraPawn::BeginPlay()
{
	Super::BeginPlay();

	// Look for an intro camera anchor placed in this level
	AUmbraIntroCam* IntroCamActor = nullptr;
	for (TActorIterator<AUmbraIntroCam> It(GetWorld()); It; ++It)
	{
		IntroCamActor = *It;
		break;
	}

	if (IntroCamActor)
	{
		IntroCamLocation = IntroCamActor->GetActorLocation();
		IntroCamRotation = IntroCamActor->GetActorRotation();
		IntroCamHoldDuration = IntroCamActor->HoldDuration;
		IntroCamTransitionDuration = IntroCamActor->TransitionDuration;

		// Detach camera from spring arm so we can position it freely
		TopDownCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		// Capture where the camera would normally be
		GameplayCamLocation = CameraBoom->GetComponentLocation()
			+ CameraBoom->GetForwardVector() * CameraBoom->TargetArmLength;
		GameplayCamRotation = CameraBoom->GetComponentRotation();

		// Move camera to the overview position
		TopDownCamera->SetWorldLocation(IntroCamLocation);
		TopDownCamera->SetWorldRotation(IntroCamRotation);

		bIntroCamActive = true;
		IntroCamElapsed = 0.f;
	}
}

void AUmbraPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIntroCamActive)
	{
		TickIntroCam(DeltaSeconds);
	}

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

	// Rolling effect  -  rotate mesh based on velocity
	const FVector Velocity = GetVelocity();
	const float Speed = Velocity.Size();
	if (Speed > 1.f)
	{
		// Roll axis is perpendicular to velocity (cross with up)
		const FVector RollAxis = FVector::CrossProduct(FVector::UpVector, Velocity).GetSafeNormal();
		// Angular distance = linear distance / radius
		const float RollAngle = (Speed * DeltaSeconds) / OrbRadius;
		const FQuat DeltaRotation(RollAxis, RollAngle);
		RollingRotation = DeltaRotation * RollingRotation;
		RollingRotation.Normalize();
		PawnMesh->SetWorldRotation(RollingRotation);
	}

	// Spin the carried battery above the pawn
	if (bCarryingBattery && CarriedBattery)
	{
		BatteryAnchor->AddLocalRotation(FRotator(0.f, BatterySpinSpeed * DeltaSeconds, 0.f));
	}

	// Track safe ground before shadow check  -  only when on solid (non-bridge) ground
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

void AUmbraPawn::TickIntroCam(float DeltaSeconds)
{
	IntroCamElapsed += DeltaSeconds;

	// Get where the camera *should* be via the spring arm socket
	GameplayCamLocation = CameraBoom->GetSocketLocation(USpringArmComponent::SocketName);
	GameplayCamRotation = CameraBoom->GetSocketRotation(USpringArmComponent::SocketName);

	if (IntroCamElapsed <= IntroCamHoldDuration)
	{
		// Phase 1: hold at overview position
		TopDownCamera->SetWorldLocation(IntroCamLocation);
		TopDownCamera->SetWorldRotation(IntroCamRotation);
	}
	else if (IntroCamElapsed <= IntroCamHoldDuration + IntroCamTransitionDuration)
	{
		// Phase 2: smoothly interpolate to gameplay position
		const float Alpha = FMath::Clamp(
			(IntroCamElapsed - IntroCamHoldDuration) / IntroCamTransitionDuration, 0.f, 1.f);
		const float Smooth = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

		const FVector NewLoc = FMath::Lerp(IntroCamLocation, GameplayCamLocation, Smooth);
		const FQuat NewRot = FQuat::Slerp(IntroCamRotation.Quaternion(), GameplayCamRotation.Quaternion(), Smooth);

		TopDownCamera->SetWorldLocation(NewLoc);
		TopDownCamera->SetWorldRotation(NewRot.Rotator());
	}
	else
	{
		// Done — reattach camera to spring arm
		TopDownCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale, USpringArmComponent::SocketName);
		bIntroCamActive = false;
	}
}

void AUmbraPawn::PerformShadowCheck()
{
	UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>();
	if (!Sub)
	{
		bIsInShadow = false;
		return;
	}

	const TArray<TWeakObjectPtr<ULightComponent>>& Lights = Sub->GetLights();
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

	for (const TWeakObjectPtr<ULightComponent>& LightPtr : Lights)
	{
		ULightComponent* Light = LightPtr.Get();
		if (!Light)
		{
			continue;
		}

		const FVector LightLocation = Light->GetComponentLocation();

		// Get attenuation radius based on light type
		float AttenuationRadius = 10000.f;
		if (const UPointLightComponent* PL = Cast<UPointLightComponent>(Light))
		{
			AttenuationRadius = PL->AttenuationRadius;
		}
		else if (const USpotLightComponent* SL = Cast<USpotLightComponent>(Light))
		{
			AttenuationRadius = SL->AttenuationRadius;
		}

		// Skip lights whose attenuation doesn't reach the pawn
		const float Distance = FVector::Dist(PawnLocation, LightLocation);
		if (Distance > AttenuationRadius)
		{
#if ENABLE_DRAW_DEBUG
			DrawDebugLine(GetWorld(), PawnLocation, LightLocation,
				FColor::Orange, false, -1.f, 0, 1.f);
#endif
			continue;
		}

		// For spotlights, skip if pawn is outside the cone
		if (const USpotLightComponent* Spot = Cast<USpotLightComponent>(Light))
		{
			const FVector LightForward = Spot->GetForwardVector();
			const FVector LightToPawn = (PawnLocation - LightLocation).GetSafeNormal();
			const float ConeHalfAngleRad = FMath::DegreesToRadians(Spot->OuterConeAngle);
			if (FVector::DotProduct(LightForward, LightToPawn) < FMath::Cos(ConeHalfAngleRad))
			{
#if ENABLE_DRAW_DEBUG
				DrawDebugLine(GetWorld(), PawnLocation, LightLocation,
					FColor::Orange, false, -1.f, 0, 1.f);
#endif
				continue;
			}
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
			// Something blocks this light  -  pawn is in its shadow
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
	UE_LOG(LogUmbra, Log, TEXT("Lost shadow while on bridge  -  respawning to last safe location"));

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
		// Done  -  restore everything
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

void AUmbraPawn::PickUpBattery(AUmbraBattery* Battery)
{
	if (bCarryingBattery || !Battery)
	{
		return;
	}

	bCarryingBattery = true;
	CarriedBattery = Battery;

	// Disable the battery's collision so it stops triggering overlaps
	Battery->SetActorEnableCollision(false);

	// Attach the battery actor to our anchor point above the pawn
	Battery->AttachToComponent(BatteryAnchor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// Save original scale, then shrink for the floating indicator
	if (UStaticMeshComponent* BattMesh = Battery->GetBatteryMesh())
	{
		OriginalBatteryScale = BattMesh->GetRelativeScale3D();
		BattMesh->SetRelativeScale3D(FVector(0.2f));
	}

	// Reset anchor rotation so spinning starts clean
	BatteryAnchor->SetRelativeRotation(FRotator::ZeroRotator);

	UE_LOG(LogUmbra, Log, TEXT("Pawn: Now carrying a battery"));
}

void AUmbraPawn::DropBattery(const FVector& DropLocation)
{
	if (!bCarryingBattery)
	{
		return;
	}

	bCarryingBattery = false;

	if (CarriedBattery)
	{
		// Detach and place at the drop-off location
		CarriedBattery->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CarriedBattery->SetActorLocation(DropLocation);
		CarriedBattery->SetActorRotation(FRotator::ZeroRotator);

		// Restore original scale and offset upward so it sits on the ground
		if (UStaticMeshComponent* BattMesh = CarriedBattery->GetBatteryMesh())
		{
			BattMesh->SetRelativeScale3D(OriginalBatteryScale);

			// Get the mesh bounds to calculate how far up to shift
			const FVector MeshExtent = BattMesh->CalcLocalBounds().BoxExtent * OriginalBatteryScale;
			CarriedBattery->SetActorLocation(DropLocation + FVector(0.f, 0.f, MeshExtent.Z));
		}

		// Collision stays disabled so it can't be picked up again
		CarriedBattery->StopSpinning();
		CarriedBattery = nullptr;
	}

	UE_LOG(LogUmbra, Log, TEXT("Pawn: Dropped battery"));
}
