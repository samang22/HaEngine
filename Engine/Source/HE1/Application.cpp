#include "CoreTypes.h"
#include "Modules/ModuleManager.h"
#include <iostream>

IMPLEMENT_MODULE(FDefaultModuleImpl, HE1);

int main()
{
	std::cout << "Hello World" << std::endl;

	return 0;
}