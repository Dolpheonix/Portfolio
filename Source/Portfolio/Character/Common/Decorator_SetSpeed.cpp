#include "Decorator_SetSpeed.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

UDecorator_SetSpeed::UDecorator_SetSpeed() : Speed(200.f)
{
	bNotifyActivation = true;
}

void UDecorator_SetSpeed::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	TObjectPtr<ACharacter> ownerCharacter = SearchData.OwnerComp.GetAIOwner()->GetCharacter();

	check(ownerCharacter);

	ownerCharacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
}