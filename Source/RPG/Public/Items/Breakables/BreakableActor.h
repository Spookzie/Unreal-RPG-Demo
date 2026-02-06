#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"


UCLASS()
class RPG_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()

public:
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& impactPoint, AActor* hitter) override;

	/*	Breaking	*/
	UFUNCTION()
	void OnChaosBreak(const FChaosBreakEvent& breakEvent);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UGeometryCollectionComponent* geometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* capsule;

private:
	/*	Treasure	*/
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class ATreasure>> treasureClasses;

	bool bBroken = false;

	/*	Treasure	*/
	void SpawnTreasure();

};