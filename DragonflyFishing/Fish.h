//
// Fish.h - A fish that swims left to right across the lake.
//
#pragma once

#include "Object.h"
#include "Event.h"

class Fish : public df::Object {

private:
	// Put fish just off the left edge at a random depth with a random speed.
	void moveToStart();

	// Called when fish swims off the right side of the screen.
	void out();

public:
	Fish();

	// Handle out-of-bounds (and, later, hook collisions).
	int eventHandler(const df::Event* p_e) override;
};