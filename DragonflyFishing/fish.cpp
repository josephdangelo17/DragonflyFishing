#include <stdlib.h>
#include "ResourceManager.h"
#include "Fish.h"
#include "EventOut.h"
#include "EventCollision.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "EventView.h"
#include "Points.h"
#include "hook.h"

//Tuning values 
const int   FISH_WATER_TOP = 6;     //first row below the waterline
const int   FISH_BOTTOM_PAD = 2;     //keep fish off the very bottom row
const float FISH_MIN_SPEED = 0.20f; //columns per game step
const float FISH_MAX_SPEED = 0.60f;

Fish::Fish() {
    if (setSprite("fish") != 0)
        LM.writeLog("Fish::Fish(): Warning! Sprite 'fish' not found");

    m_caught = false;

    setType("Fish");

    setSolidness(df::SOFT);
    setAltitude(2);

    moveToStart();
}

void Fish::moveToStart() {

    int world_vert = (int)WM.getBoundary().getVertical();

    //Start a random distance off the left edge so fish are staggered.
    float x = -(float)(getBox().getHorizontal() + rand() % 30);

    //Random depth between waterline and bottom.
    int range = world_vert - FISH_WATER_TOP - FISH_BOTTOM_PAD;

    float y = (float)(FISH_WATER_TOP + rand() % range);

    WM.moveObject(this, df::Vector(x, y));

    //Random speed, always moving right.
    float speed = FISH_MIN_SPEED +
        (FISH_MAX_SPEED - FISH_MIN_SPEED) * ((float)rand() / RAND_MAX);
    setVelocity(df::Vector(speed, 0));
}

void Fish::out() {
    //Only recycle once past the right edge (not while in the left spawn area).
    if (getPosition().getX() >= 0)
        moveToStart();
}

void Fish::caught() {

    //Only react once 
    if (m_caught)
        return;
    m_caught = true;

    df::Sound* p_sound = RM.getSound("point");
    if (p_sound != NULL)
        p_sound->play();

   
    df::EventView ev(POINTS_STRING, 5, true);
    WM.onEvent(&ev);

    //Remove this fish and spawn a replacement off the left edge.
    WM.removeObject(this);
    new Fish();
}

int Fish::eventHandler(const df::Event* p_e) {

    if (p_e->getType() == df::OUT_EVENT) {
        out();
        return 1;
    }

    return 0;
}