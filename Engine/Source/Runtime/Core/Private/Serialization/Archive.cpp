#include "Serialization/Archive.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Class.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Math/Rotator.h"

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

FArchive& FArchive::operator<<(bool& InOutData)
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

FArchive& FArchive::operator<<(float& InOutData)
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

FArchive& FArchive::operator<<(FRotator& InOutData)
{
    if (IsSaving())
    {
        (*SaveArchive)& InOutData.Pitch;
        (*SaveArchive)& InOutData.Yaw;
        (*SaveArchive)& InOutData.Roll;
    }
    else
    {
        (*LoadArchive)& InOutData.Pitch;
        (*LoadArchive)& InOutData.Yaw;
        (*LoadArchive)& InOutData.Roll;
    }
    return *this;
}

FArchive& FArchive::operator<<(FVector& InOutData)
{
    if (IsSaving())
    {
        (*SaveArchive)& InOutData.x;
        (*SaveArchive)& InOutData.y;
        (*SaveArchive)& InOutData.z;
    }
    else
    {
        (*LoadArchive)& InOutData.x;
        (*LoadArchive)& InOutData.y;
        (*LoadArchive)& InOutData.z;
    }
    return *this;
}

FArchive& FArchive::operator<<(TEnginePtr<UObject>& InOutData)
{
    FString ClassName, ObjectName;

    if (IsSaving())
    {
        ClassName = InOutData->GetClass()->ClassName;
        ObjectName = InOutData->GetName();
    }

    *this << ClassName;
    *this << ObjectName;

    if (IsLoading())
    {
        InOutData = FindObject(ClassName, ObjectName);
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