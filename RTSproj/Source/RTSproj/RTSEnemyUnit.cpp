// MOsipa 2018

#include "RTSEnemyUnit.h"
#include "Engine/World.h"
#include "Projectile.h"

ARTSEnemyUnit::ARTSEnemyUnit()
{
	Cooldown = 2.f;
	LastFired = 0.f;
}

void ARTSEnemyUnit::Shoot()
{
	//TODO make gun actor and then check if unit has a gun, and on gun check if its on cooldown
	//Check if gun is on cooldown
	if (CheckCooldown())
	{
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			this->GetActorLocation() + this->GetActorForwardVector() * 100.f,
			this->GetActorRotation()
			);

		if (!Projectile) { return; }
		Projectile->LaunchProjectile();

		LastFired = GetWorld()->GetTimeSeconds();
	}
}

bool ARTSEnemyUnit::CheckCooldown()
{
	return ((GetWorld()->GetTimeSeconds() - LastFired) >= Cooldown);
}
