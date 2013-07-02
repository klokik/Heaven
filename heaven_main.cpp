#include <iostream>

#include "HeavenWorld.hpp"

using namespace std;
using namespace heaven;

int main(void)
{
	cout<<"Hello!"<<endl;

	HeavenWorld world;

	world.init();
	world.run();

	cout<<"Good luck!"<<endl;
	return 0;
}