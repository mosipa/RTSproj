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
	FORCEINLINE TArray<AActor*> GetPatrolPoints() { return PatrolPoints; }

private:
	UPROPERTY(EditAnywhere, Category = "Patrol")
		TArray<AActor*> PatrolPoints;
};