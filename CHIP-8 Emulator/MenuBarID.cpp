#include "MenuBarID.h"

bool MenuBarID::isSpeedID(int id)
{
	return (id == ID::speed_250 ||
			id == ID::speed_500 ||
			id == ID::speed_750 ||
			id == ID::speed_1000 ||
			id == ID::speed_1250 ||
			id == ID::speed_1500);
}


bool MenuBarID::isSizeID(int id)
{
	return (id == ID::size_5x ||
			id == ID::size_10x ||
			id == ID::size_15x ||
			id == ID::size_20x ||
			id == ID::size_30x ||
			id == ID::size_40x);
}