#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraBattery.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 *  Pickup item. Player walks over it to pick it up.
 *  Once picked up, the actor is destroyed and the pawn's color changes.
 */
UCLASS()
class AUmbraBattery : public AActor
{
    GENERATED_BODY()

public:
    AUmbraBattery();

    virtual void Tick(float DeltaSeconds) override;

    /** Stop the idle spin (called after drop-off). */
    void StopSpinning() { bSpinning = false; }

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

public:
    /** Returns the visual mesh component (used by pawn to attach above head). */
    UStaticMeshComponent* GetBatteryMesh() const { return BatteryMesh; }

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> PickupSphere;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> BatteryMesh;

    bool bSpinning = false;
    float SpinSpeed = 90.f;
};