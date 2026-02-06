#include "Enemies/Enemy.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Components/CapsuleComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	healthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	healthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	pawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sesning"));
	pawnSensing->SightRadius = 4000.f;
	pawnSensing->SetPeripheralVisionAngle(45.f);
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (pawnSensing)
		pawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
		return;


	if (/*enemyState > EEnemyState::EES_Patrolling*/IsInCombatState())
	{
		// Active combat
		CheckCombatTarget();
	}
	else
	{
		// Patrolling
		CheckPatrolTarget();
	}
}


/*	Hit/Damage/Death	*/
void AEnemy::GetHit_Implementation(const FVector& impactPoint, AActor* hitter)
{
	Super::GetHit_Implementation(impactPoint, hitter);

	if (IsAlive())
	{
		ShowHealthBar();
		enemyState = EEnemyState::EES_HitReaction;
	}
	
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();

	if (!IsOutsideAttackRadius())
	{
		if (IsAlive())
			StartAttackTimer();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	if(EventInstigator)
		combatTarget = EventInstigator->GetPawn();
	
	if (!IsOutsideAttackRadius())
		enemyState = EEnemyState::EES_Attacking;
	else if (IsOutsideAttackRadius())
		StartChasing();

	return DamageAmount;
}

void AEnemy::HandleDamage(float damageAmount)
{
	Super::HandleDamage(damageAmount);

	if (attributes && healthBarWidget)
		healthBarWidget->SetHealthPercent(attributes->GetHealthPercent());
}

void AEnemy::Destroyed()
{
	if (equippedWeapon)
	{
		equippedWeapon->Destroy();
		equippedWeapon = nullptr;
	}
}

void AEnemy::Die()
{
	Super::Die();
	
	enemyState = EEnemyState::EES_Dead;

	if (enemyController)
	{
		enemyController->StopMovement();
		enemyController->UnPossess();
	}

	ClearAttackTimer();
	HideHealthBar();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SpawnSoul();

	SetLifeSpan(4.f);
}

void AEnemy::HitReactEnd()
{
	enemyState = EEnemyState::EES_NoState;
}

bool AEnemy::IsDead()
{
	return enemyState == EEnemyState::EES_Dead;
}

void AEnemy::SpawnSoul()
{

	UWorld* world = GetWorld();
	if (world && soulClass && attributes)
	{
		ASoul* spawnedSoul = world->SpawnActorDeferred<ASoul>(
			soulClass,
			GetActorTransform()
		);

		spawnedSoul->SetSouls(attributes->GetSouls());
		spawnedSoul->FinishSpawning(GetActorTransform());
	}
}


/*	Navigation	*/
void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();

		if(!IsEngaged())
			StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();

		if(!IsEngaged())
			StartChasing();
	}
	else if (CanAttack())
		StartAttackTimer();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(patrolTarget, patrolRadius))
	{
		patrolTarget = ChoosePatrolTarget();
		StartPatrolTimer();
	}
}

bool AEnemy::InTargetRange(AActor* target, double radius)
{
	if (!target)
		return false;

	const double distanceToTarget = (
		target->GetActorLocation() - GetActorLocation()
	).Size();

	return distanceToTarget <= radius;
}

void AEnemy::MoveToTarget(AActor* target)
{
	if (!enemyController || !target)
		return;
	
	FAIMoveRequest moveRequest;
	moveRequest.SetGoalActor(target);
	moveRequest.SetAcceptanceRadius(acceptanceRadius);
	
	enemyController->MoveTo(moveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	// Get all vaid targets
	TArray<AActor*> validTargets;
	for (AActor* target : patrolTargets)
	{
		if (target != patrolTarget)
			validTargets.AddUnique(target);
	}

	// Choose a random one
	if (validTargets.Num() > 0)
	{
		const int32 idx = FMath::RandRange(0, validTargets.Num() - 1);
		return validTargets[idx];
	}

	return nullptr;
}

void AEnemy::InitializeEnemy()
{
	enemyController = Cast<AAIController>(GetController());

	HideHealthBar();
	if (attributes && healthBarWidget)
		healthBarWidget->SetHealthPercent(attributes->GetHealthPercent());

	MoveToTarget(patrolTarget);
	SpawnDefaultWeapon();
}

void AEnemy::PawnSeen(APawn* seenPawn)
{
	const bool bShouldChaseTarget =
		enemyState != EEnemyState::EES_Dead &&
		enemyState != EEnemyState::EES_Chasing &&
		enemyState < EEnemyState::EES_Attacking &&
		seenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		combatTarget = seenPawn;
		ClearPatrolTimer();
		StartChasing();
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(patrolTarget);
}

void AEnemy::LoseInterest()
{
	combatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	enemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = patrollingSpeed;
	MoveToTarget(patrolTarget);
}

void AEnemy::StartChasing()
{
	enemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = chasingSpeed;
	MoveToTarget(combatTarget);
}

void AEnemy::StartPatrolTimer()
{
	GetWorldTimerManager().SetTimer(
		patrolTimer,
		this,
		&AEnemy::PatrolTimerFinished,
		FMath::RandRange(patrolWaitMin, patrolWaitMax)
	);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(patrolTimer);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(combatTarget, combatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(combatTarget, attackRadius);
}

bool AEnemy::IsChasing()
{
	return enemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return enemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsInCombatState()
{
	return
		IsChasing() ||
		IsAttacking() ||
		IsEngaged() ||
		enemyState == EEnemyState::EES_NoState;
}


/*	Attack	*/
void AEnemy::Attack()
{
	Super::Attack();

	if (!combatTarget || IsDead())
		return;

	if (enemyController)
		enemyController->StopMovement();

	enemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

void AEnemy::StartAttackTimer()
{
	// attackTimer doesn't reset every frame
	if (GetWorldTimerManager().IsTimerActive(attackTimer))
		return;

	enemyState = EEnemyState::EES_Attacking;

	if (enemyController)
		enemyController->StopMovement();

	GetWorldTimerManager().SetTimer(
		attackTimer,
		this,
		&AEnemy::Attack,
		FMath::RandRange(attackMin, attackMax)
	);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(attackTimer);
}

bool AEnemy::IsEngaged()
{
	return enemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::CanAttack()
{
	return
		!IsOutsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
}

void AEnemy::AttackEnd()
{
	enemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}


/*	Weapon	*/
void AEnemy::SpawnDefaultWeapon()
{
	UWorld* world = GetWorld();
	if (world && weaponClass)
	{
		AWeapon* defaultWeapon = world->SpawnActor<AWeapon>(weaponClass);
		defaultWeapon->Equip(
			GetMesh(),
			FName("RightHandSocket"),
			this, this
		);

		equippedWeapon = defaultWeapon;
	}
}


/*	Attributes/Components/UI	*/
void AEnemy::HideHealthBar()
{
	if (healthBarWidget)
		healthBarWidget->SetVisibility(false);
	
}

void AEnemy::ShowHealthBar()
{
	if (healthBarWidget)
		healthBarWidget->SetVisibility(true);
}