#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"


void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* pickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		pickupInterface->AddSouls(this);

		SpawnPickupSystem();
		PlayPickupSound();
		Destroy();
	}
}