#include "Engine/Engine.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, Engine);

UEngine::UEngine()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }
	E_LOG(Warning, TEXT("{} Hello"), GetName().ToString());
}