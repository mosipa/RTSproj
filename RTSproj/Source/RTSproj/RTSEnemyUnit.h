// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "RTSCharacter.h"
#include "RTSEnemyUnit.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API ARTSEnemyUnit : public ARTSCharacter
{
	GENERATED_BODY()
	
public:
	ARTSEnemyUnit();

	FORCEINLINE TArray<AActor*> GetPatrolPoints() { return PatrolPoints; }

	virtual void Shoot() override;
	void InsideTower(class AGuardTower* Tower);
	class AGuardTower* GetTower();
	FORCEINLINE void SetNearTower(bool bNear) { this->bNearTower = bNear; }
	FORCEINLINE bool IsNearTower() { return this->bNearTower; }

private:
	UPROPERTY(EditAnywhere, Category = "Patrol")
		TArray<AActor*> PatrolPoints;

	float Cooldown;
	float LastFired;

	bool CheckCooldown();

	class AGuardTower* InTower;
	bool bNearTower;
};