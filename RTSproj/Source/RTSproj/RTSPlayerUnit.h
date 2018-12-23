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

	FORCEINLINE void SetArrested(bool Arrested) { bIsArrested = Arrested; }
	FORCEINLINE bool IsCharacterArrested() { return bIsArrested; }

private:
	bool bIsArrested;
};
