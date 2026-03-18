// UmbraLevelExit.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraLevelExit.generated.h"

class UBoxComponent;

UCLASS()
class AUmbraLevelExit : public AActor
{
    GENERATED_BODY()

public:
    AUmbraLevelExit();

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> TriggerBox;

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