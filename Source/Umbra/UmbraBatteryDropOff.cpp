#include "UmbraBatteryDropOff.h"
#include "UmbraLighthouse.h"
#include "UmbraPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Umbra.h"

AUmbraBatteryDropOff::AUmbraBatteryDropOff()
{
    PrimaryActorTick.bCanEverTick = false;

    DropOffZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DropOffZone"));
    DropOffZone->SetBoxExtent(FVector(60.f, 60.f, 10.f));
    DropOffZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    DropOffZone->SetGenerateOverlapEvents(true);
    SetRootComponent(DropOffZone);

    DropOffMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DropOffMesh"));
    DropOffMesh->SetupAttachment(DropOffZone);
    DropOffMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DropOffMesh->SetCastShadow(false);

    // Placeholder flat cube visual (replace with actual mesh in editor/BP)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(
        TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshFinder.Succeeded())
    {
        DropOffMesh->SetStaticMesh(MeshFinder.Object);
        DropOffMesh->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.04f));
    }

    // Load drop sound
    static ConstructorHelpers::FObjectFinder<USoundBase> DropSoundAsset(
        TEXT("/Game/Audio/battery_drop.battery_drop"));
    if (DropSoundAsset.Succeeded())
    {
        DropSound = DropSoundAsset.Object;
    }
}

void AUmbraBatteryDropOff::BeginPlay()
{
    Super::BeginPlay();

    DropOffZone->OnComponentBeginOverlap.AddDynamic(this, &AUmbraBatteryDropOff::OnOverlapBegin);

    if (!LinkedLighthouse)
    {
        UE_LOG(LogUmbra, Warning,
            TEXT("BatteryDropOff '%s': No linked lighthouse assigned."), *GetName());
    }
}

void AUmbraBatteryDropOff::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32                OtherBodyIndex,
    bool                 bFromSweep,
    const FHitResult& SweepResult)
{
    if (bHasReceived)
    {
        return;
    }

    AUmbraPawn* Pawn = Cast<AUmbraPawn>(OtherActor);
    if (!Pawn || !Pawn->IsCarryingBattery())
    {
        return;
    }

    UE_LOG(LogUmbra, Log,
        TEXT("BatteryDropOff '%s': Battery delivered. Activating lighthouse."), *GetName());

    bHasReceived = true;

    if (DropSound)
    {
        UGameplayStatics::PlaySound2D(this, DropSound);
    }

    Pawn->DropBattery(GetActorLocation());

    if (LinkedLighthouse)
    {
        LinkedLighthouse->Activate();
    }
}