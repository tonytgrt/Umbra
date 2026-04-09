// Umbra - Light & Shadow Puzzle Game

#include "UmbraShadowBridge.h"
#include "UmbraLightSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "EngineUtils.h"
#include "Engine/World.h"

AUmbraShadowBridge::AUmbraShadowBridge()
{
	PrimaryActorTick.bCanEverTick = true;

	BridgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeBox"));
	RootComponent = BridgeBox;

	BridgeBox->SetBoxExtent(FVector(200.f, 200.f, 10.f));
	BridgeBox->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BridgeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BridgeBox->SetHiddenInGame(true);

	// Make the box walkable
	BridgeBox->CanCharacterStepUpOn = ECB_Yes;

	// Niagara effect for bridge visualization
	BridgeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BridgeEffect"));
	BridgeEffect->SetupAttachment(BridgeBox);
	BridgeEffect->SetAutoActivate(false);
}

void AUmbraShadowBridge::BeginPlay()
{
	Super::BeginPlay();

	if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
	{
		Sub->RegisterBridge(this);
	}

	// Pass bridge dimensions to the Niagara system
	const FVector Extent = BridgeBox->GetScaledBoxExtent();
	BridgeEffect->SetVectorParameter(TEXT("BridgeExtent"), Extent);
	BridgeEffect->SetFloatParameter(TEXT("Activation"), 0.f);
}

void AUmbraShadowBridge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
	{
		Sub->UnregisterBridge(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AUmbraShadowBridge::UpdateEffectParameters(float DeltaSeconds)
{
	if (!FMath::IsNearlyEqual(ActivationAlpha, TargetActivationAlpha))
	{
		ActivationAlpha = FMath::FInterpConstantTo(
			ActivationAlpha, TargetActivationAlpha, DeltaSeconds, ActivationSpeed);
	}

	BridgeEffect->SetFloatParameter(TEXT("Activation"), ActivationAlpha);

	// Activate/deactivate based on whether there are shadow positions to render
	const bool bHasPositions = CachedShadowPositions.Num() > 0;
	if (bHasPositions && !BridgeEffect->IsActive())
	{
		BridgeEffect->Activate();
	}
	else if (!bHasPositions && BridgeEffect->IsActive())
	{
		BridgeEffect->Deactivate();
	}
}

void AUmbraShadowBridge::SampleShadowPositions()
{
	CachedShadowPositions.Reset();

	UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>();
	if (!Sub)
	{
		return;
	}

	const TArray<TWeakObjectPtr<ULightComponent>>& Lights = Sub->GetLights();
	const FVector ScaledExtent = BridgeBox->GetScaledBoxExtent();
	const FVector BoxCenter = BridgeBox->GetComponentLocation();
	const FQuat BoxRot = BridgeBox->GetComponentQuat();

	const int32 GridCountX = FMath::Max(1, FMath::RoundToInt32((ScaledExtent.X * 2.f) / SampleSpacing));
	const int32 GridCountY = FMath::Max(1, FMath::RoundToInt32((ScaledExtent.Y * 2.f) / SampleSpacing));
	const float StepX = (ScaledExtent.X * 2.f) / GridCountX;
	const float StepY = (ScaledExtent.Y * 2.f) / GridCountY;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Exclude actors tagged "NoShadow" from shadow traces
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(TEXT("NoShadow")))
		{
			QueryParams.AddIgnoredActor(*It);
		}
	}

	for (int32 Xi = 0; Xi <= GridCountX; ++Xi)
	{
		for (int32 Yi = 0; Yi <= GridCountY; ++Yi)
		{
			FVector LocalOffset(-ScaledExtent.X + Xi * StepX, -ScaledExtent.Y + Yi * StepY, ScaledExtent.Z + 1.f);
			FVector WorldPos = BoxCenter + BoxRot.RotateVector(LocalOffset);

			// Match the pawn's shadow logic: a point is in shadow if ANY
			// in-range light is blocked (same as UmbraPawn::PerformShadowCheck).
			bool bPointInShadow = false;
			bool bAnyLightInRange = false;

			for (const TWeakObjectPtr<ULightComponent>& LightPtr : Lights)
			{
				ULightComponent* Light = LightPtr.Get();
				if (!Light)
				{
					continue;
				}

				const FVector LightLoc = Light->GetComponentLocation();

				float Radius = 10000.f;
				if (const UPointLightComponent* PL = Cast<UPointLightComponent>(Light))
				{
					Radius = PL->AttenuationRadius;
				}
				else if (const USpotLightComponent* SL = Cast<USpotLightComponent>(Light))
				{
					Radius = SL->AttenuationRadius;
				}

				if (FVector::Dist(WorldPos, LightLoc) > Radius)
				{
					continue;
				}

				// For spotlights, skip if point is outside the cone
				if (const USpotLightComponent* Spot = Cast<USpotLightComponent>(Light))
				{
					const FVector LightForward = Spot->GetForwardVector();
					const FVector LightToPoint = (WorldPos - LightLoc).GetSafeNormal();
					const float ConeHalfAngleRad = FMath::DegreesToRadians(Spot->OuterConeAngle);
					if (FVector::DotProduct(LightForward, LightToPoint) < FMath::Cos(ConeHalfAngleRad))
					{
						continue;
					}
				}

				bAnyLightInRange = true;

				FCollisionQueryParams LightParams = QueryParams;
				LightParams.AddIgnoredActor(Light->GetOwner());

				FHitResult Hit;
				if (GetWorld()->LineTraceSingleByChannel(Hit, WorldPos, LightLoc, ECC_Visibility, LightParams))
				{
					// Blocked path to this light — point is in shadow
					bPointInShadow = true;
					break;
				}
			}

			if (bPointInShadow && bAnyLightInRange)
			{
				// Only keep points over void (no solid ground below)
				FHitResult GroundHit;
				const FVector TraceEnd = WorldPos - FVector(0.f, 0.f, GroundTraceDepth);
				if (!GetWorld()->LineTraceSingleByChannel(GroundHit, WorldPos, TraceEnd, ECC_Visibility, QueryParams))
				{
					CachedShadowPositions.Add(WorldPos);
				}
			}
		}
	}
}

void AUmbraShadowBridge::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateEffectParameters(DeltaSeconds);

	// Throttle shadow sampling to every SampleInterval frames
	if (++FrameCounter >= SampleInterval)
	{
		FrameCounter = 0;
		SampleShadowPositions();

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
			BridgeEffect, FName("ShadowPositions"), CachedShadowPositions);
	}
}

void AUmbraShadowBridge::EnableBridge()
{
	bBridgeEnabled = true;
	BridgeBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TargetActivationAlpha = 1.f;
}

void AUmbraShadowBridge::DisableBridge()
{
	bBridgeEnabled = false;
	BridgeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TargetActivationAlpha = 0.f;
}
