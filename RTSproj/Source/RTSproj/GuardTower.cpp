// MOsipa 2018

#include "GuardTower.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

AGuardTower::AGuardTower()
{
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComponent"));
	CollisionComponent->InitCapsuleSize(75.f, 100.f);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGuardTower::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AGuardTower::OnOverlapEnd);

	SetRootComponent(Cast<USceneComponent>(CollisionComponent));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BaseMesh"));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->SetRelativeScale3D(FVector(1.f, 1.f, 4.f));
	BaseMesh->SetRelativeLocation(FVector(-50.f, 0.f, 100.f));

	UnitsInside.Empty();
}

void AGuardTower::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping GuardTower: %s"), *(this->GetName()));
}

void AGuardTower::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping GuardTower: %s stopped"), *(this->GetName()));
}