#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*	Health/Damage	*/
	void RecieveDamage(float damage);
	void Heal(float healAmount);

	/*	Stamina	*/
	void UseStamina(float staminaCost);
	void RegenStamina(float DeltaTime);

protected:
	virtual void BeginPlay() override;

private:
	/*	Health/Damage	*/
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Health")
	float health;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Health")
	float maxHealth = 100.f;
	
	/*	Stamina	*/
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Stamina")
	float stamina;										
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Stamina")
	float maxStamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Stamina")
	float dodgeCost = 15.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Stamina")
	float staminaRegenRate = 3.f;

	/*	Souls & Gold	*/
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Pickups")
	int32 gold;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Pickups")
	int32 souls;

/*	Getters & Setters	*/
public:
	float GetHealthPercent() { return health / maxHealth; }
	float GetStaminaPercent() { return stamina / maxStamina; }
	float GetStamina() { return stamina; }
	bool IsAlive() { return health > 0.f; }

	int32 GetGold() const { return gold; }
	int32 GetSouls() const { return souls; }
	float GetDodgeCost() const { return dodgeCost; }

	void AddGold(int32 goldPickedUp) { gold += goldPickedUp; }
	void AddSouls(int32 soulsPickedUp) { souls += soulsPickedUp; }
};