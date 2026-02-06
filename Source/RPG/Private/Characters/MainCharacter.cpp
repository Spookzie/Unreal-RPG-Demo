#include "Characters/MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "HUD/GameHUD.h"
#include "HUD/GameOverlay.h"
#include "Components/AttributeComponent.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"


AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Spring Arm
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->TargetArmLength = 250.f;
	springArm->bUsePawnControlRotation = true;
	springArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	springArm->SetupAttachment(GetRootComponent());

	// Camera
	viewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	viewCamera->SetupAttachment(springArm);

	// Hair & eyebrows
	hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	hair->SetupAttachment(GetMesh());
	hair->AttachmentName = FString("head");

	eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	eyebrows->SetupAttachment(GetMesh());
	eyebrows->AttachmentName = FString("head");
}

void AMainCharacter::Tick(float DeltaTime)
{
	if (attributes && gameOverlay)
	{
		attributes->RegenStamina(DeltaTime);
		gameOverlay->SetStaminaBarPercent(attributes->GetStaminaPercent());
	}
}


void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));

	/*	Movement	*/
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(mainCharacterContext, 0);
		}
	}

	/*	HUD	*/
	InitializeOverlay();
}


/*	Attack	*/
void AMainCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		actionState = EActionState::EAS_Attacking;
	}
}

bool AMainCharacter::CanAttack()
{
	// If player isn't occupied with any other task & also has weapon equipped
	return
		actionState == EActionState::EAS_Unoccupied &&
		characterState == ECharacterState::ECS_Equipped;
}

void AMainCharacter::AttackEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}


/*	Input	*/
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(movementAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &AMainCharacter::Jump);
		enhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AMainCharacter::EKeyPressed);
		enhancedInputComponent->BindAction(attackAction, ETriggerEvent::Triggered, this, &AMainCharacter::Attack);
		enhancedInputComponent->BindAction(dodgeAction, ETriggerEvent::Triggered, this, &AMainCharacter::Dodge);
	}
}


/*	Hit/Damage/Death	*/
void AMainCharacter::GetHit_Implementation(const FVector& impactPoint, AActor* hitter)
{
	Super::GetHit_Implementation(impactPoint, hitter);

	if (IsAlive())
		actionState = EActionState::EAS_HitReaction;
	
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	SetHUDHealth();

	return DamageAmount;
}

void AMainCharacter::Die()
{
	Super::Die();

	actionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void AMainCharacter::HitReactEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}


/*	Movement	*/
void AMainCharacter::Jump()
{
	if (IsUnoccupied())
		Super::Jump();
}

void AMainCharacter::Move(const FInputActionValue& value)
{
	// Can't move if the player is currently attacking
	if (!IsUnoccupied())
		return;

	const FVector2D movementVector = value.Get<FVector2D>();
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0.f, rotation.Yaw, 0.f);

	// Front-Back
	AddMovementInput(
		FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X),
		movementVector.Y
	);

	// Left-Right
	AddMovementInput(
		FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y),
		movementVector.X
	);
}

void AMainCharacter::Look(const FInputActionValue& value)
{
	if (actionState == EActionState::EAS_Dead)
		return;

	const FVector2D directionVector = value.Get<FVector2D>();

	AddControllerPitchInput(directionVector.Y);
	AddControllerYawInput(directionVector.X);
}

void AMainCharacter::Dodge()
{
	if (!IsUnoccupied() || !HasEnoughStamina())
		return;

	PlayDodgeMontage();
	actionState = EActionState::EAS_Dodge;

	if (attributes && gameOverlay)
	{
		attributes->UseStamina(attributes->GetDodgeCost());
		gameOverlay->SetStaminaBarPercent(attributes->GetStaminaPercent());
	}
}

bool AMainCharacter::HasEnoughStamina()
{
	return 
		attributes && 
		attributes->GetStamina() > attributes->GetDodgeCost();
}

bool AMainCharacter::IsUnoccupied()
{
	return actionState == EActionState::EAS_Unoccupied;
}

void AMainCharacter::DodgeEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}


/*	Pickup/Item	*/
void AMainCharacter::SetOverlappingItem(AItem* item)
{
	overlappingItem = item;
}

void AMainCharacter::AddSouls(ASoul* soul)
{
	if (attributes && gameOverlay)
	{
		attributes->AddSouls(soul->GetSouls());
		gameOverlay->SetSouls(attributes->GetSouls());
		attributes->Heal(7.f);
		gameOverlay->SetHealthBarPercent(attributes->GetHealthPercent());
	}
}

void AMainCharacter::AddGold(ATreasure* treasure)
{
	if (attributes && gameOverlay)
	{
		attributes->AddGold(treasure->GetGold());
		gameOverlay->SetGold(attributes->GetGold());
	}
}


/*	Weapon/Equip	*/
void AMainCharacter::EKeyPressed()
{
	if (AWeapon* overlappingWeapon = Cast<AWeapon>(overlappingItem))
	{
		EquipWeapon(overlappingWeapon);
		
		if (equipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				equipSound,
				GetActorLocation()
			);
		}
	}
	else
	{
		if (CanArm())
			Arm();
		else if(CanDisarm())
			Disarm();
	}
}

void AMainCharacter::AttachWeaponToHand()
{
	if (equippedWeapon)
	{
		equippedWeapon->Equip(
			GetMesh(),
			FName("RightHandSocket"),
			this, this
		);
	}
}

void AMainCharacter::AttachWeaponToBack()
{
	if (equippedWeapon)
	{
		equippedWeapon->Equip(
			GetMesh(),
			FName("SpineSocket"),
			this, this
		);
	}
}

void AMainCharacter::EquipWeapon(AWeapon* weapon)
{
	weapon->Equip(
		GetMesh(),
		FName("RightHandSocket"),
		this, this
	);

	characterState = ECharacterState::ECS_Equipped;
	overlappingItem = nullptr;
	equippedWeapon = weapon;
}

bool AMainCharacter::CanArm()
{
	return actionState == EActionState::EAS_Unoccupied
		&& characterState == ECharacterState::ECS_Unequipped
		&& equippedWeapon;
}

bool AMainCharacter::CanDisarm()
{
	return actionState == EActionState::EAS_Unoccupied
		&& characterState == ECharacterState::ECS_Equipped;
}

void AMainCharacter::Arm()
{
	PlayEquipMontage("Equip");
	characterState = ECharacterState::ECS_Equipped;
	actionState = EActionState::EAS_EquippingWeapon;
}

void AMainCharacter::Disarm()
{
	PlayEquipMontage("Unequip");
	characterState = ECharacterState::ECS_Unequipped;
	actionState = EActionState::EAS_EquippingWeapon;
}

void AMainCharacter::FinishEquipping()
{
	actionState = EActionState::EAS_Unoccupied;
}

/*	HUD	*/
void AMainCharacter::InitializeOverlay()
{
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (AGameHUD* gameHUD = Cast<AGameHUD>(playerController->GetHUD()))
		{
			gameOverlay = gameHUD->GetGameOverlay();
			if (gameOverlay && attributes)
			{
				gameOverlay->SetHealthBarPercent(attributes->GetHealthPercent());
				gameOverlay->SetStaminaBarPercent(1.f);
				gameOverlay->SetGold(0);
				gameOverlay->SetSouls(0);
			}
		}
	}
}

void AMainCharacter::SetHUDHealth()
{
	if (gameOverlay && attributes)
		gameOverlay->SetHealthBarPercent(attributes->GetHealthPercent());
}


/*	Montages	*/
void AMainCharacter::PlayEquipMontage(const FName& sectionName)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && equipMontage)
	{
		animInstance->Montage_Play(equipMontage);
		animInstance->Montage_JumpToSection(sectionName, equipMontage);
	}
}

void AMainCharacter::PlayDodgeMontage()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && dodgeMontage)
	{
		animInstance->Montage_Play(dodgeMontage);
		animInstance->Montage_JumpToSection(FName("Default"), dodgeMontage);
	}
}