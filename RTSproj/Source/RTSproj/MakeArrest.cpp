// MOsipa 2018

#include "MakeArrest.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/World.h"
#include "RTSCharacter.h"
#include "RTSAIController.h"
#include "RTSPlayerUnit.h"

EBTNodeResult::Type UMakeArrest::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	BlackboardComponent = OwnerComp.GetBlackboardComponent();

	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();
	if (!Pawn) { return EBTNodeResult::Failed; }

	Target = Cast<ARTSCharacter>(BlackboardComponent->GetValueAsObject("PlayerUnitKey"));
	if (!Target) { return EBTNodeResult::Failed; }

	bool bPlayerUnitBehaveWierd = Cast<ARTSAIController>(Cast<ACharacter>(Target)->GetController())->IsUnitBusy();

	//Rotate AI to face player unit 
	FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
	Pawn->SetActorRotation(BodyRotation);

	float DistanceToPrison = GetDistance(Target->GetActorLocation(), PRISON_LOCATION);

	//Player unit tries to do something
	if (bPlayerUnitBehaveWierd)
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 600.f;
		Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 600.f;
		BlackboardComponent->SetValueAsVector("PlayerUnitLocation", Target->GetActorLocation());
		BlackboardComponent->SetValueAsBool("PlayerUnitOnMove", true);
	}
	//Player unit doesn't move so get closer
	else if(!bPlayerUnitBehaveWierd && DistanceToPrison > 150.f)
	{
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 200.f;
		UAIBlueprintHelperLibrary::SimpleMoveToActor(Cast<AEnemyAIController>(OwnerComp.GetOwner()), Target);

		float Distance = GetDistance(Pawn->GetActorLocation(), Target->GetActorLocation());

		//If close enough, make arrest - put him in prison
		if (GetDistance(Pawn->GetActorLocation(), Target->GetActorLocation()) <= 100.f)
		{
			Cast<ARTSPlayerUnit>(Target)->SetArrested(true);
			Target->SetActorRotation(Pawn->GetActorRotation());
			Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 200.f;
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cast<APawn>(Target)->GetController(), PRISON_LOCATION);
		}
	}
	//TODO a bit buggy
	else if (!bPlayerUnitBehaveWierd && DistanceToPrison <= 150.f)
	{	
		Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent())->MaxWalkSpeed = 600.f;
		Cast<UCharacterMovementComponent>(Cast<APawn>(Target)->GetMovementComponent())->MaxWalkSpeed = 600.f;
		BlackboardComponent->ClearValue("PlayerUnitKey");
		BlackboardComponent->ClearValue("PlayerUnitLocation");
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Succeeded;
}

float UMakeArrest::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}