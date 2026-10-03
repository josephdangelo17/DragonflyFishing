//
// Junk.h - A tin can that drifts left to right across the lake.
// Catching one on the hook costs the player a life.
//
// Difficulty ramp: every 2nd Junk that drifts off the right edge
// spawns one additional Junk (up to JUNK_MAX_COUNT).
//
#pragma once

#include "Object.h"
#include "Event.h"

class Junk : public df::Object {

private:
	//Put junk just off the left edge at a random depth with a random speed.
	void moveToStart();

	//Called when junk drifts off the right side of the screen.
	void out();

public:
	Junk();
	~Junk();

	//Handle out-of-bounds and hook collisions
	int eventHandler(const df::Event* p_e) override;
};
