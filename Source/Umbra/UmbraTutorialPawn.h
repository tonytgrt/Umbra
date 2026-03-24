#pragma once

#include "CoreMinimal.h"
#include "UmbraPawn.h"
#include "UmbraTutorialPawn.generated.h"

/**
 * Tutorial pawn with a horizontal side-scrolling camera.
 * Overrides the default isometric camera to follow the orb from the side.
 */
UCLASS()
class UMBRA_API AUmbraTutorialPawn : public AUmbraPawn
{
	GENERATED_BODY()

public:
	AUmbraTutorialPawn();
};
