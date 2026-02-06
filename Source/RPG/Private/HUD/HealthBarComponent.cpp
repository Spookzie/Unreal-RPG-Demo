#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"


void UHealthBarComponent::SetHealthPercent(float percent)
{
	if (!healthBarWidget)
		healthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());

	if (healthBarWidget && healthBarWidget->HealthBar)
	{
		healthBarWidget->HealthBar->SetPercent(percent);
	}
}