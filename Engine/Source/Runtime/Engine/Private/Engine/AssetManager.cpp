#include "Engine/AssetManager.h"
#include "Engine/Engine.h"

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

    std::filesystem::path FilePath(InFilePath);
    const FString Extension = FilePath.extension().wstring().erase(0, 1); // remove .

    return nullptr;
}