#include "Hook.h"

Hook::Hook() {

	setSprite("hook");

	setType("Hook");

	setSolidness(df::SOFT);

	setPosition(df::Vector(50, 20));

	registerInterest(df::MSE_EVENT);

	lives = 3;

}

Hook::~Hook() {

	//gameover

	lives = 0;

}

int Hook::eventHandler(const df::Event* p_e) {

	if (p_e->getType() == df::MSE_EVENT) {
		const df::EventMouse* p_mouse_event =
			dynamic_cast <const df::EventMouse*> (p_e);

		if (p_mouse_event != nullptr) {

			df::Vector mouse_pos = p_mouse_event->getMousePosition();

			df::Vector hook_pos = getPosition();

			hook_pos.setY(mouse_pos.getY());

			setPosition(hook_pos);

		}

		return 1;
	}

	return 0;

}

void Hook::damage() {

	return;

}