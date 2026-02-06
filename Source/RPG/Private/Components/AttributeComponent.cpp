#include "Components/AttributeComponent.h"


UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	health = maxHealth;
	stamina = maxStamina;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


/*	Health/Damage	*/
void UAttributeComponent::RecieveDamage(float damage)
{
	health = FMath::Clamp(
		health - damage,
		0.f,
		maxHealth
	);
}

void UAttributeComponent::Heal(float healAmount)
{
	health = FMath::Clamp(
		health + healAmount,
		0.f,
		maxHealth
	);
}


/*	Stamina	*/
void UAttributeComponent::UseStamina(float staminaCost)
{
	stamina = FMath::Clamp(
		stamina - staminaCost,
		0.f,
		maxStamina
	);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	stamina = FMath::Clamp(
		stamina + staminaRegenRate * DeltaTime,
		0.f,
		maxStamina
	);
}