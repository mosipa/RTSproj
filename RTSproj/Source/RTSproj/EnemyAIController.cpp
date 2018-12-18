// MOsipa 2018

#include "EnemyAIController.h"

void AEnemyAIController::BeginPlay()
{
	auto AIPawnName = this->GetPawn()->GetName();
	UE_LOG(LogTemp, Warning, TEXT("%s has %s"), *(AIPawnName), *(this->GetName()));
}


