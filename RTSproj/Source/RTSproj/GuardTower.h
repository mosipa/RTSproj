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

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PrepareToFire(class ARTSPlayerUnit* PlayerUnit);

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
	UFUNCTION()
		void OpenFire(class ARTSPlayerUnit* PlayerUnit);

	float PrepareTime;

	FTimerHandle ShootTimerHandle;
};
