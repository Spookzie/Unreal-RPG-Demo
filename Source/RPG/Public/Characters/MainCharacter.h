#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "MainCharacter.generated.h"

class UInputAction;
class UGroomComponent;
class AItem;
class UAnimMontage;


UCLASS()
class RPG_API AMainCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;

	/*	Movement/Input	*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	/*	Hit/Damage/Death	*/
	virtual void GetHit_Implementation(const FVector& impactPoint, AActor* hitter) override;	// IHitInterface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/*	Pickup/Item	*/
	virtual void SetOverlappingItem(AItem* item) override;
	virtual void AddSouls(class ASoul* soul) override;
	virtual void AddGold(class ATreasure* treasure) override;

protected:
	virtual void BeginPlay() override;

	/*	Attack	*/
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;

	/*	Hit/Damage/Death	*/
	virtual void Die() override;

private:
	/*	States	*/
	ECharacterState characterState = ECharacterState::ECS_Unequipped;
	EActionState actionState = EActionState::EAS_Unoccupied;

	/*	Hit/Damage/Death	*/
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	/*	Movement	*/
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* mainCharacterContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* movementAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* lookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* jumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* dodgeAction;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Dodge();
	bool HasEnoughStamina();
	bool IsUnoccupied();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	/*	Weapon/Equip	*/
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EKeyAction;

	UPROPERTY(VisibleInstanceOnly)
	AItem* overlappingItem;

	void EKeyPressed();
	bool CanArm();
	bool CanDisarm();
	void Arm();
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	void EquipWeapon(class AWeapon* weapon);

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	/*	Attack	*/
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* attackAction;

	/*	Attributes/Components/HUD	*/
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* springArm;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* viewCamera;

	UPROPERTY(VisibleAnywhere)
	UGroomComponent* hair;
	UPROPERTY(VisibleAnywhere)
	UGroomComponent* eyebrows;

	UPROPERTY(EditAnywhere, Category = "Attributes|Weapon")
	USoundBase* equipSound;

	UPROPERTY()
	class UGameOverlay* gameOverlay;

	void InitializeOverlay();
	void SetHUDHealth();

	/*	Montages	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* equipMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* dodgeMontage;

	void PlayEquipMontage(const FName& sectionName);
	void PlayDodgeMontage();

/*	Setters & Getters	*/
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return characterState; }
	FORCEINLINE EActionState GetActionState() const { return actionState; }
};