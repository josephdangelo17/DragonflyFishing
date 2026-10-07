
#include "Hook.h"
#include "Fish.h"
#include "Junk.h"
#include "Lives.h"
#include "TitleScreen.h"
#include "Sound.h"
#include "ResourceManager.h"
#include "DisplayManager.h"
#include "EventStep.h"
#include "EventView.h"
#include "LogManager.h"
#include "ObjectList.h"
#include "ObjectListIterator.h"
#include "WorldManager.h"
#include "utility.h"

//Tuning values
const int HOOK_START_LIVES = 3;   //junk hits allowed before game over
const int HOOK_MIN_Y = 4;   //highest row the hook can reach
const int HOOK_GAME_OVER_STEPS = 90;  

Hook::Hook() {


    if (setSprite("hook") != 0)
        LM.writeLog("Hook::Hook(): Warning! Sprite 'hook' not found");

    setType("Hook");


    setSolidness(df::SOFT);
    setAltitude(3);  //draw on top of fish and junk

    m_lives = HOOK_START_LIVES;
    m_game_over = false;
    m_leaving = false;
    m_done = false;
    m_game_over_countdown = HOOK_GAME_OVER_STEPS;
    p_fish = nullptr;
    m_fish_held = false;

    //Fixed column in the middle of the screen, starting near the top.
    int world_horiz = (int)WM.getBoundary().getHorizontal();
    setPosition(df::Vector(world_horiz / 2.0f, (float)HOOK_MIN_Y));

    registerInterest(df::MSE_EVENT);
    registerInterest(df::KEYBOARD_EVENT);
    registerInterest(df::STEP_EVENT);
}

int Hook::getLives() const {
    return m_lives;
}

void Hook::mouse(const df::EventMouse* p_e) {

    //Stop following the mouse once the game is over.
    if (m_game_over || m_leaving)
        return;

    //Mouse position is in window (view) coordinates
    df::Vector pos = df::viewToWorld(p_e->getMousePosition());

    //Only up/down: keep our column, clamp row to the lake.
    int world_vert = (int)WM.getBoundary().getVertical();
    float y = pos.getY();
    if (y < HOOK_MIN_Y) y = (float)HOOK_MIN_Y;
    if (y > world_vert - 1) y = (float)(world_vert - 1);

    WM.moveObject(this, df::Vector(getPosition().getX(), y));

    if ((p_e->getMouseAction() == df::CLICKED) && (p_e->getMouseButton() == df::Mouse::LEFT)) {
        fishCaught();
    }
}

void Hook::keyboard(const df::EventKeyboard* p_e) {

    //Q: leave the game and go back to the title screen
    if (p_e->getKeyboardAction() == df::KEY_PRESSED &&
        p_e->getKey() == df::Keyboard::Q) {
        LM.writeLog("Hook: Q pressed, returning to title");
        m_leaving = true;
    }
}

void Hook::collide(const df::EventCollision* p_e) {

    if (m_game_over || m_leaving)
        return;

    //Figure out which of the two objects is the other one.
    df::Object* p_other = (p_e->getObject1() == this) ?
        p_e->getObject2() : p_e->getObject1();
    if (p_other == NULL)
        return;

    if (p_other->getType() == "Fish") {
        //static_cast<Fish*>(p_other)->caught();

        if (!m_fish_held) {

            m_fish_held = true;
            p_fish = static_cast<Fish*>(p_other);

            LM.writeLog("Hook: caught a fish");

        }

    }
    else if (p_other->getType() == "Junk") {
        //hit() returns true only the first time, so one can = one life.
        if (static_cast<Junk*>(p_other)->hit()) {
            m_lives--;
            LM.writeLog("Hook: hit junk! Lives left: %d", m_lives);

            df::EventView ev(LIVES_STRING, -1, true);
            WM.onEvent(&ev);

            // Clank!
            df::Sound* p_sound = RM.getSound("junk-hit");
            if (p_sound != NULL)
                p_sound->play();
            if (m_lives <= 0)
                gameOver();
        }
    }
}

void Hook::gameOver() {
    df::Music* p_music = RM.getMusic("background-music");
    if (p_music != NULL)
        p_music->stop();
    m_game_over = true;
    m_game_over_countdown = HOOK_GAME_OVER_STEPS;
    LM.writeLog("Hook: GAME OVER");
}

void Hook::returnToTitle() {

    //Only ever do this once.
    if (m_done)
        return;
    m_done = true;

    //Ignore everything from here on (collisions, mouse, more keys).
    m_leaving = true;

    df::Music* p_music = RM.getMusic("background-music");
    if (p_music != NULL)
        p_music->stop();

    //Remove all fish.
    df::ObjectList fish = WM.objectsOfType("Fish");
    df::ObjectListIterator fi(&fish);
    for (fi.first(); !fi.isDone(); fi.next())
        WM.markForDelete(fi.currentObject());

    //Remove all junk (remove() also stops it spawning replacements).
    df::ObjectList junk = WM.objectsOfType("Junk");
    df::ObjectListIterator ji(&junk);
    for (ji.first(); !ji.isDone(); ji.next())
        static_cast<Junk*>(ji.currentObject())->remove();

    //Remove the hook itself.
    WM.markForDelete(this);

    //Back to the title screen.
    new TitleScreen();
}

void Hook::step() {

    if (m_fish_held && p_fish != nullptr) {
        p_fish->setPosition(getPosition());
    }

    //Count down the "GAME OVER" message.
    if (m_game_over && !m_leaving)
        m_game_over_countdown--;

    //leave when Q was pressed, or the game over message has been shown.
    if (m_leaving || (m_game_over && m_game_over_countdown <= 0))
        returnToTitle();
}

int Hook::eventHandler(const df::Event* p_e) {

    if (p_e->getType() == df::MSE_EVENT) {
        mouse(dynamic_cast<const df::EventMouse*>(p_e));
        return 1;
    }

    if (p_e->getType() == df::KEYBOARD_EVENT) {
        keyboard(dynamic_cast<const df::EventKeyboard*>(p_e));
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

    if (m_game_over && !m_leaving) {
        df::Vector center(WM.getBoundary().getHorizontal() / 2.0f,
            WM.getBoundary().getVertical() / 2.0f);
        DM.drawString(center, "GAME OVER", df::CENTER_JUSTIFIED, df::RED);
    }

    return result;
}
