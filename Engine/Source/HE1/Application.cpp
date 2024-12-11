#include "CoreTypes.h"
#include "Modules/ModuleManager.h"
#include <iostream>

IMPLEMENT_MODULE(FDefaultModuleImpl, HE1);




extern int32 LAUNCH_API GuardedMain(const TCHAR* CmdLine);

int main()
{
	return GuardedMain(nullptr);
}