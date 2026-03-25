#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UmbraIntroCam.generated.h"

/**
 *  Place one of these in a level to define the intro camera overview position.
 *  The pawn finds it at BeginPlay and uses its location/rotation as the
 *  starting camera position before transitioning to gameplay.
 */
UCLASS()
class AUmbraIntroCam : public AActor
{
	GENERATED_BODY()

public:
	AUmbraIntroCam();

	/** How long the overview holds before transitioning (seconds). */
	UPROPERTY(EditAnywhere, Category = "Umbra|Intro Camera")
	float HoldDuration = 1.0f;

	/** How long the camera transition takes (seconds). */
	UPROPERTY(EditAnywhere, Category = "Umbra|Intro Camera")
	float TransitionDuration = 1.5f;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UBillboardComponent> EditorSprite;
#endif
};
