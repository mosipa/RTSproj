// MOsipa 2018

#include "GuardTower.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "RTSPlayerUnit.h"
#include "Projectile.h"

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

	//Time required to lock target before shooting
	PrepareTime = .5f;
}

void AGuardTower::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping GuardTower: %s"), *(this->GetName()));
}

void AGuardTower::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping GuardTower: %s stopped"), *(this->GetName()));
}

void AGuardTower::PrepareToFire(ARTSPlayerUnit* PlayerUnit)
{
	//Clear Timer of last spotted PlayerUnit
	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, this, &AGuardTower::OpenFire, PrepareTime, false);
}

void AGuardTower::OpenFire()
{
	auto Projectile = GetWorld()->SpawnActor<AProjectile>(
		this->GetActorLocation() + this->GetActorForwardVector() * 100.f,
		this->GetActorRotation()
		);

	if (Projectile)
	{
		Projectile->LaunchProjectile();
	}

	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
}