#include "Engine/Engine.h"
#include "EditorViewportClient.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, Engine);

TObjectPtr<UEngine> GEngine;
ENGINE_API UWorld* GWorld = nullptr;

UEngine::UEngine()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }
	E_LOG(Warning, TEXT("{} Hello"), GetName());
}

void UEngine::Init(HWND hViewportHandle)
{
	MainViewportHandle = hViewportHandle;

	World = NewObject<UWorld>(this, UWorld::StaticClass(), TEXT("Editor World"));
	World->WorldType - EWorldType::Editor;
	EditorWorld = World;
	GWorld = World.get();

	GWorld->InitalizeNewWorld();

	EditorViewportClient = NewObject<UEditorViewportClient>(this, nullptr, TEXT("EditorViewportClient"));
	EditorViewportClient->Init(hViewportHandle, GWorld);
}

void UEngine::Tick(float DeltaSeconds)
{
	EditorViewportClient->Draw();
}
  