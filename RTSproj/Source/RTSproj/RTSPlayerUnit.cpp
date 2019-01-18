// MOsipa 2018

#include "RTSPlayerUnit.h"
#include "Components/DecalComponent.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Building.h"

ARTSPlayerUnit::ARTSPlayerUnit()
{
	//HealthBar
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("WidgetComponent");
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));

	bIsArrested = false;
	bIsInBuilding = false;
	bInBuildingsRange = false;

	NearBuilding = nullptr;
}

void ARTSPlayerUnit::Select()
{
	CursorToWorld->SetVisibility(true);
}

void ARTSPlayerUnit::Unselect()
{
	CursorToWorld->SetVisibility(false);
}

void ARTSPlayerUnit::HealthBarInvisible(bool bVisible)
{
	WidgetComponent->bHiddenInGame = bVisible;
}

void ARTSPlayerUnit::SetInBuildingsRange(bool bInRange, ABuilding* Building) 
{
	bInBuildingsRange = bInRange;
	NearBuilding = Building;
}

ABuilding* ARTSPlayerUnit::GetNearestBuilding() 
{ 
	return NearBuilding;
}