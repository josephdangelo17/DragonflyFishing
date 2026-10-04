//
// Junk.h - A tin can that drifts left to right across the lake.
// Catching one on the hook costs the player a life.
//
#pragma once

#include "Object.h"
#include "Event.h"

class Junk : public df::Object {

private:
	bool m_hit = false;  //true once hit (so we only react once)

	//Put junk just off the left edge at a random depth with a random speed.
	void moveToStart();

	//Called when junk drifts off the right side of the screen.
	void out();

public:
	Junk();
	~Junk();

	//Called by the Hook when it hits this junk
	bool hit();

	//Handle out-of-bounds and collision
	int eventHandler(const df::Event* p_e) override;
};