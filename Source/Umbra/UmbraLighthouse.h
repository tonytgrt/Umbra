#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraLighthouse.generated.h"

class USpotLightComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLighthouseActivated, AUmbraLighthouse*, Lighthouse);

/**
 *  Spotlight-emitting actor that rotates its beam between two yaw angles.
 *  Starts deactivated. Activated when a battery is delivered to its drop-off zone.
 *  Registers its spotlight with UUmbraLightSubsystem for shadow detection.
 */
UCLASS()
class AUmbraLighthouse : public AActor
{
    GENERATED_BODY()

public:
    AUmbraLighthouse();

    virtual void Tick(float DeltaSeconds) override;

    // -- Public API (called by drop-off zone) ---------------------------------

    /** Turn the lighthouse on: enable spotlight and start rotation. */
    UFUNCTION(BlueprintCallable, Category = "Umbra|Lighthouse")
    void Activate();

    /** Turn the lighthouse off: disable spotlight and stop rotation. */
    UFUNCTION(BlueprintCallable, Category = "Umbra|Lighthouse")
    void Deactivate();

    UFUNCTION(BlueprintPure, Category = "Umbra|Lighthouse")
    bool IsActivated() const { return bIsActivated; }

    // -- Delegates ------------------------------------------------------------

    UPROPERTY(BlueprintAssignable, Category = "Umbra|Lighthouse")
    FOnLighthouseActivated OnActivated;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // -- Components -----------------------------------------------------------

    /** Root scene component (attach point ¡ª the Blueprint reparents here). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> LighthouseRoot;

    /**
     *  The spotlight. Attached to the Bulb_Top sub-component in Blueprint.
     *  Created in C++ so we can register it with the light subsystem.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USpotLightComponent> SpotLight;

private:
    // -- Designer-specified data ----------------------------------------------

    /** Starting yaw angle (world-space degrees) of the beam sweep. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse",
        meta = (ClampMin = "-360.0", ClampMax = "360.0"))
    float StartYaw = -45.f;

    /** Ending yaw angle (world-space degrees) of the beam sweep. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse",
        meta = (ClampMin = "-360.0", ClampMax = "360.0"))
    float EndYaw = 45.f;

    /** Pitch angle of the spotlight (negative = angled downward). */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse",
        meta = (ClampMin = "-90.0", ClampMax = "0.0"))
    float SpotPitch = -30.f;

    /** Rotation speed in degrees per second. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse",
        meta = (ClampMin = "1.0", ClampMax = "360.0"))
    float RotationSpeed = 30.f;

    /** Spotlight intensity when activated. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse")
    float LightIntensity = 8000.f;

    /** Spotlight cone inner angle in degrees. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse",
        meta = (ClampMin = "1.0", ClampMax = "80.0"))
    float InnerConeAngle = 22.f;

    /** Spotlight cone outer angle in degrees. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse",
        meta = (ClampMin = "1.0", ClampMax = "80.0"))
    float OuterConeAngle = 30.f;

    /** Spotlight attenuation radius (how far the beam reaches). */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse",
        meta = (ClampMin = "100.0"))
    float AttenuationRadius = 2000.f;

    // -- Runtime state --------------------------------------------------------

    bool bIsActivated = false;

    /** Current yaw the spotlight is pointing at. */
    float CurrentYaw = 0.f;

    /** +1 = sweeping toward EndYaw, -1 = sweeping toward StartYaw. */
    float SweepDirection = 1.f;
};