#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UGameOverlay;


UCLASS()
class RPG_API AGameHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<UGameOverlay> gameOverlayClass;

	UPROPERTY()
	UGameOverlay* gameOverlay;

public:
	FORCEINLINE UGameOverlay* GetGameOverlay() const { return gameOverlay; }
};