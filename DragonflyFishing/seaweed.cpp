#include "Seaweed.h"
#include "WorldManager.h"

Seaweed::Seaweed() {

	setType("Seaweed");

	setSprite("seaweed");

	setAltitude(1);

	setSolidness(df::SPECTRAL);

	randval = rand() % (72 - 10 + 1) + 10;

}

int Seaweed::draw() {

	int seafloor = WM.getBoundary().getVertical();

	setPosition(df::Vector(randval, seafloor - 2));
	df::Object::draw();

	return 0;

}