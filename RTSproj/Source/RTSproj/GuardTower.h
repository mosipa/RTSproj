// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GuardTower.generated.h"

UCLASS()
class RTSPROJ_API AGuardTower : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGuardTower();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OpenFire(class ARTSPlayerUnit* PlayerUnit);

	void UnitEntered(class ARTSEnemyUnit* EnemyUnit);
	void UnitLeft(class ARTSEnemyUnit* EnemyUnit);
	FORCEINLINE bool IsAnybodyInside() { return (this->UnitsInside.Num() != 0); }

protected:
	TArray<class ARTSEnemyUnit*> UnitsInside;

	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* BaseMesh = nullptr;

	//Place to shoot down at PlayerUnits
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Peek = nullptr;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectile> Projectile_BP;

private:
	UPROPERTY(EditAnywhere, Category = "Firing")
		float Cooldown = 2.f;

	float LastTimeFired = 0.f;;
};
