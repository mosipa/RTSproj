// MOsipa 2018

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class RTSPROJ_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	void LaunchProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
		class UCapsuleComponent* CollisionMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* BaseMesh = nullptr;

	class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	float ProjectileSpeed = 5000.f;
	float ProjectileDamage = 80.f;

	UPROPERTY(VisibleAnywhere, Category = Component)
		class URadialForceComponent* ExplosionForce = nullptr;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	float DestroyDelay = 3.f;

	void OnTimerExpire();
};
