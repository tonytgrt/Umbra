// Umbra - Light & Shadow Puzzle Game

#include "UmbraShadowBridge.h"
#include "UmbraLightSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

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

	// Instanced mesh for shadow tiles
	ShadowTiles = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ShadowTiles"));
	ShadowTiles->SetupAttachment(BridgeBox);
	ShadowTiles->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShadowTiles->SetCastShadow(false);
	ShadowTiles->NumCustomDataFloats = 0;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneFinder.Succeeded())
	{
		ShadowTiles->SetStaticMesh(PlaneFinder.Object);
	}
}

void AUmbraShadowBridge::BeginPlay()
{
	Super::BeginPlay();

	if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
	{
		Sub->RegisterBridge(this);
	}

	// Create dynamic material
	UMaterialInterface* BaseMat = BridgeMaterial;
	if (!BaseMat)
	{
		BaseMat = ShadowTiles->GetMaterial(0);
	}
	if (BaseMat)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
	}
	else
	{
		DynMaterial = UMaterialInstanceDynamic::Create(
			UMaterial::GetDefaultMaterial(MD_Surface), this);
	}

	if (DynMaterial)
	{
		DynMaterial->SetScalarParameterValue(TEXT("Opacity"), TileOpacity);
		DynMaterial->SetVectorParameterValue(TEXT("BaseColor"), BridgeColor);
		ShadowTiles->SetMaterial(0, DynMaterial);
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

void AUmbraShadowBridge::RebuildTiles()
{
	ShadowTiles->ClearInstances();

	if (CachedShadowPositions.Num() == 0)
	{
		return;
	}

	// Each tile is a small plane scaled to cover one grid cell.
	// Engine Plane is 100x100 UU, so scale = SampleSpacing / 100.
	const float TileScale = SampleSpacing / 100.f;
	const float TileZ = BridgeBox->GetComponentLocation().Z + BridgeBox->GetScaledBoxExtent().Z + 0.5f;

	for (const FVector& Pos : CachedShadowPositions)
	{
		FTransform T;
		T.SetLocation(FVector(Pos.X, Pos.Y, TileZ));
		T.SetScale3D(FVector(TileScale, TileScale, 1.f));
		ShadowTiles->AddInstance(T, /*bWorldSpace=*/ true);
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
					bPointInShadow = true;
					break;
				}
			}

			if (bPointInShadow && bAnyLightInRange)
			{
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

	if (++FrameCounter >= SampleInterval)
	{
		FrameCounter = 0;
		SampleShadowPositions();
		RebuildTiles();
	}
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
	ShadowTiles->ClearInstances();
}
