// Umbra - Light & Shadow Puzzle Game

#include "UmbraPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"

AUmbraPawn::AUmbraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision root
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(30.f);
	CollisionSphere->SetCollisionProfileName(TEXT("Pawn"));
	CollisionSphere->SetSimulatePhysics(true);
	CollisionSphere->SetLinearDamping(LinearDamping);
	CollisionSphere->SetAngularDamping(AngularDamping);
	CollisionSphere->BodyInstance.bLockZTranslation = true;
	SetRootComponent(CollisionSphere);

	// Visible mesh
	PawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	PawnMesh->SetupAttachment(CollisionSphere);
	PawnMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Top-down camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CollisionSphere);
	CameraBoom->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 1500.f;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Load default sphere mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		PawnMesh->SetStaticMesh(SphereMesh.Object);
		PawnMesh->SetRelativeScale3D(FVector(0.6f));
	}
}

void AUmbraPawn::BeginPlay()
{
	Super::BeginPlay();

	// Apply damping values (in case they were tuned in editor)
	CollisionSphere->SetLinearDamping(LinearDamping);
	CollisionSphere->SetAngularDamping(AngularDamping);
}

void AUmbraPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Apply movement force from cached input
	if (!CurrentMoveInput.IsNearlyZero())
	{
		FVector Force(CurrentMoveInput.X, CurrentMoveInput.Y, 0.f);
		CollisionSphere->AddForce(Force * MoveForce, NAME_None, true);
	}
}

void AUmbraPawn::SetMoveInput(FVector2D Input)
{
	CurrentMoveInput = Input;
}

void AUmbraPawn::PerformShadowCheck()
{
	// TODO: Phase 1 — raycast to each light, check if pawn is in shadow over void
}
