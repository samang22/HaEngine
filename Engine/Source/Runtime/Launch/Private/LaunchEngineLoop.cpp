#include "LaunchEngineLoop.h"

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	return 0;
}

int32 FEngineLoop::Init()
{
	return 0;
}
#include <iostream>
void FEngineLoop::Tick()
{
	cout << "Tick\n";
}
