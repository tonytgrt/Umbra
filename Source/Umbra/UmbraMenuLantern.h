#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraMenuLantern.generated.h"

class USpotLightComponent;
class UStaticMeshComponent;

UCLASS()
class AUmbraMenuLantern : public AActor
{
    GENERATED_BODY()

public:
    AUmbraMenuLantern();
    virtual void Tick(float DeltaSeconds) override;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> LanternMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USpotLightComponent> LanternLight;

    /** Fixed Y depth the lantern sits at (the vertical plane it moves on). */
    UPROPERTY(EditAnywhere, Category = "Menu Lantern")
    float FixedY = 100.f;

    /** How quickly the lantern follows the cursor (higher = snappier). */
    UPROPERTY(EditAnywhere, Category = "Menu Lantern")
    float FollowSpeed = 12.f;
};