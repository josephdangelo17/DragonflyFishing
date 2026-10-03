#include <stdlib.h>

#include "Junk.h"
#include "EventOut.h"
#include "LogManager.h"
#include "WorldManager.h"

// Tuning values
const int   JUNK_WATER_TOP = 6;      // first row below the waterline
const int   JUNK_BOTTOM_PAD = 3;      // sprite is 2 rows tall, keep off bottom
const float JUNK_MIN_SPEED = 0.15f;  // columns per game step (slower than fish)
const float JUNK_MAX_SPEED = 0.35f;
const int   JUNK_MAX_COUNT = 12;     // never have more than this many at once
const int   JUNK_PASSES_PER_SPAWN = 2; // every N junk that pass, add one more

// Shared by all Junk.
static int junk_count = 0;  // how many Junk currently exist
static int junk_passed = 0;  // how many have drifted off the right edge

Junk::Junk() {

    if (setSprite("junk") != 0)
        LM.writeLog("Junk::Junk(): Warning! Sprite 'junk' not found");

    setType("Junk");

    setSolidness(df::SOFT);
    setAltitude(2);

    junk_count++;
    moveToStart();
}

Junk::~Junk() {
    junk_count--;
}

void Junk::moveToStart() {

    int world_vert = (int)WM.getBoundary().getVertical();

    //Start a random distance off the left edge so junk is staggered.
    float x = -(float)(getBox().getHorizontal() + rand() % 30);

    //Random depth between waterline and bottom.
    int range = world_vert - JUNK_WATER_TOP - JUNK_BOTTOM_PAD;
    float y = (float)(JUNK_WATER_TOP + rand() % range);

    WM.moveObject(this, df::Vector(x, y));

    //Random speed, always drifting right.
    float speed = JUNK_MIN_SPEED + (JUNK_MAX_SPEED - JUNK_MIN_SPEED) * ((float)rand() / RAND_MAX);
    setVelocity(df::Vector(speed, 0));
}

void Junk::out() {

    //Only count once past the right edge (not while in the left spawn area).
    if (getPosition().getX() < 0)
        return;

    junk_passed++;

    //Every JUNK_PASSES_PER_SPAWN junk that go by, add one more
    if (junk_passed % JUNK_PASSES_PER_SPAWN == 0 && junk_count < JUNK_MAX_COUNT)
        new Junk();

    //Recycle this one back to the left side.
    moveToStart();
}

int Junk::eventHandler(const df::Event* p_e) {

    if (p_e->getType() == df::OUT_EVENT) {
        out();
        return 1;
    }

    //TODO: when Hook exists, handle df::COLLISION_EVENT here and check
    //for the other object's type "Hook" to cost the player a life.

    return 0;
}