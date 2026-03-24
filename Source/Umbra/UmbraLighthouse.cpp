#include "UmbraLighthouse.h"
#include "UmbraLightSubsystem.h"
#include "Components/SpotLightComponent.h"
#include "Umbra.h"

AUmbraLighthouse::AUmbraLighthouse()
{
    PrimaryActorTick.bCanEverTick = true;

    LighthouseRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LighthouseRoot"));
    SetRootComponent(LighthouseRoot);

    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLight->SetupAttachment(LighthouseRoot);
    SpotLight->SetIntensity(0.f);           // off by default
    SpotLight->SetVisibility(false);
    SpotLight->SetInnerConeAngle(22.f);
    SpotLight->SetOuterConeAngle(30.f);
    SpotLight->SetAttenuationRadius(2000.f);
    SpotLight->SetCastShadows(true);        // critical for shadow puzzle mechanic
}

void AUmbraLighthouse::BeginPlay()
{
    Super::BeginPlay();

    CurrentYaw = StartYaw;
    SweepDirection = 1.f;

    // Apply designer settings to the spotlight component
    SpotLight->SetInnerConeAngle(InnerConeAngle);
    SpotLight->SetOuterConeAngle(OuterConeAngle);
    SpotLight->SetAttenuationRadius(AttenuationRadius);
    SpotLight->SetRelativeRotation(FRotator(SpotPitch, StartYaw, 0.f));

    // Do NOT register with subsystem yet ¡ª only on Activate()
}

void AUmbraLighthouse::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unregister if still active
    if (bIsActivated)
    {
        if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
        {
            Sub->UnregisterLight(SpotLight);
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

    // Sweep yaw back and forth between StartYaw and EndYaw
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

    SpotLight->SetRelativeRotation(FRotator(SpotPitch, CurrentYaw, 0.f));
}

void AUmbraLighthouse::Activate()
{
    if (bIsActivated)
    {
        return;
    }

    bIsActivated = true;
    SpotLight->SetIntensity(LightIntensity);
    SpotLight->SetVisibility(true);

    // Register with the light subsystem so shadow checks include this light
    if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
    {
        Sub->RegisterLight(SpotLight);
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
    SpotLight->SetIntensity(0.f);
    SpotLight->SetVisibility(false);

    if (UUmbraLightSubsystem* Sub = GetWorld()->GetSubsystem<UUmbraLightSubsystem>())
    {
        Sub->UnregisterLight(SpotLight);
    }

    UE_LOG(LogUmbra, Log, TEXT("Lighthouse '%s': Deactivated"), *GetName());
}