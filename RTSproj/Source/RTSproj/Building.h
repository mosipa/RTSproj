// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UCLASS()
class RTSPROJ_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* BaseMesh = nullptr;
};
