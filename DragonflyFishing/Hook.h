#pragma once

#include "Object.h"
#include "EventMouse.h"

class Hook : public df::Object {

private:
	void mouse(const df::EventMouse* p_mouse_event);
	void move(int dy);
	void step();
	int lives;

public:
	Hook();
	~Hook();
	int eventHandler(const df::Event* p_e) override;
	void damage();
};