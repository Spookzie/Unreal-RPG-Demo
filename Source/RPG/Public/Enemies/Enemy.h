#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;


UCLASS()
class RPG_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	/*	Hit/Damage/Death	*/
	virtual void GetHit_Implementation(const FVector& impactPoint, AActor* hitter) override;	// IHitInterface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState enemyState = EEnemyState::EES_Patrolling;

	/*	AI/Navigation/Patrolling	*/
	bool InTargetRange(AActor* target, double radius);
	void MoveToTarget(AActor* target);
	AActor* ChoosePatrolTarget();

	/*	Attack	*/
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float damageAmount) override;
	virtual void AttackEnd() override;

	/*	Hit/Damage/Death	*/
	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:
	/*	Hit/Damage/Death	*/
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> soulClass; 
	
	bool IsDead();
	void SpawnSoul();

	/*	AI/Navigation/Patrolling	*/
	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* pawnSensing;

	UPROPERTY()
	class AAIController* enemyController;

	FTimerHandle patrolTimer;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation|Patrolling")
	AActor* patrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation|Patrolling")
	TArray<AActor*> patrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation|Combat")
	double combatRadius = 1000.f;	// Chasing range
	UPROPERTY(EditAnywhere, Category = "AI Navigation|Combat")
	double attackRadius = 450.f;	// Attacking range
	UPROPERTY(EditAnywhere, Category = "AI Navigation|Patrolling")
	double patrolRadius = 200.f;	// Patrol target detection range
	UPROPERTY(EditAnywhere, Category = "AI Navigation|Patrolling")
	double acceptanceRadius = 150.f;	// Distance at which enemy will stop from us

	UPROPERTY(EditAnywhere, Category = "AI Navigation|Patrolling")
	float patrolWaitMin = 3.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation|Patrolling")
	float patrolWaitMax = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation|Patrolling")
	float patrollingSpeed = 150.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation|Combat")
	float chasingSpeed = 350.f;

	void InitializeEnemy();
	
	UFUNCTION()
	void PawnSeen(APawn* seenPawn);	// Callback for OnPawnSeen in PawnSensingComponent

	void CheckPatrolTarget();
	void CheckCombatTarget();

	void PatrolTimerFinished();
	void LoseInterest();
	void StartPatrolling();
	void StartChasing();
	void StartPatrolTimer();
	void ClearPatrolTimer();

	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsInCombatState();

	/*	Attack	*/
	FTimerHandle attackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float attackMin = .5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float attackMax = 1.f;

	void StartAttackTimer();
	void ClearAttackTimer();
	bool IsEngaged();

	/*	Weapon	*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> weaponClass;

	void SpawnDefaultWeapon();

	/*	Attributes/Components/UI	*/
	UPROPERTY(VisibleAnywhere)
	class UHealthBarComponent* healthBarWidget;

	void HideHealthBar();
	void ShowHealthBar();
};