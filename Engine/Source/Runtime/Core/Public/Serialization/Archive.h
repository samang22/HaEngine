#pragma once
#include "CoreTypes.h"
#include <boost/serialization/nvp.hpp>
namespace boost {
    namespace serialization { template<class T> class nvp; }
    namespace archive
    {
        class text_oarchive;
        class text_iarchive;
    }
}

class UObject;
class CORE_API FArchive
{
public:
    enum class EMode { Save, Load };

    FArchive(boost::archive::text_oarchive& OutputArchive);
    FArchive(boost::archive::text_iarchive& InputArchive);

    FArchive& operator<<(uint64& InOutData);
    FArchive& operator<<(int32& InOutData);
    FArchive& operator<<(FString& InString);
    FArchive& operator<<(string& InString);

    inline bool IsSaving() const { return Mode == EMode::Save; }
    inline bool IsLoading() const { return Mode == EMode::Load; }

private:
    EMode Mode;
    boost::archive::text_oarchive* SaveArchive = nullptr;
    boost::archive::text_iarchive* LoadArchive = nullptr;
};