// MOsipa 2018

#include "Building.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "RTSPlayerUnit.h"
#include "RTSAIController.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComponent"));
	CollisionComponent->InitCapsuleSize(75.f, 100.f);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABuilding::OnOverlapBegin);

	SetRootComponent(Cast<USceneComponent>(CollisionComponent));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BaseMesh"));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->SetRelativeLocation(FVector(-50.f, 0.f, 0.f));

	UnitsInside.Empty();
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("%i"), UnitsInside.Num());
}

void ABuilding::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *(OtherActor->GetName()));

	UnitsInside.Add(Cast<ARTSPlayerUnit>(OtherActor));
	
	ARTSPlayerUnit* Character = Cast<ARTSPlayerUnit>(OtherActor);

	//Stops, sets bool and hides unit from player
	//TODO move? somewhere unit which has enetered a building or delete/despawn
	//TODO units get inside building when they're near it - make player click on the building
	Character->HealthBarVisible(true);
	Character->GetMovementComponent()->StopMovementImmediately();
	Character->SetInBuilding(true);
	Cast<ARTSPlayerUnit>(OtherActor)->SetActorHiddenInGame(true);
	Cast<ARTSPlayerUnit>(OtherActor)->SetActorEnableCollision(false);
}