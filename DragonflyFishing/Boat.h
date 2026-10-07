#pragma once

#include "object.h"

class Boat : public df::Object {

public:

	Boat();
	int draw(void) override;

};