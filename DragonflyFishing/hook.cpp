#include "Hook.h"
#include "Fish.h"
#include "Junk.h"
#include "Lives.h"

#include "DisplayManager.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "utility.h"
#include "vector.h"
#include "EventView.h"

//Tuning values.
const int HOOK_START_LIVES = 3;   //junk hits allowed before game over
const int HOOK_MIN_Y = 4;   //highest row the hook can reach
const int HOOK_GAME_OVER_STEPS = 90;  //~3 seconds at 30 steps/second

Hook::Hook() {
    if (setSprite("hook") != 0)
        LM.writeLog("Hook::Hook(): Warning! Sprite 'hook' not found");

    setType("Hook");

    
    setSolidness(df::SOFT);
    setAltitude(3);  //draw on top of fish and junk

    m_lives = HOOK_START_LIVES;
    m_game_over = false;
    m_game_over_countdown = HOOK_GAME_OVER_STEPS;
    m_fish_held = false;
    p_fish = nullptr;

    //Fixed column in the middle of the screen, starting near the top.
    int world_horiz = (int)WM.getBoundary().getHorizontal();
    setPosition(df::Vector(world_horiz / 2.0f, (float)HOOK_MIN_Y));

    registerInterest(df::MSE_EVENT);
    registerInterest(df::STEP_EVENT);
}

int Hook::getLives() const {
    return m_lives;
}

void Hook::mouse(const df::EventMouse* p_e) {

    //Stop following the mouse once the game is over.
    if (m_game_over)
        return;

    //Mouse position is in window (view) coordinates; convert to world.
    df::Vector pos = df::viewToWorld(p_e->getMousePosition());

    //Only up/down: keep our column, clamp row to the lake.
    int world_vert = (int)WM.getBoundary().getVertical();
    float y = pos.getY();
    if (y < HOOK_MIN_Y) {
        y = (float)HOOK_MIN_Y;
    }
    if (y > world_vert - 1) {
        y = (float)(world_vert - 1);
    }

    WM.moveObject(this, df::Vector(getPosition().getX(), y));

    //if (m_fish_held && p_fish != nullptr) {
    //    p_fish->setPosition(getPosition());
    //}

    if ((p_e->getMouseAction() == df::CLICKED) && (p_e->getMouseButton() == df::Mouse::LEFT)) {
        fishCaught();
    }

}

void Hook::collide(const df::EventCollision* p_e) {

    if (m_game_over)
        return;

    //Figure out which of the two objects is the other one.
    df::Object* p_other;
    if (p_e->getObject1() == this) {
        p_other = p_e->getObject2(); 
    }
    else {
        p_other = p_e->getObject1(); 
    }

    if (p_other == nullptr) {
        return;
    }

    if (p_other->getType() == "Fish") {

        //static_cast<Fish*>(p_other)->caught();

        if (!m_fish_held) {
            
            m_fish_held = true;
            p_fish = static_cast<Fish*>(p_other);

            LM.writeLog("Hook: caught a fish");

        }

        /* if (static_cast<Fish*>(p_other)->getPosition().getY() == 6) {
            
        } */

    }

    else if (p_other->getType() == "Junk") {
        //hit() returns true only the first time, so one can = one life.
        if (static_cast<Junk*>(p_other)->hit()) {
            m_lives--;

            df::EventView ev(LIVES_STRING, -1, true);
            WM.onEvent(&ev);

            LM.writeLog("Hook: hit junk! Lives left: %d", m_lives);
            if (m_lives <= 0)
                gameOver();
        }
    }
}

void Hook::gameOver() {
    m_game_over = true;
    m_game_over_countdown = HOOK_GAME_OVER_STEPS;
    LM.writeLog("Hook: GAME OVER");
}

void Hook::step() {

    if (m_fish_held && p_fish != nullptr) {
        p_fish->setPosition(getPosition());
    }

    //After showing "GAME OVER" for a bit, end the game.
    if (m_game_over && --m_game_over_countdown <= 0)
        GM.setGameOver(true);
}

int Hook::eventHandler(const df::Event* p_e) {

    if (p_e->getType() == df::MSE_EVENT) {
        mouse(dynamic_cast<const df::EventMouse*>(p_e));
        return 1;
    }

    if (p_e->getType() == df::COLLISION_EVENT) {
        collide(dynamic_cast<const df::EventCollision*>(p_e));
        return 1;
    }

    if (p_e->getType() == df::STEP_EVENT) {
        step();
        return 1;
    }

    return 0;
}

void Hook::fishCaught() {

    if (!m_fish_held) {
        return;
    }

    if (p_fish == nullptr) {
        return;
    }

    if (getPosition().getY() > HOOK_MIN_Y) {
        return;
    }

    p_fish->caught();
    //WM.removeObject(p_fish);
    p_fish = nullptr;
    m_fish_held = false;
}

int Hook::draw() {    

    df::Vector hook_pos = getPosition();

    //draw fishing line behind hook
    for (int i = 4; i < hook_pos.getY(); i++) {
        DM.drawCh(df::Vector(hook_pos.getX(), i), '|', df::COLOR_DEFAULT);
    }

    int result = df::Object::draw();

    if (m_game_over) {
        df::Vector center(WM.getBoundary().getHorizontal() / 2.0f,
            WM.getBoundary().getVertical() / 2.0f);
        DM.drawString(center, "GAME OVER", df::CENTER_JUSTIFIED, df::RED);
    }

    return result;
}