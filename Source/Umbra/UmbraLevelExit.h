// UmbraLevelExit.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraLevelExit.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class USoundBase;

UCLASS()
class AUmbraLevelExit : public AActor
{
    GENERATED_BODY()

public:
    AUmbraLevelExit();

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> TriggerBox;

    /** Niagara particle effect for the exit portal visual. Assign the system asset in editor/BP. */
    UPROPERTY(VisibleAnywhere, Category = "Umbra|Exit")
    TObjectPtr<UNiagaraComponent> PortalEffect;

    /** Sound played when the orb reaches the exit. */
    UPROPERTY(EditAnywhere, Category = "Umbra|Audio")
    TObjectPtr<USoundBase> WinSound;

    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );
};