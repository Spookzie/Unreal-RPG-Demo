#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"


UCLASS()
class RPG_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetHealthPercent(float percent);

private:
	UPROPERTY()
	class UHealthBar* healthBarWidget;
};
