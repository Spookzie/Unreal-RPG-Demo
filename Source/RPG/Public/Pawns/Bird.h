#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;


UCLASS()
class RPG_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	
	/*	Movement	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* birdMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* moveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* lookAction;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* capsule;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* birdMesh;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* springArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* viewCamera;
};