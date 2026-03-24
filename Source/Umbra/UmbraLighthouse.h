#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraLighthouse.generated.h"

class USpotLightComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLighthouseActivated, AUmbraLighthouse*, Lighthouse);

/**
 *  Spotlight-emitting actor with optional beam rotation and body translation.
 *  Starts deactivated unless bAutoActivate is set.
 *  Activated when a battery is delivered to its drop-off zone.
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

    /** Turn the lighthouse on: enable spotlight and start rotation/translation. */
    UFUNCTION(BlueprintCallable, Category = "Umbra|Lighthouse")
    void Activate();

    /** Turn the lighthouse off: disable spotlight and stop rotation/translation. */
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> LighthouseRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USpotLightComponent> LighthouseSpotLight;

private:
    // -- Activation -----------------------------------------------------------

    /** If true, the lighthouse activates automatically on BeginPlay (for debugging). */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse")
    bool bAutoActivate = false;

    // -- Rotation settings ----------------------------------------------------

    /** Enable spotlight beam rotation (ping-pong sweep between StartYaw and EndYaw). */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Rotation")
    bool bEnableRotation = true;

    /** Starting yaw angle (world-space degrees) of the beam sweep. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Rotation",
        meta = (ClampMin = "-360.0", ClampMax = "360.0", EditCondition = "bEnableRotation"))
    float StartYaw = -45.f;

    /** Ending yaw angle (world-space degrees) of the beam sweep. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Rotation",
        meta = (ClampMin = "-360.0", ClampMax = "360.0", EditCondition = "bEnableRotation"))
    float EndYaw = 45.f;

    /** Pitch angle of the spotlight (negative = angled downward). */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Rotation",
        meta = (ClampMin = "-90.0", ClampMax = "0.0", EditCondition = "bEnableRotation"))
    float SpotPitch = -30.f;

    /** Rotation speed in degrees per second. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Rotation",
        meta = (ClampMin = "1.0", ClampMax = "360.0", EditCondition = "bEnableRotation"))
    float RotationSpeed = 30.f;

    // -- Translation settings -------------------------------------------------

    /** Enable the lighthouse to physically move between its start position and an end offset. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Translation")
    bool bEnableTranslation = false;

    /** End position as an offset from the lighthouse's starting location. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Translation",
        meta = (EditCondition = "bEnableTranslation"))
    FVector TranslationOffset = FVector::ZeroVector;

    /** Movement speed in units per second. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Lighthouse|Translation",
        meta = (ClampMin = "1.0", EditCondition = "bEnableTranslation"))
    float TranslationSpeed = 200.f;

    // -- Spotlight settings ----------------------------------------------------

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

    /** World-space start position (captured in BeginPlay). */
    FVector TranslationStart;

    /** World-space end position (TranslationStart + TranslationOffset). */
    FVector TranslationEnd;

    /** +1 = moving toward end, -1 = moving toward start. */
    float TranslationDirection = 1.f;
};
