#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"


UCLASS()
class RPG_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	/*	Weapon/Item/Equip	*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class AWeapon* equippedWeapon;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type collisionEnabled);

	/*	AI/Navigation	*/
	UPROPERTY(BlueprintReadOnly, Category = "AI Navigation|Combat")
	AActor* combatTarget;

	/*	Attack	*/
	virtual void Attack();
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	void DisableMeshCollision();

	/*	Montages	*/
	UPROPERTY(EditAnywhere, Category = "Montages")
	double warpTargetDistance = 75.f;

	void PlayAttackMontage();
	void PlayHitReactMontage(const FName& sectionName);
	void PlayDeathMontage();
	void PlayMontageSection(UAnimMontage* montage, const FName& sectionName, float playRate);
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	/*	Hit/Damage/Death	*/
	virtual void GetHit_Implementation(const FVector& impactPoint, AActor* hitter) override;	// IHitInterface
	virtual void Die();
	void DirectionalHitReact(const FVector& impactPoint);

	/*	Attributes/Components	*/
	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* attributes;

	void PlayHitSound(const FVector& impactPoint);
	void SpawnHitParticles(const FVector& impactPoint);
	virtual void HandleDamage(float damageAmount);
	bool IsAlive();

private:
	/*	Montages	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* attackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* hitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* deathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> attackMontageSections;
	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> deathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Montages")
	float attackMontagePlayRate = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Montages")
	float deathMontagePlayRate = 1.f;

	/*	Attributes/Components	*/
	UPROPERTY(EditAnywhere, Category = "Attributes|Sounds")
	USoundBase* hitSound;
	
	UPROPERTY(EditAnywhere, Category = "Attributes|Particles")
	UParticleSystem* hitParticles;
};