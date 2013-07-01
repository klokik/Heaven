#include <iostream>

#include "world.hpp"

using namespace std;
using namespace heaven;

int main(void)
{
	cout<<"Hello!"<<endl;

	World world;

	world.init();
	world.run();

	cout<<"Good luck!"<<endl;
	return 0;
}