// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UCLASS(abstract)
class RTSPROJ_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	ABuilding();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	FORCEINLINE void UnitEntered(class ARTSPlayerUnit* Unit) { UnitsInside.Add(Unit); }

	virtual	void ReleaseUnits() PURE_VIRTUAL(&ABuilding::ReleaseUnits,);

protected:
	TArray<class ARTSPlayerUnit*> UnitsInside;

	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* BaseMesh = nullptr;
};
