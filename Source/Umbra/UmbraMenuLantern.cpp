#include "UmbraMenuLantern.h"
#include "Components/PointLightComponent.h"
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

    LanternLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LanternLight"));
    LanternLight->SetupAttachment(LanternMesh);
    LanternLight->SetIntensity(5000.f);
    LanternLight->SetAttenuationRadius(800.f);
    LanternLight->CastShadows = true;
    LanternLight->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
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
                FVector Target = WorldPos + WorldDir * T;
                Target.Y = FixedY;

                // Smooth follow
                FVector Current = GetActorLocation();
                FVector NewLoc = FMath::VInterpTo(Current, Target, DeltaSeconds, FollowSpeed);
                SetActorLocation(NewLoc);
            }
        }
    }
}