#pragma once

#include "Object.h"

#define WATERLINE_CHAR '='

class Waterline : public df::Object {

public:

	Waterline();
	int draw(void) override;

};