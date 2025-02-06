#pragma once

// RHI 명령 컨텍스트 인터페이스. 때로는 RHI가 이를 처리합니다.
// 명령 리스트를 병렬로 처리할 수 있는 플랫폼에서는 별도의 객체입니다.
class IRHICommandContext
{
public:
	virtual ~IRHICommandContext()
	{
	}
};