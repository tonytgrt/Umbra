// UmbraLevelExit.cpp
#include "UmbraLevelExit.h"
#include "UmbraPawn.h"
#include "UmbraPuzzleGameMode.h"
#include "UmbraTutorialGameMode.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

AUmbraLevelExit::AUmbraLevelExit()
{
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->SetGenerateOverlapEvents(true);
    RootComponent = TriggerBox;

    PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
    PortalEffect->SetupAttachment(TriggerBox);
    PortalEffect->SetAutoActivate(true);

    // Load win sound
    static ConstructorHelpers::FObjectFinder<USoundBase> WinSoundAsset(
        TEXT("/Game/Audio/win.win"));
    if (WinSoundAsset.Succeeded())
    {
        WinSound = WinSoundAsset.Object;
    }
}

void AUmbraLevelExit::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (Cast<AUmbraPawn>(OtherActor))
    {
        if (WinSound)
        {
            UGameplayStatics::PlaySound2D(this, WinSound);
        }
        if (AUmbraPuzzleGameMode* GM = GetWorld()->GetAuthGameMode<AUmbraPuzzleGameMode>())
        {
            GM->OnLevelCleared();
        }
        else if (AUmbraTutorialGameMode* TutorialGM = GetWorld()->GetAuthGameMode<AUmbraTutorialGameMode>())
        {
            TutorialGM->OnTutorialCompleted();
        }
    }
}

void AUmbraLevelExit::BeginPlay()
{
    Super::BeginPlay();
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AUmbraLevelExit::OnOverlapBegin);
}