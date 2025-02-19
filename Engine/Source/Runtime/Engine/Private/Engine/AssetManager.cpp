#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Factories/Factory.h"

FAssetManager* FAssetManager::Get(const bool bDestroy)
{
    static unique_ptr<FAssetManager> Instance = make_unique<FAssetManager>();
    if (bDestroy)
    {
        Instance.reset();
        return nullptr;
    }

    return Instance.get();
}

UObject* FAssetManager::LoadAsset(const type_info& InAssetType, const FString& InFilePath)
{
    if (LoadedAssets[InAssetType.hash_code()].contains(InFilePath))
    {
        return LoadedAssets[InAssetType.hash_code()][InFilePath].get();
    }

    const bool bFileExist = std::filesystem::exists(InFilePath);
    if (!bFileExist)
    {
        E_LOG(Error, TEXT("Can not find file path: {}"), InFilePath)
            return nullptr;
    }

    //std::filesystem::path FilePath(InFilePath);
    //const FString Extension = FilePath.extension().wstring().erase(0, 1); // remove .

    TArray<UClass*> FactoryClasses = UClass::GetAllSubclassOfClass(UFactory::StaticClass());
    TObjectPtr<UFactory> NewFactory;
    for (UClass* Class : FactoryClasses)
    {
        UFactory* Factory = Class->GetDefaultObject<UFactory>();
        if (Factory == nullptr)
        {
            E_LOG(Error, TEXT("CDO 로딩 중"));
            return nullptr;
        }

        if (Factory->FactoryCanImport(InFilePath))
        {
            NewFactory = NewObject<UFactory>(nullptr, Factory->GetClass());
            break;
        }
    }

    if (NewFactory)
    {
        TObjectPtr<UObject> NewAsset = NewFactory->FactoryCreateFile(InFilePath, InFilePath, nullptr);
        LoadedAssets[InAssetType.hash_code()][InFilePath] = NewAsset;
        return NewAsset.get();
    }

    return NewFactory.get();
}