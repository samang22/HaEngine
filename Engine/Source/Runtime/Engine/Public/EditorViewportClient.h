#pragma once
#include "ViewportClient.h"
#include "EditorViewportClient.generated.h"

// Unreal 에서는 FViewport 에서 다양한 ViewportClient(Ex. UGameViewportClient, FEditorViewportClient 등)를 대상으로
// 입력 정보와 렌더링 요청을 전달하고 있는 것으로 보임.
// 이를 간소화 하여 UViewportClient를 만들었고, 여기에 직접적으로 명령을 전달할 예정입니다.
UCLASS()
class UEditorViewportClient : public UViewportClient
{
	GENERATED_BODY()

};