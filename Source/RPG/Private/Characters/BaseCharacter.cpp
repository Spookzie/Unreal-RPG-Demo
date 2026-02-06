#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


/*	Weapon/Collision	*/
void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type collisionEnabled)
{
	if (equippedWeapon && equippedWeapon->GetWeaponBox())
	{
		equippedWeapon->GetWeaponBox()->SetCollisionEnabled(collisionEnabled);

		/*
		* We clear the array every time the player attack starts/finishes,
		* This maintains the temporary state of ignoreActors, preventing only double hit
		**************************/
		equippedWeapon->ignoreActors.Empty();
	}
}


/*	Attack	*/
void ABaseCharacter::Attack()
{
	if (combatTarget && combatTarget->ActorHasTag(FName("Dead")))
		combatTarget = nullptr;
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


/*	Montages	*/
void ABaseCharacter::PlayAttackMontage()
{
	if (attackMontageSections.IsEmpty())
		return;

	const int32 maxSectionIdx = attackMontageSections.Num() - 1;
	const int32 selection = FMath::RandRange(0, maxSectionIdx);

	PlayMontageSection(attackMontage, attackMontageSections[selection], attackMontagePlayRate);
}

void ABaseCharacter::PlayHitReactMontage(const FName& sectionName)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && hitReactMontage)
	{
		animInstance->Montage_Play(hitReactMontage);
		animInstance->Montage_JumpToSection(sectionName, hitReactMontage);
	}
}

void ABaseCharacter::PlayDeathMontage()
{
	const int32 maxSectionIdx = deathMontageSections.Num() - 1;
	const int32 selection = FMath::RandRange(0, maxSectionIdx);

	PlayMontageSection(deathMontage, deathMontageSections[selection], deathMontagePlayRate);
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* montage, const FName& sectionName, float playRate)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && montage)
	{
		animInstance->Montage_Play(montage, playRate);
		animInstance->Montage_JumpToSection(sectionName, montage);
	}
}

void ABaseCharacter::StopAttackMontage()
{
	if (UAnimInstance* animInstance = GetMesh()->GetAnimInstance())
	{
		animInstance->Montage_Stop(.25f, attackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if(!combatTarget)
		return FVector();

	const FVector combatTargetLoc = combatTarget->GetActorLocation();	// Location of the other actor
	const FVector loc = GetActorLocation();	// Location of this actor

	FVector targetToMe = (loc - combatTargetLoc).GetSafeNormal();	// Normalized distance b/w the two
	targetToMe *= warpTargetDistance;	// Distance we want the two actors to maintain while warping to each other

	return combatTargetLoc + targetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (combatTarget)
		return combatTarget->GetActorLocation();

	return FVector();
}


/*	Hit/Damage/Death	*/
void ABaseCharacter::GetHit_Implementation(const FVector& impactPoint, AActor* hitter)
{
	if (IsAlive() && hitter)
		DirectionalHitReact(hitter->GetActorLocation());
	else
		Die();

	PlayHitSound(impactPoint);
	SpawnHitParticles(impactPoint);
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));

	PlayDeathMontage();
}

void ABaseCharacter::DirectionalHitReact(const FVector& impactPoint)
{
	// Getting the hit angle
	const FVector forward = GetActorForwardVector();
	const FVector impactParalleled(impactPoint.X, impactPoint.Y, GetActorLocation().Z);	// parallelizing the hit vector to ground (ease calculations)
	const FVector hitVector = (impactParalleled - GetActorLocation()).GetSafeNormal();

	double theta = FMath::RadiansToDegrees(
		FMath::Acos(
			FVector::DotProduct(forward, hitVector)
		)
	);	// magnitude only (0-180)

	// Getting the angle direction
	const FVector crossProduct = FVector::CrossProduct(forward, hitVector);
	if (crossProduct.Z < 0)	// hit is from left
		theta *= -1.f;

	// Switching animation to play
	FName section("FromBack");

	if (theta >= -45.f && theta < 45.f)
		section = FName("FromFront");
	else if (theta >= -135.f && theta < -45.f)
		section = FName("FromRight");
	else if (theta >= 45.f && theta < 135.f)
		section = FName("FromLeft");

	PlayHitReactMontage(section);
}


/*	Attributes/Components	*/
void ABaseCharacter::PlayHitSound(const FVector& impactPoint)
{
	if (hitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			hitSound,
			impactPoint
		);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& impactPoint)
{
	if (!GetWorld() || !hitParticles)
		return;

	if (impactPoint.ContainsNaN())
		return;

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		hitParticles,
		impactPoint
	);
}

void ABaseCharacter::HandleDamage(float damageAmount)
{
	if (attributes)
		attributes->RecieveDamage(damageAmount);
}

bool ABaseCharacter::IsAlive()
{
	return attributes && attributes->IsAlive();
}