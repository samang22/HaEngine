#include "ScenePrivate.h"
#include "EngineModule.h"
#include "RendererInterface.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "PrimitiveSceneProxy.h"

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

void FScene::AddPrimitive(UPrimitiveComponent* Primitive)
{
    FPrimitiveSceneProxy* NewProxy = Primitive->CreateSceneProxy();
    if (!NewProxy) { return; }

    Proxies.push_back(NewProxy);
}

void FScene::RemovePrimitive(UPrimitiveComponent* Primitive)
{
    auto It = find_if(Proxies.begin(), Proxies.end(),
        [Primitive](FPrimitiveSceneProxy* Proxy)
        {
            return Proxy->GetPrimitiveComponent() == Primitive;
        }
    );

    if (It == Proxies.end())
    {
        E_LOG(Error, TEXT("Check"));
        return;
    }

    Proxies.erase(It);
}

void FScene::Release()
{
    for (FPrimitiveSceneProxy* Proxy : Proxies)
    {
        delete Proxy;
    }
    Proxies.clear();

    GetRendererModule().RemoveScene(this);
}