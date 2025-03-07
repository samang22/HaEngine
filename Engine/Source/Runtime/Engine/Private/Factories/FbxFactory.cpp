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
			const FString FbxNodeName = ANSI_TO_TCHAR(InNode->GetName());
			E_LOG(Log, TEXT("ExtractFbx Mesh: {}"), FbxNodeName);

			fbxsdk::FbxMesh* Mesh = static_cast<fbxsdk::FbxMesh*>(NodeAttribute); _ASSERT(Mesh);
			fbxsdk::FbxLayer* BaseLayer = Mesh->GetLayer(0);
			if (BaseLayer == nullptr)
			{
				E_LOG(Error, TEXT("BaseLayer is nullptr!"));
				return;
			}

			bool bSuccessed = true;

			// 정점(Vertex): Mesh를 구성하는 점(GetControlPoints)
			// 폴리곤(Polygon): 점들이 모여서 만들어진 도형(보통 삼각형)
			// GetPolygonSize: 폴리곤 하나의 크기 (삼격형이면 3개; 우리는 삼각형만 지원)
			// 서로 다른 Polygon에 Vertex가 중첩 될 수 있다. 따라서 Normal도 하나의 Vertex에 여럿 존재할 수 있고 이는 Mode에 따라 달라질 수 있다

			const int32 PolygonCount = Mesh->GetPolygonCount();
			const int32 VertexCount = Mesh->GetControlPointsCount();

			// Vertices 추출
			TArray<FPositionNormal> Vertices; 
			{
				const uint32 VertexCount = Mesh->GetControlPointsCount();
				fbxsdk::FbxVector4* FbxVertices = Mesh->GetControlPoints();

				Vertices.resize(VertexCount);

				// Position 정보 추출
				for (uint32 i = 0; i < VertexCount; ++i)
				{
					// UE 축에 맞게 변환
					Vertices[i].Position = FVector3D(FbxVertices[i][0], -FbxVertices[i][1], FbxVertices[i][2]);
				}

                // Normal이 없는 경우 생성한다
                if (BaseLayer->GetNormals() == nullptr)
                {
                    Mesh->InitNormals();
                    Mesh->GenerateNormals();
                }

                // UE에서 FbxStaticMeshImport.cpp 참조
                // 
                // Normal 정보 추출
                fbxsdk::FbxGeometryElementNormal* LayerElementNormal = Mesh->GetElementNormal();
                fbxsdk::FbxLayerElement::EMappingMode NormalMappingMode = LayerElementNormal->GetMappingMode();
                fbxsdk::FbxLayerElement::EReferenceMode NormalReferenceMode = LayerElementNormal->GetReferenceMode();

                int32 CurrentVertexInstanceIndex = 0;
                for (int32 PolygonIndex = 0; PolygonIndex < PolygonCount; ++PolygonIndex)
                {
                    const int32 PolygonVertexCount = Mesh->GetPolygonSize(PolygonIndex);
                    if (PolygonVertexCount != 3)
                    {
                        E_LOG(Warning, TEXT("PolygonVertexCount({}) != 3"), PolygonVertexCount);
                        return;
                    }

                    for (int32 CornerIndex = 0; CornerIndex < PolygonVertexCount; ++CornerIndex)
                    {
                        const int32 ControlPointIndex = Mesh->GetPolygonVertex(PolygonIndex, CornerIndex);
                        int32 RealFbxVertexIndex = /*SkippedVertexInstance +*/ CurrentVertexInstanceIndex + CornerIndex;
                        // 노멀은 탄젠트와 바이노멀과 다른 참조 및 매핑 모드를 가질 수 있습니다.
                        int32 NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ?
                            ControlPointIndex : RealFbxVertexIndex;
                        int32 NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ?
                            NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);

                        FbxVector4 TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
                        Vertices[ControlPointIndex].Normal = FVector3D(TempValue[0], -TempValue[1], TempValue[2]);
                    }
                    CurrentVertexInstanceIndex += PolygonVertexCount;
                }

			}

			TArray<uint32> Indices;
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
				NewMeshData.Name = FbxNodeName;
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
