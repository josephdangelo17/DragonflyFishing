#include "Waterline.h"
#include "DisplayManager.h"
#include "WorldManager.h"

Waterline::Waterline() {

	setType("Waterline");

	setSolidness(df::SPECTRAL);

}

int Waterline::draw() {

	int world_x = (int)WM.getBoundary().getHorizontal();

	for (int i = 0; i < world_x; i++) {
		DM.drawCh(df::Vector(i, 3), WATERLINE_CHAR, df::BLUE);
	}

	return 0;

}