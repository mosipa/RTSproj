// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "PlayersHideout.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API APlayersHideout : public ABuilding
{
	GENERATED_BODY()

public:
	APlayersHideout();

	UFUNCTION(BlueprintCallable)
		virtual	void ReleaseUnits() override;

	FORCEINLINE TArray<FVector> GetPossibleCamLocations() { return PossibleCamLocations; }
	FORCEINLINE TArray<FRotator> GetPossibleCamRotations() { return PossibleCamRotations; }

	void ToggleTransparency(float OpacityValue);
//TESTING
private:
	TArray<FVector> PossibleCamLocations;
	TArray<FRotator> PossibleCamRotations;

	class UMaterial* StoredMaterial;
	class UMaterialInstanceDynamic* DynamicMaterialInst;
};
