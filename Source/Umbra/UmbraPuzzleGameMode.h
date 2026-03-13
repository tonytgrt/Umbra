// Umbra - Light & Shadow Puzzle Game

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UmbraPuzzleGameMode.generated.h"

/**
 *  Game mode for Umbra puzzle levels.
 *  Sets UmbraPawn as default pawn and UmbraPawnController as player controller.
 */
UCLASS()
class AUmbraPuzzleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUmbraPuzzleGameMode();

	void ResetLevel();
};
