#pragma once
#include "Factory.h"
#include "FbxFactory.generated.h"


namespace fbxsdk
{
    class FbxScene;
    class FbxManager;
    class FbxNode;
}

struct FMeshData
{
    TArray<FVector3D> Vertices;
    TArray<uint32> Indices;
};


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

    void ExtractFbx(fbxsdk::FbxNode* InNode, TArray<FMeshData>& OutMeshData);
};                  