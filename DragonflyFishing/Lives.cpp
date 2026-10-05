#include "Lives.h"
#include "Event.h"
#include "EventStep.h"
#include "LogManager.h"

Lives::Lives() {

	// start with 3 lives
	m_lives = 3;

	// display
	setLocation(df::TOP_RIGHT);
	setViewString(LIVES_STRING);
	setColor(df::MAGENTA);

	setValue(m_lives);

}

// Handle event
int Lives::eventHandler(const df::Event* p_e) {

	// parent handles event if health update
	if (df::ViewObject::eventHandler(p_e)) {

		return 1;
	}

	return 0;

}