#include "TitleScreen.h"
#include "Fish.h"
#include "Junk.h"
#include "Hook.h"
#include "Boat.h"
#include "Lives.h"
#include "Points.h"
#include "Waterline.h"
#include "Music.h"
#include "ResourceManager.h"
#include "DisplayManager.h"
#include "EventKeyboard.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"


//How many of each to create when the game starts.
const int START_FISH = 6;
const int START_JUNK = 2;   //more spawn as junk drifts past

const int BLINK_STEPS = 15; //prompt toggles every ~0.5 seconds

TitleScreen::TitleScreen() {

    setType("TitleScreen");
    setAltitude(df::MAX_ALTITUDE);

    setPosition(df::Vector(WM.getBoundary().getHorizontal() / 2.0f,
        WM.getBoundary().getVertical() / 2.0f));

    m_steps = 0;

    registerInterest(df::KEYBOARD_EVENT);
    registerInterest(df::STEP_EVENT);
}

void TitleScreen::start() {

    //Ignore repeat SPACE presses before this title screen is deleted.
    if (m_started)
        return;
    m_started = true;

    //New game: restart the junk difficulty ramp.
    Junk::resetPassCount();

    for (int i = 0; i < START_FISH; i++)
        new Fish();
    for (int i = 0; i < START_JUNK; i++)
        new Junk();
    new Hook();
    new Waterline();
    new Boat();
    new Points();
    new Lives();

    //Background music while fishing (loops).
    df::Music* p_music = RM.getMusic("background-music");
    if (p_music != NULL)
        p_music->play(true);

    //Title screen is done.
    WM.markForDelete(this);
}

int TitleScreen::eventHandler(const df::Event* p_e) {

    if (p_e->getType() == df::KEYBOARD_EVENT) {
        const df::EventKeyboard* p_k =
            dynamic_cast<const df::EventKeyboard*>(p_e);

        if (p_k->getKeyboardAction() == df::KEY_PRESSED) {
            switch (p_k->getKey()) {
            case df::Keyboard::SPACE:
                start();
                break;
            case df::Keyboard::Q:
                GM.setGameOver(true);
                break;
            default:
                break;
            }
        }
        return 1;
    }

    if (p_e->getType() == df::STEP_EVENT) {
        m_steps++;
        return 1;
    }

    return 0;
}

//Draw one line of text, centered (or left-justified at x) on row y.
static void line(float x, float y, std::string text,
    df::Justification just, df::Color color) {
    DM.drawString(df::Vector(x, y), text, just, color);
}

int TitleScreen::draw() {

    float cx = WM.getBoundary().getHorizontal() / 2.0f;
    df::Justification C = df::CENTER_JUSTIFIED;

    //Title banner.
    const char* waves = "~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~";
    line(cx, 1, waves, C, df::CYAN);
    line(cx, 3, "D R A G O N - F L Y", C, df::YELLOW);
    line(cx, 4, "F I S H I N G", C, df::YELLOW);
    line(cx, 5, waves, C, df::CYAN);
    line(cx, 7, "}(0)        }(0)        }(0)", C, df::YELLOW);

    //How to play.
    line(cx, 9, "HOW TO PLAY", C, df::GREEN);
    line(cx, 10, "Move the mouse up and down to move your hook (J).", C, df::WHITE);
    line(cx, 11, "Touch a fish with the hook to catch it.", C, df::WHITE);
    line(cx, 12, "Avoid the tin cans! Each can you hit costs a life.", C, df::WHITE);
    line(cx, 13, "You have 3 lives. Lose them all and it's game over.", C, df::WHITE);

    //Controls (left-justified so the columns line up).
    line(cx, 15, "CONTROLS", C, df::GREEN);
    float x = cx - 16;
    df::Justification L = df::LEFT_JUSTIFIED;
    line(x, 16, "Mouse  :  move the hook up / down", L, df::WHITE);
    line(x, 17, "SPACE  :  start the game", L, df::WHITE);
    line(x, 18, "Right Click  :  score fish", L, df::WHITE);
    line(x, 19, "Q      :  quit", L, df::WHITE);
    

    //Blinking prompt.
    if ((m_steps / BLINK_STEPS) % 2 == 0)
        line(cx, 21, "Press SPACE to play", C, df::YELLOW);

    line(cx, 22, "Kelsey Bishqemi & Joseph D'Angelo  -  IMGD 3000", C, df::CYAN);

    return 0;
}