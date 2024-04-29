#include "CustomGameMode.h"
#include "CustomController.h"
#include "CustomPlayerState.h"
#include "../Character/PlayerCharacter.h"

ACustomGameMode::ACustomGameMode()
{
	DefaultPawnClass		= APlayerCharacter::StaticClass();
	PlayerControllerClass	= ACustomController::StaticClass();
	PlayerStateClass		= ACustomPlayerState::StaticClass();
}