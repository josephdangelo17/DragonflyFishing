#pragma once

#include "Event.h"
#include "ViewObject.h"

#define LIVES_STRING "Lives"

class Lives : public df::ViewObject {

private:

	int m_lives;

public:

	Lives();
	int eventHandler(const df::Event* p_e) override;

};