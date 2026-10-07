//
// Hook.h - Generic fishing hook, drawn as a "J".
//
#pragma once


#include "Object.h"
#include "Event.h"
#include "EventMouse.h"
#include "EventCollision.h"
#include "EventKeyboard.h"
#include "Fish.h"

class Hook : public df::Object {

private:
	int  m_lives;                //hits left before game over
	bool m_game_over;            //true once out of lives (message showing)
	bool m_leaving;              //true once we're returning to the title
	bool m_done;                 //true once cleanup has run (only do it once)
	int  m_game_over_countdown;  //steps to show "GAME OVER" before leaving
	Fish* p_fish;				//pointer to keep track of caught fish
	bool m_fish_held;			//true if holding a fish

	void mouse(const df::EventMouse* p_e);
	void keyboard(const df::EventKeyboard* p_e);
	void collide(const df::EventCollision* p_e);
	void step();
	void gameOver();

	//Remove all fish, junk and this hook, then show the title screen.
	void returnToTitle();

public:
	Hook();

	int eventHandler(const df::Event* p_e) override;
	int draw() override;

	int getLives() const;
	void fishCaught();
};