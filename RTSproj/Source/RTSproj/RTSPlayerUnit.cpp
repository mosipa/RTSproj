// MOsipa 2018

#include "RTSPlayerUnit.h"
#include "Components/DecalComponent.h"

ARTSPlayerUnit::ARTSPlayerUnit()
{
	bIsArrested = false;
}

void ARTSPlayerUnit::Select()
{
	CursorToWorld->SetVisibility(true);
}

void ARTSPlayerUnit::Unselect()
{
	CursorToWorld->SetVisibility(false);
}