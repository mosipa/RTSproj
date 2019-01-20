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
	UFUNCTION(BlueprintCallable)
		virtual	void ReleaseUnits() override;
};
