#pragma once
#include "CoreMinimal.h"

#pragma pack(push, 1)
class FPacketHeader
{
public:
    int32 PacketSize = 0;
};

//
// 채널에서 수신된 데이터 묶음.
//
class FInBunch : public FPacketHeader /*: public FNetBitReader*/ 
{
public:
};
#pragma pack(pop)