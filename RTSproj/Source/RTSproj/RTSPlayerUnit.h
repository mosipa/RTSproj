// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "RTSCharacter.h"
#include "RTSPlayerUnit.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API ARTSPlayerUnit : public ARTSCharacter
{
	GENERATED_BODY()

	ARTSPlayerUnit();

public:
	void Select();
	void Unselect();

	FORCEINLINE bool IsCharacterArrested() { return bIsArrested; }
	FORCEINLINE void SetArrested(bool bArrested) { bIsArrested = bArrested; }
	FORCEINLINE bool IsCharacterInBuilding() { return bIsInBuilding; }
	FORCEINLINE void SetInBuilding(bool bInBuilding) { bIsInBuilding = bInBuilding; }
	FORCEINLINE bool IsCharacterInBuildingsRange() { return bInBuildingsRange; }
	void SetInBuildingsRange(bool bInRange, class ABuilding* Building);

	void HealthBarInvisible(bool bVisible);

	class ABuilding* GetNearestBuilding();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UWidgetComponent* WidgetComponent;

private:
	bool bIsArrested;
	bool bIsInBuilding;
	bool bInBuildingsRange;

	class ABuilding* NearBuilding;
};
