// MOsipa 2018

#include "Prison.h"
#include "RTSPlayerUnit.h"

APrison::APrison()
{
	bIsAnyUnitInside = false;
}

void APrison::ReleaseUnits()
{
	UE_LOG(LogTemp, Warning, TEXT("Releasing from prison"));

	if (UnitsInside.Num() > 0)
	{
		for (ARTSPlayerUnit* PlayerUnit : UnitsInside)
		{
			PlayerUnit->HealthBarInvisible(false);
			PlayerUnit->SetInBuilding(false);
			PlayerUnit->SetActorHiddenInGame(false);
			PlayerUnit->SetActorEnableCollision(true);

			PlayerUnit->SetArrested(false);
		}

		UnitsInside.Empty();
	}
}