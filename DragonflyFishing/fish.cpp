#include <stdlib.h>

#include "Fish.h"
#include "EventOut.h"
#include "EventCollision.h"
#include "LogManager.h"
#include "WorldManager.h"

// Tuning values (adjust to match your waterline / HUD layout).
const int   FISH_WATER_TOP = 6;     // first row below the waterline
const int   FISH_BOTTOM_PAD = 2;     // keep fish off the very bottom row
const float FISH_MIN_SPEED = 0.20f; // columns per game step
const float FISH_MAX_SPEED = 0.60f;

Fish::Fish() {

    // Sprite is loaded in game.cpp with label "fish".
    // Animation plays automatically using the slowdown in the sprite file.
    if (setSprite("fish") != 0)
        LM.writeLog("Fish::Fish(): Warning! Sprite 'fish' not found");

    setType("Fish");

    // SOFT: generates collision events (needed for the hook) but fish
    // pass through each other instead of bouncing/blocking.
    setSolidness(df::SOFT);
    setAltitude(2);

    moveToStart();
}

void Fish::moveToStart() {

    int world_vert = (int)WM.getBoundary().getVertical();

    // Start a random distance off the left edge so fish are staggered.
    float x = -(float)(getBox().getHorizontal() + rand() % 30);

    // Random depth between waterline and bottom.
    int range = world_vert - FISH_WATER_TOP - FISH_BOTTOM_PAD;
    if (range < 1) range = 1;
    float y = (float)(FISH_WATER_TOP + rand() % range);

    WM.moveObject(this, df::Vector(x, y));

    // Random speed, always moving right.
    float speed = FISH_MIN_SPEED +
        (FISH_MAX_SPEED - FISH_MIN_SPEED) * ((float)rand() / RAND_MAX);
    setVelocity(df::Vector(speed, 0));
}

void Fish::out() {
    // Only recycle once past the right edge (not while in the left spawn area).
    if (getPosition().getX() >= 0)
        moveToStart();
}

int Fish::eventHandler(const df::Event* p_e) {

    if (p_e->getType() == df::OUT_EVENT) {
        out();
        return 1;
    }

    // TODO: when Hook exists, handle df::COLLISION_EVENT here and check
    // for the other object's type "Hook" to mark the fish as caught.

    return 0;
}