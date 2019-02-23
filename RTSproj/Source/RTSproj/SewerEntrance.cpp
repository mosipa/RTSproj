// MOsipa 2018

#include "SewerEntrance.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "RTSPlayerUnit.h"

// Sets default values
ASewerEntrance::ASewerEntrance()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	SetRootComponent(Cast<USceneComponent>(BaseMesh));

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
	CollisionBox->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASewerEntrance::OnOverlapBegin);
}

void ASewerEntrance::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->IsChildOf<ARTSPlayerUnit>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapping by %s"), *(OtherActor->GetName()));
	}
}