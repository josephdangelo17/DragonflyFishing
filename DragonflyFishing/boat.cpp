#include "Boat.h"

Boat::Boat() {

	setType("Boat");

	setSprite("boat");

	setAltitude(4);

	setPosition(df::Vector(40, 2));

	setSolidness(df::SPECTRAL);

}

int Boat::draw() {

	return df::Object::draw();


}