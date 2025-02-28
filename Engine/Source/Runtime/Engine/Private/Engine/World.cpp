#include "Engine/World.h"
#include "Engine/Level.h"

#include "EngineModule.h"
#include "RendererInterface.h"
#include "SceneInterface.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "Engine/StaticMeshActor.h"



FDelegate<UWorld*> WorldCreatedDelegate;
FDelegate<UWorld*> WorldDestroyedDelegate;

FActorSpawnParameters::FActorSpawnParameters()
	: Name(NAME_NONE)
	, Template(nullptr)
	, Owner(nullptr)
	, Instigator(nullptr)
	, TransformScaleMethod(ESpawnActorScaleMethod::MultiplyWithRoot)
	, ObjectFlags(EObjectFlags::RF_NoFlags) 
{
}

UWorld::UWorld()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }

	WorldCreatedDelegate.Broadcast(this);
}

UWorld::~UWorld()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }
	WorldDestroyedDelegate.Broadcast(this);


	if (Scene)
	{
		Scene->Release();
		Scene = nullptr;
	}
}

void UWorld::InitalizeNewWorld()
{
	PersistentLevel = NewObject<ULevel>(this, ULevel::StaticClass(), TEXT("PersistentLevel"));
	PersistentLevel->OwningWorld = this->As<UWorld>();

	InitWorld();

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	FTransform TransformLeft = FTransform(FRotator::ZeroRotator, FVector3D(200.f, -100.f, 0.f), FVector3D(0.5f, 0.5f, 0.5f));
	FTransform TransformRight = FTransform(FRotator::ZeroRotator, FVector3D(200.f, 100.f, 0.f), FVector3D(0.5f, 0.5f, 0.5f));	
	AStaticMeshActor* Actor = SpawnActor<AStaticMeshActor>(nullptr, TransformLeft, ActorSpawnParameters);
	AStaticMeshActor* Actor2 = SpawnActor<AStaticMeshActor>(nullptr, TransformRight, ActorSpawnParameters);

}

void UWorld::InitializeActorsForPlay(FRegisterComponentContext* Context)
{
	// 한 그룹의 업데이트와 모든 레벨의 구성 요소를 업데이트합니다.
	// 쿠킹된 데이터가 있거나 편집기에서 플레이 중이라면, 빈번히 콜드 데이터를 다시 실행할 필요가 없습니다.
	// 디스크에서 로드되었는지 아니면 복제되었는지에 따라 PIE 세계가 업데이트되었는지 여부를 확인합니다.
	UpdateWorldComponents(Context);
}

void UWorld::UpdateWorldComponents(FRegisterComponentContext* Context)
{
	PersistentLevel->UpdateLevelComponents(Context);
}

void UWorld::InitWorld()
{
	GetRendererModule().AllocateScene(this, ERHIFeatureLevel::SM5);
}

void UWorld::Tick(float DeltaSeconds)
{
	if (WorldType == EWorldType::PIE)
	{
		for (TObjectPtr<AActor> Actor : PersistentLevel->Actors)
		{
			Actor->Tick(DeltaSeconds);
		}
	}
}

void UWorld::Serialize(FArchive& Ar)
{
	//Super::Serialize(Ar);

	Ar << PersistentLevel;
}

string UWorld::Save()
{
	std::stringstream Buffer;
	boost::archive::text_oarchive SaveArchive = boost::archive::text_oarchive(Buffer);
	FArchive Ar = FArchive(SaveArchive);

	uint64 ObjectCount = PersistentLevel->Actors.size();
	Ar << ObjectCount;

	for (TEnginePtr<AActor> Actor : PersistentLevel->Actors)
	{
		Actor->Serialize(Ar);
	}

	string String = Buffer.str();
	return String;
}

void UWorld::Load(const string& InLoadString)
{
	std::stringstream Buffer = std::stringstream(InLoadString);
	boost::archive::text_iarchive LoadArchive = boost::archive::text_iarchive(Buffer);
	FArchive Ar = FArchive(LoadArchive);

	uint64 ObjectCount = 0;
	Ar << ObjectCount;

	for (uint64 i = 0; i < ObjectCount; ++i)
	{
		FString ObjectName;
		Ar << ObjectName;

		auto It = std::find_if(PersistentLevel->Actors.begin(), PersistentLevel->Actors.end(),
			[&ObjectName](TObjectPtr<AActor> Actor)
			{
				if (Actor->GetName() == ObjectName) { return true; }
				return false;
			}
		);

		if (It != PersistentLevel->Actors.end())
		{
			It->get()->Serialize(Ar);
		}
	}
}

TObjectPtr<UWorld> UWorld::GetDuplicatedWorldForPIE(UWorld* InWorld)
{
	TObjectPtr<UWorld> NewWorld = Cast<UWorld>(StaticDuplicateObject(InWorld, nullptr, TEXT("PIE World"), EDuplicateMode::PIE));
	NewWorld->WorldType = EWorldType::PIE;
	return NewWorld;
}

AActor* UWorld::SpawnActor(UClass* Class, FTransform const* UserTransformPtr, const FActorSpawnParameters& SpawnParameters)
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

	FName NewActorName = SpawnParameters.Name;
	FTransform const UserTransform = UserTransformPtr ? *UserTransformPtr : FTransform::Identity;

	EObjectFlags ActorFlags = SpawnParameters.ObjectFlags;

	// actually make the actor object
	shared_ptr<AActor> Actor = NewObject<AActor>(LevelToSpawnIn, Class, NewActorName, ActorFlags, Template);
	LevelToSpawnIn->Actors.push_back(Actor);

	AActor* NewActor = Actor.get();

	NewActor->PostSpawnInitialize(UserTransform, SpawnParameters.Owner, SpawnParameters.Instigator, SpawnParameters.TransformScaleMethod);

	// Broadcast notification of spawn
	OnActorSpawned.Broadcast(NewActor);
	E_LOG(Log, TEXT("Actor Spawned({})"), Class->GetName());
	E_LOG(Warning, TEXT("Actor Spawned({})"), Class->GetName());

	return NewActor;
}

bool UWorld::AreActorsInitialized() const
{
	return bActorsInitialized && PersistentLevel && PersistentLevel->Actors.size();
}

bool UWorld::IsGameWorld() const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE || WorldType == EWorldType::GamePreview || WorldType == EWorldType::GameRPC;
}

bool UWorld::IsEditorWorld() const
{
	return WorldType == EWorldType::Editor || WorldType == EWorldType::EditorPreview || WorldType == EWorldType::PIE;
}

