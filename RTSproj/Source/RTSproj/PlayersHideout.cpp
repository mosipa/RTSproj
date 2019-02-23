// MOsipa 2018

#include "PlayersHideout.h"
#include "RTSPlayerUnit.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

APlayersHideout::APlayersHideout()
{
	//Find material base mesh is made of
	ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Materials/M_Building"));
	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
	}

	PossibleCamLocations.Add(FVector(-330.f, 0.f, 150.f));
	PossibleCamLocations.Add(FVector(-10.f, -450.f, 150.f));
	PossibleCamLocations.Add(FVector(170.f, 145.f, 150.f));

	PossibleCamRotations.Add(FRotator(-20.f, 0.f, 0.f));
	PossibleCamRotations.Add(FRotator(-20.f, 60.f, 0.f));
	PossibleCamRotations.Add(FRotator(-20.f, 240.f, 0.f));

	bChangedMaterial = false;
}

void APlayersHideout::ToggleTransparency()
{
	if (!StoredMaterial || !BaseMesh) { return; }

	//Create dynamic material
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterial, BaseMesh);

	if (!DynamicMaterialInst) { return; }
	float OpacityValue;

	//If building' material is transparent
	if (bChangedMaterial)
	{
		OpacityValue = 1.f;
		bChangedMaterial = false;
	}
	//If building's material is solid
	else
	{
		OpacityValue = 0.2f;
		bChangedMaterial = true;
	}

	//Change value of opacity parameter of material
	DynamicMaterialInst->SetScalarParameterValue("Opacity", OpacityValue);
	this->BaseMesh->SetMaterial(0, DynamicMaterialInst);
}

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