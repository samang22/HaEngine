#pragma once
#include "Factory.h"
#include "FbxFactory.generated.h"


namespace fbxsdk
{
    class FbxScene;
    class FbxManager;
    class FbxNode;
}

/**
 * 모든 팩토리의 기본 클래스
 * 새로운 객체를 생성하고 가져오는 객체.
 *
 */
UCLASS()
class UFbxFactory : public UFactory
{
    GENERATED_BODY()

public:
    virtual bool FactoryCanImport(const FString& Filename) override;
    virtual TObjectPtr<UObject> FactoryCreateFile(const FName InName, const FString& InFileName, const TCHAR* Params) override;


protected:
    fbxsdk::FbxScene* LoadFbxScene(fbxsdk::FbxManager* InFbxManager, const char* InFileName);

    struct FMeshData
    {
        TArray<TArray<FVector3D>> Vertices;
        TArray<TArray<uint32>> Indices;
    };
    void ExtractFbx(fbxsdk::FbxNode* InNode, FMeshData& OutMeshData);
};                  