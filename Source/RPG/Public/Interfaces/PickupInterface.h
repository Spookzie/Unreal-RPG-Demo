#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"


UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};


class RPG_API IPickupInterface
{
	GENERATED_BODY()

public:
	virtual void SetOverlappingItem(class AItem* item);
	virtual void AddSouls(class ASoul* soul);
	virtual void AddGold(class ATreasure* treasure);
};
