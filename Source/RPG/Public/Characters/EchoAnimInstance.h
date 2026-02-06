#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "EchoAnimInstance.generated.h"


UCLASS()
class RPG_API UEchoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class AMainCharacter* mainCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	class UCharacterMovementComponent* movementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float groundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	ECharacterState characterState;

	UPROPERTY(BlueprintReadOnly, Category = "Action State")
	EActionState actionState;
};