#pragma once

#include "object.h"

class Seaweed : public df::Object {

private:
	int randval;

public:

	Seaweed();
	int draw(void) override;

};