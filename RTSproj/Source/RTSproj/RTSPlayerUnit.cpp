// MOsipa 2018

#include "RTSPlayerUnit.h"
#include "Components/DecalComponent.h"

void ARTSPlayerUnit::Select()
{
	CursorToWorld->SetVisibility(true);
}

void ARTSPlayerUnit::Unselect()
{
	CursorToWorld->SetVisibility(false);
}


