#include "Serialization/Archive.h"
#include "UObject/UObjectGlobals.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

FArchive::FArchive(boost::archive::text_oarchive& OutputArchive)
    : Mode(EMode::Save), SaveArchive(&OutputArchive)
{
}

FArchive::FArchive(boost::archive::text_iarchive& InputArchive)
    : Mode(EMode::Load), LoadArchive(&InputArchive)
{
}

FArchive& FArchive::operator<<(uint64& InOutData)
{
    if (IsSaving())
    {
        (*SaveArchive)& InOutData;
    }
    else
    {
        (*LoadArchive)& InOutData;
    }
    return *this;
}

FArchive& FArchive::operator<<(int32& InOutData)
{
    if (IsSaving())
    {
        (*SaveArchive)& InOutData;
    }
    else
    {
        (*LoadArchive)& InOutData;
    }
    return *this;
}

FArchive& FArchive::operator<<(FString& InString)
{
    string String;
    if (IsSaving())
    {
        String = TCHAR_TO_ANSI(InString);
        (*SaveArchive)& String;
    }
    else
    {
        (*LoadArchive)& String;
        InString = ANSI_TO_TCHAR(String);
    }
    return *this;
}

FArchive& FArchive::operator<<(string& InString)
{
    if (IsSaving())
    {
        (*SaveArchive)& InString;
    }
    else
    {
        (*LoadArchive)& InString;
    }
    return *this;
}