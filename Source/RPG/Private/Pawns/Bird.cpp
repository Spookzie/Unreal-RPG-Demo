#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"


ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	// Capsule component
	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	capsule->SetCapsuleHalfHeight(20.f);	
	capsule->SetCapsuleRadius(18.f);
	SetRootComponent(capsule);

	// Skeletal mesh
	birdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	birdMesh->SetupAttachment(GetRootComponent());

	// Spring Arm
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->TargetArmLength = 300.f;
	springArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	springArm->SetupAttachment(GetRootComponent());
	
	// Camera
	viewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	viewCamera->FieldOfView = 120.f;
	viewCamera->SetupAttachment(springArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void ABird::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(birdMappingContext, 0);
		}
	}
}


/*	Movement	*/
void ABird::Move(const FInputActionValue& value)
{
	const float directionalValue = value.Get<float>();
	
	if (Controller && directionalValue != 0.f)
		AddMovementInput(GetActorForwardVector(), directionalValue);
}

void ABird::Look(const FInputActionValue& value)
{
	const FVector2D lookAxisVector = value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(lookAxisVector.X);
		AddControllerPitchInput(lookAxisVector.Y);
	}
}


void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


/*	Movement	*/
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}