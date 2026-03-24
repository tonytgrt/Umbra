#include "UmbraTutorialPawn.h"
#include "GameFramework/SpringArmComponent.h"

AUmbraTutorialPawn::AUmbraTutorialPawn()
{
	// Override camera boom for horizontal side-scrolling view
	if (CameraBoom)
	{
		// Camera looks along Y axis with a slight downward angle
		CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
		CameraBoom->TargetArmLength = 800.f;
		CameraBoom->SocketOffset = FVector(0, 300.f, 80.f);
	}
}
