#include "HUD/GameOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UGameOverlay::SetHealthBarPercent(float percent)
{
	if (HealthProgressBar)
		HealthProgressBar->SetPercent(percent);
}


void UGameOverlay::SetStaminaBarPercent(float percent)
{
	if (StaminaProgressBar)
		StaminaProgressBar->SetPercent(percent);
}


void UGameOverlay::SetGold(int32 gold)
{
	if (GoldText)
	{
		GoldText->SetText(
			FText::FromString(
				FString::Printf(TEXT("%d"), gold)
			)
		);
	}
}


void UGameOverlay::SetSouls(int32 souls)
{
	if (SoulsText)
	{
		SoulsText->SetText(
			FText::FromString(
				FString::Printf(TEXT("%d"), souls)
			)
		);
	}
}