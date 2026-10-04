//
// Hook.h - Generic fishing hook, drawn as a "J".
//
#pragma once


#include "Object.h"
#include "Event.h"
#include "EventMouse.h"
#include "EventCollision.h"

class Hook : public df::Object {

private:
	int  m_lives;                //hits left before game over
	bool m_game_over;            //true once out of lives
	int  m_game_over_countdown;  //steps to show "GAME OVER" before exiting

	void mouse(const df::EventMouse* p_e);
	void collide(const df::EventCollision* p_e);
	void step();
	void gameOver();

public:
	Hook();

	int eventHandler(const df::Event* p_e) override;
	int draw() override;

	int getLives() const;
};
