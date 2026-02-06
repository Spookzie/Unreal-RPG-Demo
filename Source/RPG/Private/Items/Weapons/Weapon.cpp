#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "Items/Breakables/BreakableActor.h"
#include "NiagaraComponent.h"


AWeapon::AWeapon()
{
	weaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	weaponBox->SetupAttachment(GetRootComponent());
	weaponBox->SetBoxExtent(FVector(2.f, 1.f, 40.f));
	weaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	weaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	weaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	boxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	boxTraceStart->SetupAttachment(GetRootComponent());

	boxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	boxTraceEnd->SetupAttachment(GetRootComponent());
}


void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	weaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}


void AWeapon::Equip(USceneComponent* inParent, FName inSocketName, AActor* newOwner, APawn* newInstigator)
{
	itemState = EItemState::EIS_Equipped;
	SetOwner(newOwner);
	SetInstigator(newInstigator);
	AttachMeshToSocket(inParent, inSocketName);

	if (pickupSphere)
		pickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (itemEffect)
		itemEffect->Deactivate();
}


void AWeapon::AttachMeshToSocket(USceneComponent* inParent, const FName& inSocketName)
{
	itemMesh->AttachToComponent(
		inParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		inSocketName
	);
}


/*	Box Collision Callbacks	*/
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor))
		return;

	FHitResult boxHit;
	BoxTrace(boxHit);

	// Calling the enemies' gethit function
	if (AActor* actorHit = boxHit.GetActor())
	{
		if (ActorIsSameType(actorHit))
			return;

		const FVector hitPoint =
			boxHit.bBlockingHit ? boxHit.ImpactPoint : boxHit.Location;

		PlayPotBreakSound(hitPoint);

		// Damaging
		UGameplayStatics::ApplyDamage(
			actorHit,
			damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		if (IHitInterface* hitInterface = Cast<IHitInterface>(actorHit))
		{
			hitInterface->Execute_GetHit(actorHit, hitPoint, GetOwner());
		}
		
		CreateFields(hitPoint);	// Call the field strain to break objects
	}
}


/*	Attributes	*/
void AWeapon::PlayPotBreakSound(const FVector& impactPoint)
{
	TArray<AActor*> overlappingActors;

	// Get the overlapping pots
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		impactPoint,
		200.f,	// Same radius as strain
		{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible) },
		ABreakableActor::StaticClass(),
		{},
		overlappingActors
	);

	const int32 PotsHit = overlappingActors.Num();

	if (PotsHit > 1 && multiPotBreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			multiPotBreakSound,
			impactPoint
		);
	}
	else if (PotsHit == 1 && singlePotBreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			singlePotBreakSound,
			impactPoint
		);
	}
}


/*	Box Tracing	*/
void AWeapon::BoxTrace(FHitResult& boxHit)
{
	const FVector start = boxTraceStart->GetComponentLocation();
	const FVector end = boxTraceEnd->GetComponentLocation();

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	actorsToIgnore.Add(GetOwner());

	// Preventing double hit
	for (AActor* actor : ignoreActors)
		actorsToIgnore.AddUnique(actor);


	UKismetSystemLibrary::BoxTraceSingle(
		this, start, end,
		boxTraceExtent,
		boxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		actorsToIgnore,
		bShowBoxTraceDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		boxHit,
		true
	);

	ignoreActors.AddUnique(boxHit.GetActor());
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}