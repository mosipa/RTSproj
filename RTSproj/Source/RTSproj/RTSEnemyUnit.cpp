// MOsipa 2018

#include "RTSEnemyUnit.h"
#include "Engine/World.h"
#include "GuardTower.h"

ARTSEnemyUnit::ARTSEnemyUnit()
{
	Cooldown = 2.f;
	LastFired = 0.f;
	bNearTower = false;
}

void ARTSEnemyUnit::Shoot()
{
	//Check if gun is on cooldown
	if (CheckCooldown())
	{
		Super::Shoot();

		LastFired = GetWorld()->GetTimeSeconds();
	}
}

bool ARTSEnemyUnit::CheckCooldown()
{
	return ((GetWorld()->GetTimeSeconds() - LastFired) >= Cooldown);
}

void ARTSEnemyUnit::InsideTower(AGuardTower* Tower)
{
	InTower = Tower;
}

AGuardTower* ARTSEnemyUnit::GetTower()
{
	return InTower;
}