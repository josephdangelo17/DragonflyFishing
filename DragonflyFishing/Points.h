#pragma once

#include "Event.h"
#include "ViewObject.h"
#include "EventStep.h"

#define POINTS_STRING "Points"

class Points : public df::ViewObject {

private:
	int m_point;

public:
	Points();
	int eventHandler(const df::Event* p_e) override;
};