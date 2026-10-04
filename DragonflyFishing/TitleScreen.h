//
// TitleScreen.h - Title screen with instructions.
//
#pragma once

#include "Object.h"
#include "Event.h"

class TitleScreen : public df::Object {

private:
	int m_steps;  //step counter, used to blink the "press SPACE" prompt

	//Create the game objects and remove the title screen.
	void start();

public:
	TitleScreen();

	int eventHandler(const df::Event* p_e) override;
	int draw() override;
};
