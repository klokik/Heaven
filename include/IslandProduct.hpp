#ifndef ISLANDPRODUCT_H_
#define ISLANDPRODUCT_H_

namespace heaven
{
	class Ship;

	struct IslandProduct
	{
		enum Type {NONE=0,FOOD=1,IRON=2,SHIP=3};

		Type prod_type;
		float amount;
		Ship *ship;
	};
}

#endif /* ISLANDPRODUCT_H_ */