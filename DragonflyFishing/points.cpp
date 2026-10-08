#include "Points.h"

Points::Points() {

	setType("Points");
	setLocation(df::TOP_LEFT);
	setViewString(POINTS_STRING);
	setColor(df::MAGENTA);

}

int Points::eventHandler(const df::Event* p_e) {

	if (df::ViewObject::eventHandler(p_e)) {
		return 1;
	}

}