#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;


UCLASS()
class RPG_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void Equip(USceneComponent* inParent, FName inSocketName, AActor* newOwner, APawn* newInstigator);
	void AttachMeshToSocket(USceneComponent* inParent, const FName& inSocketName);

	TArray<AActor*> ignoreActors;	// Tracking the actors that have been hit to ignore them (preventing double hit)

protected:
	virtual void BeginPlay() override;

	/*	Box Collision Callbacks	*/
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	/*	Breaking Objects	*/
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& fieldLocation);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* weaponBox;

	/*	Box Tracing	*/
	UPROPERTY(VisibleAnywhere)
	USceneComponent* boxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* boxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector boxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxTraceDebug = false;
	
	void BoxTrace(FHitResult& boxHit);
	bool ActorIsSameType(AActor* OtherActor);

	/*	Attributes	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Breakables")
	USoundBase* singlePotBreakSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Breakables")
	USoundBase* multiPotBreakSound;

	void PlayPotBreakSound(const FVector& impactPoint);

	/*	Attack/Hit/Damage	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float damage = 20.f;


/*	Setters & Getters	*/
public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return weaponBox; }
};