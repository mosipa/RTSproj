// MOsipa 2018

#include "MakeArrest.h"
#include "EnemyAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"


EBTNodeResult::Type UMakeArrest::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	Pawn = Cast<AEnemyAIController>(OwnerComp.GetOwner())->GetPawn();

	auto Target = Cast<AActor>(Blackboard->GetValueAsObject("EnemyKey"));

	//Rotate AI to face Enemy 
	FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), Target->GetActorLocation());
	Pawn->SetActorRotation(BodyRotation);

	UE_LOG(LogTemp, Warning, TEXT("Unit: %s under arrest"), *(Target->GetName()));

	return EBTNodeResult::Succeeded;
}