#pragma once

/** UWorld 객체의 goal/source를 지정합니다 */
namespace EWorldType
{
    enum Type
    {
        /** 유형이 지정되지 않은 월드, 대부분의 경우 스트리밍된 서브 레벨의 잔여 월드가 됩니다 */
        None,

        /** 게임 월드 */
        Game,

        /** 에디터에서 편집 중인 월드 */
        Editor,

        /** 에디터 내 플레이(PIE) 월드 */
        PIE,

        /** 에디터 도구용 프리뷰 월드 */
        EditorPreview,

        /** 게임용 프리뷰 월드 */
        GamePreview,

        /** 게임용 최소 RPC 월드 */
        GameRPC,

        /** 레벨 에디터에서 현재 편집 중이 아닌 로드된 에디터 월드 */
        Inactive
    };
}
