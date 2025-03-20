#include "Engine/EngineBaseTypes.h"


//
// Convert this URL to text.
//
FString FURL::ToString(bool FullyQualified) const
{
    FString Result;

    // Emit protocol.
    /*if ((Protocol != UrlConfig.DefaultProtocol) || FullyQualified)
    {
        Result += Protocol;
        Result += TEXT(":");

        if (Host != UrlConfig.DefaultHost)
        {
            Result += TEXT("//");
        }
    }*/

    // Emit host and port
    //if ((Host != UrlConfig.DefaultHost) || (Port != UrlConfig.DefaultPort))
    {
        if (!Host.empty())
        {
            Result += Host;
        }
        else
        {
            Result += TEXT("0.0.0.0");
        }
        Result += TEXT(":") + std::to_wstring(Port);
        //Result += GetHostPortString();

        //if (!Map.StartsWith(TEXT("/")) && !Map.StartsWith(TEXT("\\")))
        //{
        //    Result += TEXT("/");
        //}
    }

    // Emit map.
    //if (Map.Len() > 0)
    //{
    //    Result += Map;
    //}

    // Emit options.
    //for (int32 i = 0; i < Op.Num(); i++)
    //{
    //    Result += TEXT("?");
    //    Result += Op[i];
    //}

    // Emit portal.
    //if (Portal.Len() > 0)
    //{
    //    Result += TEXT("#");
    //    Result += Portal;
    //}

    return Result;
}