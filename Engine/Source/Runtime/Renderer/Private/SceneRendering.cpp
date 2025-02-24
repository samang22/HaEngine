#include "RendererModule.h"
#include "ScenePrivate.h"

FRendererModule::FRendererModule()
{
}

void FRendererModule::StartupModule()
{
    IRendererModule::StartupModule();
}

void FRendererModule::ShutdownModule()
{
    IRendererModule::ShutdownModule();
}

FSceneInterface* FRendererModule::AllocateScene(UWorld* World, ERHIFeatureLevel::Type InFeatureLevel)
{
    FScene* NewScene = new FScene(World, InFeatureLevel);
    AllocatedScenes.insert(NewScene);
    return NewScene;
}

void FRendererModule::RemoveScene(FSceneInterface* Scene)
{
    AllocatedScenes.erase(Scene);
    delete Scene;
}
