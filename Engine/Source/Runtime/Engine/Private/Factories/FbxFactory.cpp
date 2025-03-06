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
    struct FFbxManagerContext
    {
        FFbxManagerContext(fbxsdk::FbxManager* InFbxManager)
            : Manager(InFbxManager) { }

        ~FFbxManagerContext()
        {
            _ASSERT(Manager);
            Manager->Destroy();
            Manager = nullptr;
        }

        fbxsdk::FbxManager* Manager = nullptr;
    };
    FFbxManagerContext FbxManagerContext(fbxsdk::FbxManager::Create());

    fbxsdk::FbxIOSettings* SdkIOSettings = fbxsdk::FbxIOSettings::Create(FbxManagerContext.Manager, IOSROOT);
    FbxManagerContext.Manager->SetIOSettings(SdkIOSettings);

    fbxsdk::FbxScene* Scene = LoadFbxScene(FbxManagerContext.Manager, TCHAR_TO_ANSI(InFileName).c_str());
    if (!Scene)
    {
        E_LOG(Error, TEXT("Failed to create Scene."));
        return nullptr;
    }

    TObjectPtr<UStaticMesh> NewStaticMesh;
    // Mesh 정보를 얻어온다
    {
        TArray<FMeshData> MeshData;
        ExtractFbx(Scene->GetRootNode(), MeshData);
        Scene->Destroy();

		NewStaticMesh = NewObject<UStaticMesh>(nullptr, UStaticMesh::StaticClass(), InName);
		NewStaticMesh->Create(MeshData);
	}
    return NewStaticMesh;
}

fbxsdk::FbxScene* UFbxFactory::LoadFbxScene(fbxsdk::FbxManager* InFbxManager, const char* InFileName)
{
    struct FFbxImporterContext
    {
        FFbxImporterContext(fbxsdk::FbxImporter* InFbxImporter)
            : Importer(InFbxImporter) {
        }

        ~FFbxImporterContext()
        {
            _ASSERT(Importer);
            Importer->Destroy();
            Importer = nullptr;
        }

        fbxsdk::FbxImporter* Importer = nullptr;
    };
    FFbxImporterContext Context(fbxsdk::FbxImporter::Create(InFbxManager, "FBX Importer"));

    if (!Context.Importer->Initialize(InFileName, -1, InFbxManager->GetIOSettings()))
    {
        E_LOG(Error, TEXT("UFbxFactory::LoadFbxScene Importer->Initialize failed: {}"), ANSI_TO_TCHAR(Context.Importer->GetStatus().GetErrorString()));
        return nullptr;
    }


    fbxsdk::FbxScene* Scene = fbxsdk::FbxScene::Create(InFbxManager, "FBX Scene");
    if (!Context.Importer->Import(Scene))
    {
        E_LOG(Error, TEXT("UFbxFactory::LoadFbxScene Importer->Import failed: {}"), ANSI_TO_TCHAR(Context.Importer->GetStatus().GetErrorString()));

        Scene->Destroy();
        return nullptr;
    }
    return Scene;
}

void UFbxFactory::ExtractFbx(fbxsdk::FbxNode* InNode, TArray<FMeshData>& OutMeshData)
{
	fbxsdk::FbxNodeAttribute* NodeAttribute = InNode->GetNodeAttribute();
	if (NodeAttribute != nullptr)
	{
		fbxsdk::FbxNodeAttribute::EType AttributeType = NodeAttribute->GetAttributeType();

		if (AttributeType == fbxsdk::FbxNodeAttribute::eMesh)
		{
			fbxsdk::FbxMesh* Mesh = static_cast<fbxsdk::FbxMesh*>(NodeAttribute);
			_ASSERT(Mesh);
			bool bSuccessed = true;

			// Vertices 추출
			TArray<FVector3D> Vertices;
			{
				const uint32 VertexCount = Mesh->GetControlPointsCount();
				fbxsdk::FbxVector4* FbxVertices = Mesh->GetControlPoints();

				Vertices.reserve(VertexCount);

				for (uint32 i = 0; i < VertexCount; ++i)
				{
					// UE 축에 맞게 변환
					Vertices.emplace_back(FbxVertices[i][0], -FbxVertices[i][1], FbxVertices[i][2]);
				}
			}

			TArray<uint32> Indices;
            const int32 PolygonCount = Mesh->GetPolygonCount();
            // Indices 추출
			{
				// 삼각형의 갯수
				for (int32 i = 0; i < PolygonCount; ++i)
				{
					const int32 PolygonSize = Mesh->GetPolygonSize(i);
					if (PolygonSize == -1)
					{
						E_LOG(Error, TEXT("ExtractFbx PolygonSize Error! (-1)"));
						bSuccessed = false;
						break;
					}
					else if (PolygonSize != 3)
					{
						E_LOG(Warning, TEXT("ExtractFbx not supported PolygonSize: {}"), PolygonSize);
						bSuccessed = false;
						break;
					}

					// 일반적으로 3번 돌겠죠? 삼각형 이니까
					for (int32 j = 0; j < PolygonSize; ++j)
					{
						const int32 Index = Mesh->GetPolygonVertex(i, j);
						if (Index == -1)
						{
							E_LOG(Error, TEXT("ExtractFbx Index Error! (-1)"));
							bSuccessed = false;
							break;
						}
						Indices.push_back(Index);
					}
				}
			}

			if (bSuccessed)
			{
				FMeshData& NewMeshData = OutMeshData.emplace_back();
				NewMeshData.Vertices = move(Vertices);
				NewMeshData.Indices = move(Indices);
                NewMeshData.NumPrimitives = PolygonCount;
                // Material 생성
				{
					const int MaterialCount = Mesh->GetElementMaterialCount();
				}
			}
		}
	}

	for (uint32 i = 0; i < InNode->GetChildCount(); ++i)
	{
		ExtractFbx(InNode->GetChild(i), OutMeshData);
	}
}
