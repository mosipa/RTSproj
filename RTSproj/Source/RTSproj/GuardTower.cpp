// MOsipa 2018

#include "GuardTower.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "RTSPlayerUnit.h"
#include "Projectile.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "RTSEnemyUnit.h"

AGuardTower::AGuardTower()
{
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComponent"));
	CollisionComponent->InitCapsuleSize(45.f, 100.f);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGuardTower::OnOverlapBegin);

	SetRootComponent(Cast<USceneComponent>(CollisionComponent));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BaseMesh"));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->SetRelativeScale3D(FVector(1.f, 1.f, 4.f));
	BaseMesh->SetRelativeLocation(FVector(-75.f, 0.f, 100.f));

	Peek = CreateDefaultSubobject<UStaticMeshComponent>(FName("Peek"));
	Peek->SetupAttachment(BaseMesh);
	Peek->SetRelativeLocation(FVector(0.f, 0.f, 65.f));

	UnitsInside.Empty();
}

void AGuardTower::UnitEntered(ARTSEnemyUnit* EnemyUnit)
{
	this->UnitsInside.Add(EnemyUnit);
}

void AGuardTower::UnitLeft(ARTSEnemyUnit* EnemyUnit)
{
	TArray<ARTSEnemyUnit*> TempUnitsInside;
	TempUnitsInside.Empty();

	for (auto& Enemy : UnitsInside)
	{
		if (!(EnemyUnit->GetName().Equals(Enemy->GetName())))
		{
			TempUnitsInside.Add(Enemy);
		}
	}

	UnitsInside = TempUnitsInside;
}

void AGuardTower::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARTSEnemyUnit* Enemy = Cast<ARTSEnemyUnit>(OtherActor);
	if (Enemy)
	{
		Enemy->InsideTower(Cast<AGuardTower>(OverlappedComp->GetOwner()));
		Enemy->SetNearTower(true);
	}
}

void AGuardTower::OpenFire(ARTSPlayerUnit* PlayerUnit)
{
	//Rotates guard located at peek
	FRotator PeekRotation = UKismetMathLibrary::FindLookAtRotation(this->Peek->GetComponentLocation(), PlayerUnit->GetActorLocation());
	this->Peek->SetWorldRotation(PeekRotation);

	//Count cooldown
	float CooldownLeft = GetWorld()->GetTimeSeconds() - LastTimeFired;

	if (CooldownLeft > Cooldown)
	{
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			Projectile_BP,
			this->Peek->GetComponentLocation() + this->Peek->GetForwardVector() * 100.f,
			this->Peek->GetComponentRotation()
			);

		if (Projectile)
		{
			LastTimeFired = GetWorld()->GetTimeSeconds();
			Projectile->LaunchProjectile();
		}
	}
}