// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Prison.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJ_API APrison : public ABuilding
{
	GENERATED_BODY()
	
public:
	APrison();

	virtual void ReleaseUnits() override;

private:
	bool bIsAnyUnitInside;
};
