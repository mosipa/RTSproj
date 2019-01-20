// MOsipa 2018

#include "PlayersHideout.h"
#include "RTSPlayerUnit.h"

void APlayersHideout::ReleaseUnits()
{
	//TODO units may be stuck due to number of units that reappear
	//find a way to prevent this from happening
	if (UnitsInside.Num() > 0)
	{
		for (ARTSPlayerUnit* PlayerUnit : UnitsInside)
		{
			PlayerUnit->HealthBarInvisible(false);
			PlayerUnit->SetInBuilding(false);
			PlayerUnit->SetActorHiddenInGame(false);
			PlayerUnit->SetActorEnableCollision(true);
		}

		UnitsInside.Empty();
	}
}