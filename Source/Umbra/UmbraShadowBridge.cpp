// Umbra - Light & Shadow Puzzle Game

#include "UmbraShadowBridge.h"
#include "UmbraLightSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "DrawDebugHelpers.h"

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
}

void AUmbraShadowBridge::BeginPlay()
{
	Super::BeginPlay();

	if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
	{
		Sub->RegisterBridge(this);
	}
}

void AUmbraShadowBridge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
	{
		Sub->UnregisterBridge(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AUmbraShadowBridge::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if ENABLE_DRAW_DEBUG
	const FColor OutlineColor = bBridgeEnabled ? FColor::Green : FColor::Red;
	DrawDebugBox(GetWorld(), BridgeBox->GetComponentLocation(),
		BridgeBox->GetScaledBoxExtent(), BridgeBox->GetComponentQuat(),
		OutlineColor, false, -1.f, 0, 3.f);

	// Sample a grid across the bridge surface and show shadow coverage
	UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>();
	if (Sub)
	{
		const TArray<TWeakObjectPtr<ULightComponent>>& Lights = Sub->GetLights();
		const FVector ScaledExtent = BridgeBox->GetScaledBoxExtent();
		const FVector BoxCenter = BridgeBox->GetComponentLocation();
		const FQuat BoxRot = BridgeBox->GetComponentQuat();

		constexpr float SampleSpacing = 30.f;
		const int32 GridCountX = FMath::Max(1, FMath::RoundToInt32((ScaledExtent.X * 2.f) / SampleSpacing));
		const int32 GridCountY = FMath::Max(1, FMath::RoundToInt32((ScaledExtent.Y * 2.f) / SampleSpacing));
		const float StepX = (ScaledExtent.X * 2.f) / GridCountX;
		const float StepY = (ScaledExtent.Y * 2.f) / GridCountY;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		for (int32 Xi = 0; Xi <= GridCountX; ++Xi)
		{
			for (int32 Yi = 0; Yi <= GridCountY; ++Yi)
			{
				FVector LocalOffset(-ScaledExtent.X + Xi * StepX, -ScaledExtent.Y + Yi * StepY, ScaledExtent.Z + 1.f);
				FVector WorldPos = BoxCenter + BoxRot.RotateVector(LocalOffset);

				bool bPointInShadow = false;

				for (const TWeakObjectPtr<ULightComponent>& LightPtr : Lights)
				{
					ULightComponent* Light = LightPtr.Get();
					if (!Light)
					{
						continue;
					}

					const FVector LightLoc = Light->GetComponentLocation();

					// Get attenuation radius based on light type
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

					FCollisionQueryParams LightParams = QueryParams;
					LightParams.AddIgnoredActor(Light->GetOwner());

					FHitResult Hit;
					if (GetWorld()->LineTraceSingleByChannel(Hit, WorldPos, LightLoc, ECC_Visibility, LightParams))
					{
						bPointInShadow = true;
						break;
					}
				}

				if (bPointInShadow)
				{
					DrawDebugPoint(GetWorld(), WorldPos, 8.f, FColor::Purple, false, -1.f, 0);
				}
			}
		}
	}
#endif
}

void AUmbraShadowBridge::EnableBridge()
{
	bBridgeEnabled = true;
	BridgeBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AUmbraShadowBridge::DisableBridge()
{
	bBridgeEnabled = false;
	BridgeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
