#include <iostream>

#include "HeavenWorld.hpp"
#include "AEDebug.h"

using namespace std;
using namespace heaven;

int main(void)
{
	AEPrintLog("Hello!");

	HeavenWorld world;

	if(!world.init())
		return 0;

	world.run();

	aengine::AEPrintLog("Good luck!");
	return 0;
}