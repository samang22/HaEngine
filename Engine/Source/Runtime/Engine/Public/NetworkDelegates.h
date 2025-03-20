#pragma once
#include "CoreMinimal.h"

/**
* 연결 수락 응답 코드
*/
namespace EAcceptConnection
{
    enum Type
    {
        /** 연결 거부 */
        Reject,
        /** 연결 수락 */
        Accept,
        /** 서버 이동 중에 응답을 보내지 않고 연결을 무시 */
        Ignore
    };

    /** @return 전달된 열거형의 문자열 버전 */
    inline const TCHAR* ToString(EAcceptConnection::Type EnumVal)
    {
        switch (EnumVal)
        {
        case Reject:
        {
            return TEXT("Reject");
        }
        case Accept:
        {
            return TEXT("Accept");
        }
        case Ignore:
        {
            return TEXT("Ignore");
        }
        }
        return TEXT("");
    }
};

/**
 * 네트워크 코드는 이 클래스를 사용하여 알림을 전송합니다.
 */
class FNetworkNotify
{
public:
    /**
     * 들어오는 연결이 대기 중임을 알리고, 인터페이스가 요청을 거부할 기회를 제공합니다.
     *
     * @return EAcceptConnection 현재 시점에서 연결을 수락할 의사가 있는지를 나타냅니다.
     */
    virtual EAcceptConnection::Type NotifyAcceptingConnection() { _ASSERT(false); return EAcceptConnection::Ignore; }

    /**
     * 원격 요청의 결과로 새로운 연결이 생성/설정되었음을 알립니다. 이는 이전에 NotifyAcceptingConnection에 의해 승인된 요청입니다.
     *
     * @param Connection 새로 생성된 연결
     */
    virtual void NotifyAcceptedConnection(class UNetConnection* Connection) { _ASSERT(false); }

    /**
     * 원격 요청(Actor 생성 등)의 결과로 새로운 채널이 생성/열림을 알립니다.
     *
     * @param Channel 새로 생성된 채널
     *
     * @return 채널을 열어야 하면 true, 채널을 거부(채널 파괴)해야 하면 false를 반환합니다.
     */
    virtual bool NotifyAcceptingChannel(class UChannel* Channel) { _ASSERT(false); return false; }

    /**
     * 원격 연결의 제어 채널을 통해 전송된 메시지에 대한 핸들러입니다.
     * 메시지를 처리할 필요는 없지만, Bunch에서 데이터를 읽은 경우 해당 메시지의 전체 데이터 스트림을 읽어야 합니다 (즉, FNetControlMessage<TYPE>::Receive()를 사용).
     */
    virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch) { _ASSERT(false); }
};