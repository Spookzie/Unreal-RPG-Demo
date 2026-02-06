#include "Items/Item.h"
#include "RPG/RPG.h"
#include "Components/SphereComponent.h"
#include "Characters/MainCharacter.h"
#include "NiagaraComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(itemMesh);

	pickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	pickupSphere->SetupAttachment(GetRootComponent());

	itemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	itemEffect->SetupAttachment(GetRootComponent());
}


void AItem::BeginPlay()
{
	Super::BeginPlay();

	/* 
	* We have a delegate OnComponentBeginOverlap in PrimitiveComponent.h
	* OnSphereOverlap = Our callback function
	* We bind our callback to the delegate using AddDynamic 
	********************/
	pickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	pickupSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}


/*	Sphere Component	*/
void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ 
	if (IPickupInterface* pickupInterface = Cast<IPickupInterface>(OtherActor))
		pickupInterface->SetOverlappingItem(this);
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IPickupInterface* pickupInterface = Cast<IPickupInterface>(OtherActor))
		pickupInterface->SetOverlappingItem(nullptr);
}


/*	Effects	*/
void AItem::SpawnPickupSystem()
{
	if (pickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			pickupEffect,
			GetActorLocation()
		);
	}
}

void AItem::PlayPickupSound()
{
	if (pickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			pickupSound,
			GetActorLocation()
		);
	}
}


/*	Movement	*/
void AItem::SinMovement(float DeltaTime)
{
	runningTime += DeltaTime;
	float deltaZ = amplitude * FMath::Sin(runningTime * timeConstant);

	AddActorWorldOffset(FVector(0.f, 0.f, deltaZ));
}


void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(itemState == EItemState::EIS_Hovering)
		SinMovement(DeltaTime);
}