#include "UmbraMenuLantern.h"
#include "UmbraLightSubsystem.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AUmbraMenuLantern::AUmbraMenuLantern()
{
    PrimaryActorTick.bCanEverTick = true;

    LanternMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LanternMesh"));
    LanternMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LanternMesh->SetCastShadow(false);
    SetRootComponent(LanternMesh);

    LanternLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LanternLight"));
    LanternLight->SetupAttachment(LanternMesh);
    LanternLight->SetIntensity(100000.f);
    LanternLight->SetAttenuationRadius(2000.f);
    LanternLight->SetInnerConeAngle(22.f);
    LanternLight->SetOuterConeAngle(30.f);
    LanternLight->CastShadows = true;
    LanternLight->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
    // Point the spotlight forward (toward the letters, along -Y)
    LanternLight->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}

void AUmbraMenuLantern::BeginPlay()
{
    Super::BeginPlay();

    if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
    {
        Sub->RegisterLight(LanternLight);
    }
}

void AUmbraMenuLantern::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
    {
        Sub->UnregisterLight(LanternLight);
    }

    Super::EndPlay(EndPlayReason);
}

void AUmbraMenuLantern::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    // Tint light yellow while mouse is pressed
    const bool bPressed = PC->IsInputKeyDown(EKeys::LeftMouseButton);
    LanternLight->SetLightColor(bPressed ? FLinearColor(1.f, 0.85f, 0.3f) : FLinearColor::White);

    // Project mouse screen position onto a vertical plane (Y = FixedY)
    // so that mouse up/down maps to Z axis movement
    FVector WorldPos, WorldDir;
    if (PC->DeprojectMousePositionToWorld(WorldPos, WorldDir))
    {
        if (FMath::Abs(WorldDir.Y) > KINDA_SMALL_NUMBER)
        {
            const float T = (FixedY - WorldPos.Y) / WorldDir.Y;
            if (T > 0.f)
            {
                FVector Target = WorldPos + WorldDir * T + CursorOffset;
                Target.Y = FixedY;

                // Smooth follow
                FVector Current = GetActorLocation();
                FVector NewLoc = FMath::VInterpTo(Current, Target, DeltaSeconds, FollowSpeed);
                SetActorLocation(NewLoc);
            }
        }
    }
}