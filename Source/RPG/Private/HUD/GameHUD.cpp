#include "HUD/GameHUD.h"
#include "HUD/GameOverlay.h"


void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* controller = world->GetFirstPlayerController();
		if (controller && gameOverlayClass)
		{
			gameOverlay = CreateWidget<UGameOverlay>(controller, gameOverlayClass);
			gameOverlay->AddToViewport();
		}
	}
}