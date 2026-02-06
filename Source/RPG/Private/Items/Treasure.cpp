#include "Items/Treasure.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/GameplayStatics.h"


void ATreasure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldRotation(FRotator(0.f, DeltaTime * 50.f, 0.f));
}


void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* pickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		pickupInterface->AddGold(this);

		PlayPickupSound();
		Destroy();
	}
}
