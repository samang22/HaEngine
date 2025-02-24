#include "ScenePrivate.h"
#include "EngineModule.h"
#include "RendererInterface.h"
#include "Engine/World.h"

FScene::FScene(UWorld* InWorld, /*bool bInRequiresHitProxies, bool bInIsEditorScene, bool bCreateFXSystem,*/ ERHIFeatureLevel::Type InFeatureLevel)
    : FSceneInterface(InFeatureLevel)
    , World(InWorld)
{
    _ASSERT(World);
    World->Scene = this;
}

FScene::~FScene()
{
}

void FScene::Release()
{
    GetRendererModule().RemoveScene(this);
}