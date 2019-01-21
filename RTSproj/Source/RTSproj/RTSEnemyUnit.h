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

	void Shoot();

private:
	UPROPERTY(EditAnywhere, Category = "Patrol")
		TArray<AActor*> PatrolPoints;

	float Cooldown;
	float LastFired;

	bool CheckCooldown();
};