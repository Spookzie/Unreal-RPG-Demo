#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlay.generated.h"


UCLASS()
class RPG_API UGameOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(float percent);
	void SetStaminaBarPercent(float percent);
	void SetGold(int32 gold);
	void SetSouls(int32 souls);

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsText;
};
