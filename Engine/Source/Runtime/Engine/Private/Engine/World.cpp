#include "Engine/World.h"
#include "Engine/Level.h"

FActorSpawnParameters::FActorSpawnParameters()
	: Name(NAME_NONE)
	, Template(nullptr)
	, Owner(nullptr)
	, Instigator(nullptr)
	, TransformScaleMethod(ESpawnActorScaleMethod::MultiplyWithRoot)
	, ObjectFlags(EObjectFlags::RF_NoFlags) 
{
}

void UWorld::InitalizeNewWorld()
{
	PersistentLevel = NewObject<ULevel>(this, ULevel::StaticClass(), TEXT("PersistentLevel"));
	AActor* Actor = SpawnActor<AActor>(nullptr, FTransform::Identity);
	shared_ptr<AActor> A = Actor->As<AActor>();
}

AActor* UWorld::SpawnActor(UClass* Class, FTransform const* Transform, const FActorSpawnParameters& SpawnParameters)
{
	_ASSERT(PersistentLevel.get());

	// Make sure this class is spawnable.
	if (!Class)
	{
		E_LOG(Warning, TEXT("SpawnActor failed because no class was specified"));
		return NULL;
	}
	else if (!Class->IsChildOf(AActor::StaticClass()))
	{
		E_LOG(Warning, TEXT("SpawnActor failed because {} is not an actor class"), Class->GetName());
		return NULL;
	}
	else if (SpawnParameters.Template != NULL && SpawnParameters.Template->GetClass() != Class)
	{
		E_LOG(Warning, TEXT("SpawnActor failed because template class ({}) does not match spawn class ({})"), SpawnParameters.Template->GetClass()->GetName(), Class->GetName());
		return NULL;
	}

	ULevel* LevelToSpawnIn = PersistentLevel.get();

	// 템플릿이 제공되지 않은 경우 클래스의 기본 액터를 템플릿으로 사용합니다.
	AActor* Template = SpawnParameters.Template ? SpawnParameters.Template : Class->GetDefaultObject<AActor>();
	_ASSERT(Template);

	return nullptr;
}

