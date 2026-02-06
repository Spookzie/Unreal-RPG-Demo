#include "Items/Breakables/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	geometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(geometryCollection);
	geometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	geometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	geometryCollection->SetGenerateOverlapEvents(true);

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	capsule->SetupAttachment(GetRootComponent());
	capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	geometryCollection->SetNotifyBreaks(true);
	geometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaosBreak);
}


void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABreakableActor::GetHit_Implementation(const FVector& impactPoint, AActor* hitter)
{
}


/*	Helpers/Callbacks	*/
void ABreakableActor::OnChaosBreak(const FChaosBreakEvent& breakEvent)
{
	if (bBroken)
		return;
	bBroken = true;

	SetLifeSpan(3.f);
	capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnTreasure();
}


void ABreakableActor::SpawnTreasure()
{
	if (FMath::FRand() <= 0.75f)	// 75% chance of spawning treasure
	{
		if (GetWorld() && treasureClasses.Num() > 0)
		{
			FVector loc = GetActorLocation();
			loc.Z += 25.f;

			const int32 idx = FMath::RandRange(0, treasureClasses.Num() - 1);

			GetWorld()->SpawnActor<ATreasure>(
				treasureClasses[idx],
				loc,
				GetActorRotation()
			);
		}
	}

}