// MOsipa 2018

#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Movement Component"));
	ProjectileMovementComponent->bAutoActivate = false;

	CollisionMesh = CreateDefaultSubobject<UCapsuleComponent>(FName("Collision Mesh"));
	CollisionMesh->InitCapsuleSize(15.f, 15.f);
	RootComponent = Cast<USceneComponent>(CollisionMesh);
	CollisionMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Base Mesh"));
	BaseMesh->SetRelativeScale3D(FVector(.25f, .25f, .25f));
	BaseMesh->SetupAttachment(RootComponent);

	//Comment to make it visible
	BaseMesh->SetVisibility(false);

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Explosion Force"));
	ExplosionForce->Radius = 50.f;
	ExplosionForce->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::LaunchProjectile()
{
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * ProjectileSpeed);
	ProjectileMovementComponent->Activate();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AProjectile::OnTimerExpire, DestroyDelay, false);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplosionForce->FireImpulse();
	SetRootComponent(ExplosionForce);
	CollisionMesh->DestroyComponent();

	UGameplayStatics::ApplyDamage(
		OtherActor,
		ProjectileDamage,
		GetWorld()->GetFirstPlayerController(),
		this,
		UDamageType::StaticClass()
	);
}

void AProjectile::OnTimerExpire()
{
	this->Destroy();
}