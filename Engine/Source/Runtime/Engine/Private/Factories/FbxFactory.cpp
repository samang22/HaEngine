#include "Factories/FbxFactory.h"
#include "Engine/StaticMesh.h"
#include "Fbx.h"
bool UFbxFactory::FactoryCanImport(const FString& Filename)
{
    FString Extension = FPaths::GetExtension(Filename);
    std::transform(Extension.begin(), Extension.end(), Extension.begin(), towlower);
    if (Extension == TEXT("fbx") /*|| Extension == TEXT("obj")*/)
    {
        return true;
    }

    return false;
}

TObjectPtr<UObject> UFbxFactory::FactoryCreateFile(const FName InName, const FString& InFileName, const TCHAR* Params)
{
    //struct FFbxManagerContext
    //{
    //    FFbxManagerContext(fbxsdk::FbxManager* InFbxManager)
    //        : Manager(InFbxManager) { }

    //    ~FFbxManagerContext()
    //    {
    //        _ASSERT(Manager);
    //        Manager->Destroy();
    //        Manager = nullptr;
    //    }

    //    fbxsdk::FbxManager* Manager = nullptr;
    //};
    //FFbxManagerContext FbxManagerContext(fbxsdk::FbxManager::Create());

    //fbxsdk::FbxIOSettings* SdkIOSettings = fbxsdk::FbxIOSettings::Create(FbxManagerContext.Manager, IOSROOT);
    //FbxManagerContext.Manager->SetIOSettings(SdkIOSettings);

    TObjectPtr<UStaticMesh> NewStaticMesh;

    return NewStaticMesh;
}
