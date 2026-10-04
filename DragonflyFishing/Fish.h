//
// Fish.h - A fish that swims left to right across the lake.
//
#pragma once

#include "Object.h"
#include "Event.h"

class Fish : public df::Object {

private:
	bool m_caught = false;  //true once caught (so we only react once)

	//Put fish just off the left edge at a random depth with a random speed.
	void moveToStart();

	//Called when fish swims off the right side of the screen.
	void out();

public:
	Fish();

	//Called by the Hook when it hits this fish.
	void caught();

	//Handle out-of-bounds and collision
	int eventHandler(const df::Event* p_e) override;
};