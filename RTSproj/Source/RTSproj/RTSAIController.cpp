// MOsipa 2018

#include "RTSAIController.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"

void ARTSAIController::Move(FVector MoveTo)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MoveTo);
}


void ARTSAIController::PresentYourself()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor: %s has controller: %s"), *(this->GetPawn()->GetName()), *(this->GetName()));
}
