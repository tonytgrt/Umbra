#include "UmbraLighthouse.h"
#include "UmbraLightSubsystem.h"
#include "Components/SpotLightComponent.h"
#include "Umbra.h"
#include "Engine/World.h"

AUmbraLighthouse::AUmbraLighthouse()
{
    PrimaryActorTick.bCanEverTick = true;

    LighthouseRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LighthouseRoot"));
    SetRootComponent(LighthouseRoot);

    LighthouseSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LighthouseSpotLight"));
    LighthouseSpotLight->SetupAttachment(LighthouseRoot);
    LighthouseSpotLight->SetIntensity(0.f);           // off by default
    LighthouseSpotLight->SetVisibility(false);
    LighthouseSpotLight->SetInnerConeAngle(22.f);
    LighthouseSpotLight->SetOuterConeAngle(30.f);
    LighthouseSpotLight->SetAttenuationRadius(2000.f);
    LighthouseSpotLight->SetCastShadows(true);        // critical for shadow puzzle mechanic
}

void AUmbraLighthouse::BeginPlay()
{
    Super::BeginPlay();

    // Attach spotlight to the Blueprint's Bulb_Top component (if it exists)
    if (USceneComponent* BulbTop = FindComponentByTag<USceneComponent>(TEXT("BulbTop")))
    {
        LighthouseSpotLight->AttachToComponent(BulbTop, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    }
    else
    {
        // Fallback: try finding by name
        TArray<UActorComponent*> Comps;
        GetComponents(Comps);
        for (UActorComponent* Comp : Comps)
        {
            if (USceneComponent* SC = Cast<USceneComponent>(Comp))
            {
                if (SC->GetName().Contains(TEXT("Bulb_Top")))
                {
                    LighthouseSpotLight->AttachToComponent(SC, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                    break;
                }
            }
        }
    }

    CurrentYaw = StartYaw;
    SweepDirection = 1.f;

    // Apply designer settings to the spotlight component
    LighthouseSpotLight->SetInnerConeAngle(InnerConeAngle);
    LighthouseSpotLight->SetOuterConeAngle(OuterConeAngle);
    LighthouseSpotLight->SetAttenuationRadius(AttenuationRadius);
    LighthouseSpotLight->SetRelativeRotation(FRotator(SpotPitch, StartYaw, 0.f));

    // Capture translation endpoints
    TranslationStart = GetActorLocation();
    TranslationEnd = TranslationStart + TranslationOffset;
    TranslationDirection = 1.f;

    if (bAutoActivate)
    {
        Activate();
    }
}

void AUmbraLighthouse::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bIsActivated)
    {
        if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
        {
            Sub->UnregisterLight(LighthouseSpotLight);
        }
    }

    Super::EndPlay(EndPlayReason);
}

void AUmbraLighthouse::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bIsActivated)
    {
        return;
    }

    // --- Rotation: sweep spotlight yaw back and forth ---
    if (bEnableRotation)
    {
        CurrentYaw += SweepDirection * RotationSpeed * DeltaSeconds;

        if (SweepDirection > 0.f && CurrentYaw >= EndYaw)
        {
            CurrentYaw = EndYaw;
            SweepDirection = -1.f;
        }
        else if (SweepDirection < 0.f && CurrentYaw <= StartYaw)
        {
            CurrentYaw = StartYaw;
            SweepDirection = 1.f;
        }

        LighthouseSpotLight->SetRelativeRotation(FRotator(SpotPitch, CurrentYaw, 0.f));
    }

    // --- Translation: move the whole actor back and forth ---
    if (bEnableTranslation)
    {
        const FVector Target = (TranslationDirection > 0.f) ? TranslationEnd : TranslationStart;
        const FVector NewLocation = FMath::VInterpConstantTo(GetActorLocation(), Target, DeltaSeconds, TranslationSpeed);
        SetActorLocation(NewLocation);

        if (FVector::DistSquared(NewLocation, Target) < 1.f)
        {
            SetActorLocation(Target);
            TranslationDirection *= -1.f;
        }
    }
}

void AUmbraLighthouse::Activate()
{
    if (bIsActivated)
    {
        return;
    }

    bIsActivated = true;
    LighthouseSpotLight->SetIntensity(LightIntensity);
    LighthouseSpotLight->SetVisibility(true);

    if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
    {
        Sub->RegisterLight(LighthouseSpotLight);
    }

    UE_LOG(LogUmbra, Log, TEXT("Lighthouse '%s': Activated"), *GetName());
    OnActivated.Broadcast(this);
}

void AUmbraLighthouse::Deactivate()
{
    if (!bIsActivated)
    {
        return;
    }

    bIsActivated = false;
    LighthouseSpotLight->SetIntensity(0.f);
    LighthouseSpotLight->SetVisibility(false);

    if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
    {
        Sub->UnregisterLight(LighthouseSpotLight);
    }

    UE_LOG(LogUmbra, Log, TEXT("Lighthouse '%s': Deactivated"), *GetName());
}
