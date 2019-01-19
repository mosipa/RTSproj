// MOsipa 2018

#include "Building.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "RTSPlayerUnit.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComponent"));
	CollisionComponent->InitCapsuleSize(75.f, 100.f);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ABuilding::OnOverlapEnd);

	SetRootComponent(Cast<USceneComponent>(CollisionComponent));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BaseMesh"));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->SetRelativeScale3D(FVector(1.f, 3.f, 2.f));
	BaseMesh->SetRelativeLocation(FVector(-50.f, 0.f, 0.f));

	UnitsInside.Empty();
}

void ABuilding::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARTSPlayerUnit* PlayerUnit = Cast<ARTSPlayerUnit>(OtherActor);
	PlayerUnit->SetInBuildingsRange(true, this);
}

void ABuilding::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARTSPlayerUnit* PlayerUnit = Cast<ARTSPlayerUnit>(OtherActor);
	PlayerUnit->SetInBuildingsRange(false, nullptr);
}

void ABuilding::ReleaseUnits()
{
	//TODO units may be stuck due to number of units that reappear
	//find a way to prevent this from happening
	if (UnitsInside.Num() > 0)
	{
		for (ARTSPlayerUnit* PlayerUnit : UnitsInside)
		{
			PlayerUnit->HealthBarInvisible(false);
			PlayerUnit->SetInBuilding(false);
			PlayerUnit->SetActorHiddenInGame(false);
			PlayerUnit->SetActorEnableCollision(true);
		}

		UnitsInside.Empty();
	}
}