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

	FORCEINLINE void SetArrested(bool bArrested) { bIsArrested = bArrested; }
	FORCEINLINE bool IsCharacterArrested() { return bIsArrested; }
	FORCEINLINE bool IsCharacterInBuilding() { return bIsInBuilding; }
	FORCEINLINE void SetInBuilding(bool bInBuilding) { bIsInBuilding = bInBuilding; }

	void HealthBarVisible(bool bToggle);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UWidgetComponent* WidgetComponent;

private:
	bool bIsArrested;
	bool bIsInBuilding;
};
