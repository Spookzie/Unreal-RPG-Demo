#include "Characters/EchoAnimInstance.h"
#include "Characters/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UEchoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	if (mainCharacter)
		movementComponent = mainCharacter->GetCharacterMovement();
}


void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (movementComponent)
	{
		groundSpeed = UKismetMathLibrary::VSizeXY(movementComponent->Velocity);
		isFalling = movementComponent->IsFalling();
		characterState = mainCharacter->GetCharacterState();
		actionState = mainCharacter->GetActionState();
	}
}