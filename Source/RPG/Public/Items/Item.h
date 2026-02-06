#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};


UCLASS()
class RPG_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	EItemState itemState = EItemState::EIS_Hovering;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* itemMesh;

	/*	Sphere Component	*/
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* pickupSphere;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*	Effects	*/
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* itemEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* pickupSound;

	void SpawnPickupSystem();
	void PlayPickupSound();

private:
	/*	Movement	*/
	float runningTime;
	UPROPERTY(EditAnywhere, Category = Movement)
	float amplitude = 0.25f;
	UPROPERTY(EditAnywhere, Category = Movement)
	float timeConstant = 5.f;

	void SinMovement(float DeltaTime);

	/*	Effects	*/
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* pickupEffect;
};