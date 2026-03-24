#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraBatteryDropOff.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AUmbraLighthouse;

/**
 *  Designated zone where the player drops off a battery to activate a lighthouse.
 *  Place near the lighthouse and assign the LinkedLighthouse in the editor.
 */
UCLASS()
class AUmbraBatteryDropOff : public AActor
{
    GENERATED_BODY()

public:
    AUmbraBatteryDropOff();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32                OtherBodyIndex,
        bool                 bFromSweep,
        const FHitResult& SweepResult);

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> DropOffZone;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> DropOffMesh;

    /** The lighthouse this drop-off zone activates. Assign in editor. */
    UPROPERTY(EditInstanceOnly, Category = "Umbra|Battery")
    TObjectPtr<AUmbraLighthouse> LinkedLighthouse;

    /** Has a battery already been delivered here? */
    bool bHasReceived = false;
};