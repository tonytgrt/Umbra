#include "UmbraBattery.h"
#include "UmbraPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Umbra.h"

AUmbraBattery::AUmbraBattery()
{
    PrimaryActorTick.bCanEverTick = true;

    // Overlap trigger sphere
    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    PickupSphere->InitSphereRadius(50.f);
    PickupSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    PickupSphere->SetGenerateOverlapEvents(true);
    SetRootComponent(PickupSphere);

    // Visual mesh
    BatteryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BatteryMesh"));
    BatteryMesh->SetupAttachment(PickupSphere);
    BatteryMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BatteryMesh->SetCastShadow(false);

    // Placeholder cylinder visual (replace with actual mesh in editor/BP)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (MeshFinder.Succeeded())
    {
        BatteryMesh->SetStaticMesh(MeshFinder.Object);
        BatteryMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
    }

    // Load collect sound
    static ConstructorHelpers::FObjectFinder<USoundBase> CollectSoundAsset(
        TEXT("/Game/Audio/battery_collect.battery_collect"));
    if (CollectSoundAsset.Succeeded())
    {
        CollectSound = CollectSoundAsset.Object;
    }
}

void AUmbraBattery::BeginPlay()
{
    Super::BeginPlay();

    PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AUmbraBattery::OnOverlapBegin);
    bSpinning = true;
}

void AUmbraBattery::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bSpinning)
    {
        BatteryMesh->AddLocalRotation(FRotator(0.f, SpinSpeed * DeltaSeconds, 0.f));
    }
}

void AUmbraBattery::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32                OtherBodyIndex,
    bool                 bFromSweep,
    const FHitResult& SweepResult)
{
    AUmbraPawn* Pawn = Cast<AUmbraPawn>(OtherActor);
    if (!Pawn)
    {
        return;
    }

    // Don't pick up if already carrying
    if (Pawn->IsCarryingBattery())
    {
        return;
    }

    UE_LOG(LogUmbra, Log, TEXT("Battery '%s': Picked up by pawn"), *GetName());

    if (CollectSound)
    {
        UGameplayStatics::PlaySound2D(this, CollectSound);
    }

    Pawn->PickUpBattery(this);
}