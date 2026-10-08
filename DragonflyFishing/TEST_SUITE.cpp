//
// TEST_SUITE.cpp - Test suite for the Dragonfly Fishing game classes.
//

// Engine includes.
#include "Manager.h"
#include "LogManager.h"
#include "Vector.h"
#include "Object.h"
#include "ObjectList.h"
#include "ObjectListIterator.h"
#include "Event.h"
#include "EventStep.h"
#include "EventOut.h"
#include "EventCollision.h"
#include "EventMouse.h"
#include "EventKeyboard.h"
#include "EventView.h"
#include "ViewObject.h"
#include "WorldManager.h"
#include "GameManager.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Music.h"
#include "Box.h"
#include "utility.h"

// Game includes.
#include "Boat.h"
#include "Waterline.h"
#include "Fish.h"
#include "Junk.h"
#include "Lives.h"
#include "Points.h"
#include "Hook.h"
#include "TitleScreen.h"

// System includes.
#include <cstdio>
#include <cmath>
#include <chrono>
#include <thread>
#include <string>

static int g_tests_run = 0;
static int g_tests_passed = 0;

// these mirror the tuning constants in the game's .cpp files; if a
// constant is changed in the game, update it here too
static const int T_START_FISH = 6;       // titleScreen.cpp  START_FISH
static const int T_START_JUNK = 2;       // titleScreen.cpp  START_JUNK
static const int T_START_LIVES = 3;      // hook.cpp         HOOK_START_LIVES
static const int T_HOOK_MIN_Y = 4;       // hook.cpp         HOOK_MIN_Y
static const int T_GAME_OVER_STEPS = 90; // hook.cpp         HOOK_GAME_OVER_STEPS
static const int T_JUNK_MAX_COUNT = 12;  // junk.cpp         JUNK_MAX_COUNT
static const int T_FISH_POINTS = 5;      // fish.cpp         points for a catch

// record and print the result of a single check
static void check(const std::string& name, bool passed) {
	g_tests_run++;
	if (passed) {
		g_tests_passed++;
		printf("  [PASS] %s\n", name.c_str());
	}
	else {
		printf("  [FAIL] %s\n", name.c_str());
	}
}

// a check that could not be run on this machine (not counted either way)
static void skip(const std::string& name, const std::string& reason) {
	printf("  [SKIP] %s (%s)\n", name.c_str(), reason.c_str());
}

static void printHeader(const std::string& title) {
	printf("\n--- %s ---\n", title.c_str());
}

static bool nearly(float a, float b) {
	return std::fabs(a - b) < 0.0001f;
}

// ======================================================================
// Helpers
// ======================================================================

// (re)load one game sprite by label; harmless if it is already loaded
static void loadSprite(const std::string& label) {
	RM.loadSprite("sprites/" + label + "-spr.txt", label);
}

static void loadGameResources() {
	loadSprite("fish");
	loadSprite("junk");
	loadSprite("hook");
	loadSprite("boat");
	RM.loadSound("sounds/junk-hit.wav", "junk-hit");
	RM.loadMusic("sounds/background-music.wav", "background-music");
}

// delete every object in the world and reset the junk difficulty ramp,
// so each test starts from a known, clean state
static void clearWorld() {
	df::ObjectList all = WM.getAllObjects(true);
	df::ObjectListIterator it(&all);
	for (it.first(); !it.isDone(); it.next())
		WM.markForDelete(it.currentObject());
	WM.update();
	Junk::resetPassCount();
}

static int countOfType(const std::string& type) {
	return WM.objectsOfType(type).getCount();
}

// first object of a type, or NULL if there is none
static df::Object* firstOfType(const std::string& type) {
	df::ObjectList list = WM.objectsOfType(type);
	if (list.getCount() == 0)
		return NULL;
	return list[0];
}

// number of ViewObjects (Points / Lives displays) with the given label
static int countViewObjects(const std::string& label) {
	int n = 0;
	df::ObjectList all = WM.getAllObjects(true);
	df::ObjectListIterator it(&all);
	for (it.first(); !it.isDone(); it.next()) {
		df::ViewObject* p = dynamic_cast<df::ViewObject*>(it.currentObject());
		if (p != NULL && p->getViewString() == label)
			n++;
	}
	return n;
}

// sum of the values shown by every display with the given label.
// With exactly one display this is just that display's value; if old
// displays were left behind, their values show up in the sum.
static int sumViewValues(const std::string& label) {
	int sum = 0;
	df::ObjectList all = WM.getAllObjects(true);
	df::ObjectListIterator it(&all);
	for (it.first(); !it.isDone(); it.next()) {
		df::ViewObject* p = dynamic_cast<df::ViewObject*>(it.currentObject());
		if (p != NULL && p->getViewString() == label)
			sum += p->getValue();
	}
	return sum;
}

static void pressKey(df::Object* p_o, df::Keyboard::Key key,
	df::EventKeyboardAction action = df::KEY_PRESSED) {
	df::EventKeyboard ev;
	ev.setKey(key);
	ev.setKeyboardAction(action);
	p_o->eventHandler(&ev);
}

static void sendStep(df::Object* p_o, int times = 1) {
	for (int i = 0; i < times; i++) {
		df::EventStep ev;
		p_o->eventHandler(&ev);
	}
}

static void sendMouse(df::Object* p_o, df::EventMouseAction action,
	df::Mouse::Button button, float x, float y) {
	df::EventMouse ev;
	ev.setMouseAction(action);
	ev.setMouseButton(button);
	ev.setMousePosition(df::Vector(x, y));
	p_o->eventHandler(&ev);
}

static void sendCollision(df::Object* p_hook, df::Object* p_other) {
	df::EventCollision ev(p_hook, p_other, p_hook->getPosition());
	p_hook->eventHandler(&ev);
}

static void sendOut(df::Object* p_o) {
	df::EventOut ev;
	p_o->eventHandler(&ev);
}

// clean world -> title screen -> SPACE -> game running
static void startNewGame() {
	clearWorld();
	TitleScreen* p_title = new TitleScreen();
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
}

// player presses Q during the game; the hook returns to the title on
// its next step event
static void quitToTitle() {
	df::Object* p_hook = firstOfType("Hook");
	if (p_hook == NULL)
		return;
	pressKey(p_hook, df::Keyboard::Q);
	sendStep(p_hook);
	WM.update();
}

// player loses all lives; the hook shows GAME OVER and returns to the
// title once the countdown finishes
static void loseAllLivesToTitle() {
	df::Object* p_hook = firstOfType("Hook");
	if (p_hook == NULL)
		return;
	for (int i = 0; i < T_START_LIVES; i++) {
		Junk* p_junk = new Junk();
		sendCollision(p_hook, p_junk);
	}
	sendStep(p_hook, T_GAME_OVER_STEPS);
	WM.update();
}

// true while the background music is actually playing (needs an audio
// device; always false on a machine without one)
static bool musicPlaying() {
	df::Music* p_music = RM.getMusic("background-music");
	if (p_music == NULL)
		return false;
	return p_music->getMusic()->getStatus() == sf::SoundSource::Status::Playing;
}

// A freshly-started game should look the same every time: one of each
// display/scenery object, the starting fish/junk, a full-lives hook, a
// zero score, and no title screen.
static void verifyFreshGame(const std::string& ctx) {
	check(ctx + ": exactly 1 Hook", countOfType("Hook") == 1);
	check(ctx + ": " + std::to_string(T_START_FISH) + " Fish",
		countOfType("Fish") == T_START_FISH);
	check(ctx + ": " + std::to_string(T_START_JUNK) + " Junk",
		countOfType("Junk") == T_START_JUNK);
	check(ctx + ": exactly 1 Waterline", countOfType("Waterline") == 1);
	check(ctx + ": exactly 1 Boat", countOfType("Boat") == 1);
	check(ctx + ": exactly 1 Points display",
		countViewObjects(POINTS_STRING) == 1);
	check(ctx + ": exactly 1 Lives display",
		countViewObjects(LIVES_STRING) == 1);
	check(ctx + ": no TitleScreen left over", countOfType("TitleScreen") == 0);
	check(ctx + ": score shown is 0", sumViewValues(POINTS_STRING) == 0);
	check(ctx + ": lives shown is " + std::to_string(T_START_LIVES),
		sumViewValues(LIVES_STRING) == T_START_LIVES);

	Hook* p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	check(ctx + ": hook has full lives",
		p_hook != NULL && p_hook->getLives() == T_START_LIVES);
}

// After leaving a game, only the title screen should remain.
static void verifyOnlyTitleScreen(const std::string& ctx) {
	check(ctx + ": exactly 1 TitleScreen", countOfType("TitleScreen") == 1);
	check(ctx + ": no Hook left", countOfType("Hook") == 0);
	check(ctx + ": no Fish left", countOfType("Fish") == 0);
	check(ctx + ": no Junk left", countOfType("Junk") == 0);
	check(ctx + ": no Waterline left", countOfType("Waterline") == 0);
	check(ctx + ": no Boat left", countOfType("Boat") == 0);
	check(ctx + ": no Points display left",
		countViewObjects(POINTS_STRING) == 0);
	check(ctx + ": no Lives display left",
		countViewObjects(LIVES_STRING) == 0);
	check(ctx + ": world holds only the title screen",
		WM.getAllObjects(true).getCount() == 1);
}

// ======================================================================
// Boat
// ======================================================================
static void testBoat() {
	printHeader("Boat");
	clearWorld();

	// happy path - constructor sets up the boat
	Boat* p_boat = new Boat();
	check("type is \"Boat\"", p_boat->getType() == "Boat");
	// Boat asks for altitude 5, but the engine only accepts 0..MAX_ALTITUDE
	// and silently ignores out-of-range requests, so check that the
	// request was actually accepted (i.e. it is not still the default)
	df::Object* p_plain = new df::Object();
	check("altitude request was accepted by the engine (not left at default)",
		p_boat->getAltitude() != p_plain->getAltitude());
	check("altitude is within 0..MAX_ALTITUDE",
		p_boat->getAltitude() >= 0 && p_boat->getAltitude() <= df::MAX_ALTITUDE);
	delete p_plain;
	check("position is (40,2)",
		p_boat->getPosition().getX() == 40 && p_boat->getPosition().getY() == 2);
	check("solidness is SPECTRAL", p_boat->getSolidness() == df::SPECTRAL);
	check("sprite attached", p_boat->getAnimation().getSprite() != NULL);
	check("box matches 9x3 boat sprite",
		p_boat->getBox().getHorizontal() == 9 &&
		p_boat->getBox().getVertical() == 3);
	check("draw() returns 0", p_boat->draw() == 0);

	// edge case - SPECTRAL means nothing can collide with the boat
	df::Object* p_mover = new df::Object();
	p_mover->setSolidness(df::HARD);
	p_mover->setPosition(df::Vector(40, 2));
	df::ObjectList hits = WM.getCollisions(p_mover, df::Vector(40, 2));
	bool found = false;
	for (int i = 0; i < hits.getCount(); i++)
		if (hits[i] == p_boat) found = true;
	check("objects can move straight through the boat (no collision)", !found);
	delete p_mover;

	// error condition - boat sprite was never loaded: construct anyway,
	// without crashing, and keep the other settings
	RM.unloadSprite("boat");
	Boat* p_boat2 = new Boat();
	check("Boat with no 'boat' sprite loaded still constructs",
		p_boat2->getType() == "Boat");
	check("Boat with no sprite has no sprite attached",
		p_boat2->getAnimation().getSprite() == NULL);
	check("Boat with no sprite keeps its position",
		p_boat2->getPosition().getX() == 40 && p_boat2->getPosition().getY() == 2);
	loadSprite("boat");

	delete p_boat;
	delete p_boat2;
}

// ======================================================================
// Waterline
// ======================================================================
static void testWaterline() {
	printHeader("Waterline");
	clearWorld();

	// happy path - constructor and draw
	Waterline* p_water = new Waterline();
	check("type is \"Waterline\"", p_water->getType() == "Waterline");
	check("solidness is SPECTRAL", p_water->getSolidness() == df::SPECTRAL);
	check("altitude is within the valid range",
		p_water->getAltitude() >= 0 && p_water->getAltitude() <= df::MAX_ALTITUDE);
	check("draw() returns 0", p_water->draw() == 0);

	// edge case - a zero-width world draws nothing but does not crash
	df::Box original = WM.getBoundary();
	WM.setBoundary(df::Box(df::Vector(0, 0), 0, original.getVertical()));
	check("draw() with a zero-width world returns 0", p_water->draw() == 0);

	// error condition - a negative-width world (nonsense) must not crash
	WM.setBoundary(df::Box(df::Vector(0, 0), -5, original.getVertical()));
	check("draw() with a negative-width world returns 0", p_water->draw() == 0);
	WM.setBoundary(original);

	delete p_water;
}

// ======================================================================
// Fish
// ======================================================================
static void testFish() {
	printHeader("Fish");
	clearWorld();

	// happy path - constructor
	Fish* p_fish = new Fish();
	check("type is \"Fish\"", p_fish->getType() == "Fish");
	check("solidness is SOFT", p_fish->getSolidness() == df::SOFT);
	check("altitude is 2", p_fish->getAltitude() == 2);
	check("sprite attached", p_fish->getAnimation().getSprite() != NULL);
	check("starts off the left edge of the screen",
		p_fish->getPosition().getX() < 0);
	float speed = p_fish->getVelocity().getX();
	check("moves right at a speed between 0.20 and 0.60",
		speed >= 0.20f - 0.0001f && speed <= 0.60f + 0.0001f);
	check("moves purely horizontally", nearly(p_fish->getVelocity().getY(), 0));

	// edge case - many fish all spawn in the water, off the left edge, and
	// moving right (checks the random start-position range)
	bool all_ok = true;
	int world_vert = (int)WM.getBoundary().getVertical();
	for (int i = 0; i < 50; i++) {
		Fish* p = new Fish();
		float y = p->getPosition().getY();
		float vx = p->getVelocity().getX();
		if (p->getPosition().getX() >= 0 || y < 6 || y >= world_vert - 2 || vx <= 0)
			all_ok = false;
		delete p;
	}
	check("50 spawns: all start left of screen, below waterline, above bottom",
		all_ok);

	// happy path - an OUT event on the right edge recycles the fish to the left
	p_fish->setPosition(df::Vector(10, 10));
	df::EventOut out_ev;
	int result = p_fish->eventHandler(&out_ev);
	check("OUT event is handled (returns 1)", result == 1);
	check("fish past the right edge is recycled to the left",
		p_fish->getPosition().getX() < 0);

	// edge case - an OUT event while still in the left spawn area is ignored
	df::Vector before = p_fish->getPosition();
	sendOut(p_fish);
	check("OUT event in the left spawn area does not move the fish",
		p_fish->getPosition().getX() == before.getX() &&
		p_fish->getPosition().getY() == before.getY());

	// edge case - other events are not handled
	df::EventStep step;
	check("non-OUT event returns 0", p_fish->eventHandler(&step) == 0);

	// happy path - caught() awards points and swaps in a replacement fish
	Points* p_points = new Points();
	int fish_before = countOfType("Fish");
	p_fish->caught();
	check("caught() adds 5 points", p_points->getValue() == T_FISH_POINTS);
	check("caught fish is removed from the world",
		WM.objectWithId(p_fish->getId()) == NULL);
	check("a replacement fish keeps the fish count the same",
		countOfType("Fish") == fish_before);

	// edge case - caught() only reacts once
	p_fish->caught();
	check("second caught() on the same fish gives no more points",
		p_points->getValue() == T_FISH_POINTS);
	check("second caught() does not spawn another replacement",
		countOfType("Fish") == fish_before);

	// error condition - 'fish' sprite never loaded: construct without crashing
	RM.unloadSprite("fish");
	Fish* p_nosprite = new Fish();
	check("Fish with no 'fish' sprite loaded still constructs",
		p_nosprite->getType() == "Fish");
	check("Fish with no sprite has no sprite attached",
		p_nosprite->getAnimation().getSprite() == NULL);
	check("Fish with no sprite still spawns off the left edge",
		p_nosprite->getPosition().getX() < 0);
	loadSprite("fish");

	delete p_fish;      // removed from the world by caught(), so we free it
	delete p_nosprite;
	delete p_points;
}

// ======================================================================
// Junk
// ======================================================================
static void testJunk() {
	printHeader("Junk");
	clearWorld();

	// happy path - constructor
	Junk* p_junk = new Junk();
	check("type is \"Junk\"", p_junk->getType() == "Junk");
	check("solidness is SOFT", p_junk->getSolidness() == df::SOFT);
	check("altitude is 2", p_junk->getAltitude() == 2);
	check("sprite attached", p_junk->getAnimation().getSprite() != NULL);
	check("starts off the left edge of the screen",
		p_junk->getPosition().getX() < 0);
	float speed = p_junk->getVelocity().getX();
	check("drifts right at a speed between 0.15 and 0.35",
		speed >= 0.15f - 0.0001f && speed <= 0.35f + 0.0001f);

	// happy path - hit() returns true, swaps in a replacement
	check("hit() the first time returns true", p_junk->hit());
	check("replacement is created straight away (old one awaits deletion)",
		countOfType("Junk") == 2);

	// edge case - hit() only reacts once
	check("hit() a second time returns false", !p_junk->hit());
	check("second hit() does not spawn another replacement",
		countOfType("Junk") == 2);
	WM.update();
	check("after update() the hit junk is gone, one replacement remains",
		countOfType("Junk") == 1);

	// happy path - remove() deletes without a replacement
	clearWorld();
	Junk* p_removed = new Junk();
	p_removed->setPosition(df::Vector(5, 10));

	// error condition - a removed junk must ignore everything afterwards
	p_removed->remove();
	sendOut(p_removed);
	check("OUT event after remove() does not recycle the junk",
		p_removed->getPosition().getX() == 5);
	check("hit() after remove() returns false", !p_removed->hit());
	WM.update();
	check("remove() leaves no junk behind and spawns no replacement",
		countOfType("Junk") == 0);

	// happy path - an OUT event past the right edge recycles to the left
	clearWorld();
	Junk* p_out = new Junk();
	p_out->setPosition(df::Vector(10, 10));
	df::EventOut out_ev;
	check("OUT event is handled (returns 1)", p_out->eventHandler(&out_ev) == 1);
	check("junk past the right edge is recycled to the left",
		p_out->getPosition().getX() < 0);

	// edge case - OUT event in the left spawn area is ignored; other
	// events are not handled
	df::Vector before = p_out->getPosition();
	sendOut(p_out);
	check("OUT event in the left spawn area does not move the junk",
		p_out->getPosition().getX() == before.getX());
	df::EventStep step;
	check("non-OUT event returns 0", p_out->eventHandler(&step) == 0);

	// happy path - every 2nd junk that passes adds one more junk
	clearWorld();
	Junk* p_a = new Junk();
	Junk::resetPassCount();
	p_a->setPosition(df::Vector(10, 10));
	sendOut(p_a);
	check("1st junk to pass: no extra junk yet", countOfType("Junk") == 1);
	p_a->setPosition(df::Vector(10, 10));
	sendOut(p_a);
	check("2nd junk to pass: one extra junk spawns", countOfType("Junk") == 2);

	// edge case - resetPassCount() restarts the ramp
	clearWorld();
	Junk* p_b = new Junk();
	Junk::resetPassCount();
	p_b->setPosition(df::Vector(10, 10));
	sendOut(p_b);                 // 1 pass
	Junk::resetPassCount();       // back to 0
	p_b->setPosition(df::Vector(10, 10));
	sendOut(p_b);                 // 1 pass again, NOT the 2nd
	check("resetPassCount() restarts the 2-pass spawn cycle",
		countOfType("Junk") == 1);

	// edge case - below the cap, the extra junk is still allowed
	clearWorld();
	for (int i = 0; i < T_JUNK_MAX_COUNT - 1; i++)
		new Junk();
	Junk::resetPassCount();
	df::Object* p_c = firstOfType("Junk");
	for (int i = 0; i < 2; i++) {
		p_c->setPosition(df::Vector(10, 10));
		sendOut(p_c);
	}
	check("with 11 junk, a spawn reaches the cap of 12",
		countOfType("Junk") == T_JUNK_MAX_COUNT);

	// error condition - at the cap, no more junk is ever spawned
	clearWorld();
	for (int i = 0; i < T_JUNK_MAX_COUNT; i++)
		new Junk();
	Junk::resetPassCount();
	p_c = firstOfType("Junk");
	for (int i = 0; i < 4; i++) {
		p_c->setPosition(df::Vector(10, 10));
		sendOut(p_c);
	}
	check("at the cap of 12, junk passing does not spawn more",
		countOfType("Junk") == T_JUNK_MAX_COUNT);

	// error condition - 'junk' sprite never loaded: no crash
	clearWorld();
	RM.unloadSprite("junk");
	Junk* p_nosprite = new Junk();
	check("Junk with no 'junk' sprite loaded still constructs",
		p_nosprite->getType() == "Junk");
	check("Junk with no sprite has no sprite attached",
		p_nosprite->getAnimation().getSprite() == NULL);
	loadSprite("junk");

	clearWorld();
}

// ======================================================================
// Lives
// ======================================================================
static void testLives() {
	printHeader("Lives");
	clearWorld();

	// happy path - constructor
	Lives* p_lives = new Lives();
	check("view string is \"Lives\"", p_lives->getViewString() == LIVES_STRING);
	check("starts with 3 lives shown", p_lives->getValue() == 3);
	check("located in the top right", p_lives->getLocation() == df::TOP_RIGHT);
	check("color is magenta", p_lives->getColor() == df::MAGENTA);

	// happy path - a -1 delta event takes a life off
	df::EventView lose_one(LIVES_STRING, -1, true);
	int result = p_lives->eventHandler(&lose_one);
	check("matching EventView is handled (returns 1)", result == 1);
	check("value drops from 3 to 2", p_lives->getValue() == 2);

	// edge case - an EventView for a different display is ignored
	df::EventView other_tag(POINTS_STRING, -1, true);
	result = p_lives->eventHandler(&other_tag);
	check("EventView with a different tag is not handled (returns 0)", result == 0);
	check("EventView with a different tag does not change the value",
		p_lives->getValue() == 2);

	// edge case - a non-delta event replaces the value
	df::EventView replace(LIVES_STRING, 3, false);
	p_lives->eventHandler(&replace);
	check("non-delta EventView replaces the value", p_lives->getValue() == 3);

	// edge case - a non-view event is not handled
	df::EventStep step;
	check("non-view event returns 0", p_lives->eventHandler(&step) == 0);

	// error condition - the display does not clamp at 0 (Hook is
	// responsible for ending the game at 0 lives)
	for (int i = 0; i < 4; i++)
		p_lives->eventHandler(&lose_one);
	check("display is not clamped: 3 - 4 = -1", p_lives->getValue() == -1);

	delete p_lives;
}

// ======================================================================
// Points
// ======================================================================
static void testPoints() {
	printHeader("Points");
	clearWorld();

	// happy path - constructor
	Points* p_points = new Points();
	check("view string is \"Points\"", p_points->getViewString() == POINTS_STRING);
	check("starts at 0 points", p_points->getValue() == 0);
	check("located in the top left", p_points->getLocation() == df::TOP_LEFT);
	check("color is magenta", p_points->getColor() == df::MAGENTA);

	// happy path - a +5 delta event
	df::EventView plus_five(POINTS_STRING, 5, true);
	int result = p_points->eventHandler(&plus_five);
	check("matching EventView is handled (returns 1)", result == 1);
	check("value rises to 5", p_points->getValue() == 5);

	// edge case - deltas accumulate
	p_points->eventHandler(&plus_five);
	p_points->eventHandler(&plus_five);
	check("three +5 events total 15", p_points->getValue() == 15);

	// edge case - negative delta and non-delta (replace)
	df::EventView minus_ten(POINTS_STRING, -10, true);
	p_points->eventHandler(&minus_ten);
	check("a -10 delta takes 15 down to 5", p_points->getValue() == 5);
	df::EventView reset(POINTS_STRING, 0, false);
	p_points->eventHandler(&reset);
	check("non-delta EventView of 0 resets the score", p_points->getValue() == 0);

	// error condition - an EventView for another display (Lives) must not
	// change the score
	df::EventView lives_event(LIVES_STRING, -1, true);
	result = p_points->eventHandler(&lives_event);
	check("EventView with a different tag is not handled (returns 0)", result == 0);
	check("EventView with a different tag does not change the score",
		p_points->getValue() == 0);

	delete p_points;
}

// ======================================================================
// Hook
// ======================================================================
static void testHook() {
	printHeader("Hook");
	clearWorld();

	int world_horiz = (int)WM.getBoundary().getHorizontal();
	int world_vert = (int)WM.getBoundary().getVertical();

	// happy path - constructor
	Hook* p_hook = new Hook();
	check("type is \"Hook\"", p_hook->getType() == "Hook");
	check("solidness is SOFT", p_hook->getSolidness() == df::SOFT);
	check("altitude is 3 (drawn above fish and junk)", p_hook->getAltitude() == 3);
	check("starts with 3 lives", p_hook->getLives() == T_START_LIVES);
	check("sprite attached", p_hook->getAnimation().getSprite() != NULL);
	check("starts in the middle column",
		nearly(p_hook->getPosition().getX(), world_horiz / 2.0f));
	check("starts at the top of the lake",
		nearly(p_hook->getPosition().getY(), (float)T_HOOK_MIN_Y));
	check("registered for mouse, keyboard and step events",
		p_hook->getEventCount() == 3);
	check("draw() returns 0", p_hook->draw() == 0);

	// happy path - mouse moves the hook up/down only
	float start_x = p_hook->getPosition().getX();
	sendMouse(p_hook, df::MOVED, df::Mouse::UNDEFINED_MOUSE_BUTTON, 10, 12);
	check("mouse move sets the hook's row", nearly(p_hook->getPosition().getY(), 12));
	check("mouse move never changes the hook's column",
		nearly(p_hook->getPosition().getX(), start_x));

	// edge case - the row is clamped to the lake
	sendMouse(p_hook, df::MOVED, df::Mouse::UNDEFINED_MOUSE_BUTTON, 10, 0);
	check("mouse above the lake clamps to the top row",
		nearly(p_hook->getPosition().getY(), (float)T_HOOK_MIN_Y));
	sendMouse(p_hook, df::MOVED, df::Mouse::UNDEFINED_MOUSE_BUTTON, 10, 500);
	check("mouse below the lake clamps to the bottom row",
		nearly(p_hook->getPosition().getY(), (float)(world_vert - 1)));
	sendMouse(p_hook, df::MOVED, df::Mouse::UNDEFINED_MOUSE_BUTTON, 10, (float)T_HOOK_MIN_Y);
	check("mouse exactly on the top row boundary is accepted",
		nearly(p_hook->getPosition().getY(), (float)T_HOOK_MIN_Y));

	// edge case - unrelated events are not handled
	df::EventOut out;
	check("OUT event is not handled (returns 0)", p_hook->eventHandler(&out) == 0);

	// edge case - other keys / key releases do not leave the game
	pressKey(p_hook, df::Keyboard::A);
	pressKey(p_hook, df::Keyboard::Q, df::KEY_RELEASED);
	sendStep(p_hook, 3);
	WM.update();
	check("a non-Q key and a Q *release* do not leave the game",
		countOfType("TitleScreen") == 0 && countOfType("Hook") == 1);
	p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));

	// error condition - a collision with nothing must not crash
	sendCollision(p_hook, NULL);
	df::EventCollision null_ev(NULL, NULL, df::Vector(0, 0));
	p_hook->eventHandler(&null_ev);
	check("collision events with NULL objects do not crash or cost a life",
		p_hook->getLives() == T_START_LIVES);

	// error condition - a collision with something that is not fish/junk
	Boat* p_boat = new Boat();
	sendCollision(p_hook, p_boat);
	check("collision with a Boat is ignored", p_hook->getLives() == T_START_LIVES);

	// happy path - junk costs exactly one life and updates the display
	clearWorld();
	p_hook = new Hook();
	Lives* p_lives = new Lives();
	Junk* p_junk = new Junk();
	sendCollision(p_hook, p_junk);
	check("hitting junk costs a life", p_hook->getLives() == 2);
	check("Lives display drops to match", p_lives->getValue() == 2);

	// edge case - the same can only cost one life; swapped argument order
	sendCollision(p_hook, p_junk);
	check("hitting the same junk twice only costs one life", p_hook->getLives() == 2);
	Junk* p_junk2 = new Junk();
	df::EventCollision swapped(p_junk2, p_hook, p_hook->getPosition());
	p_hook->eventHandler(&swapped);
	check("collision works when the hook is object2",
		p_hook->getLives() == 1 && p_lives->getValue() == 1);

	// happy path - catching a fish: fish rides on the hook
	clearWorld();
	p_hook = new Hook();
	Points* p_points = new Points();
	Fish* p_fish = new Fish();
	Fish* p_fish2 = new Fish();
	df::Vector fish2_before = p_fish2->getPosition();
	sendCollision(p_hook, p_fish);
	sendStep(p_hook);
	check("a hooked fish follows the hook",
		p_fish->getPosition().getX() == p_hook->getPosition().getX() &&
		p_fish->getPosition().getY() == p_hook->getPosition().getY());

	// edge case - only one fish can be held at a time
	sendCollision(p_hook, p_fish2);
	sendStep(p_hook);
	check("a second fish touched while holding one is not picked up",
		p_fish2->getPosition().getX() == fish2_before.getX() &&
		p_fish2->getPosition().getY() == fish2_before.getY());

	// edge case - the fish only scores when brought to the top
	sendMouse(p_hook, df::MOVED, df::Mouse::UNDEFINED_MOUSE_BUTTON, 10, 15);
	p_hook->fishCaught();
	check("fishCaught() below the top row scores nothing", p_points->getValue() == 0);
	sendMouse(p_hook, df::MOVED, df::Mouse::UNDEFINED_MOUSE_BUTTON, 10, 0);
	p_hook->fishCaught();
	check("fishCaught() at the top row scores 5", p_points->getValue() == T_FISH_POINTS);

	// edge case - the hook lets go after a catch, nothing more to score
	p_hook->fishCaught();
	check("fishCaught() again with nothing held scores nothing",
		p_points->getValue() == T_FISH_POINTS);

	// happy path - clicking the left button at the top also lands the fish
	clearWorld();
	p_hook = new Hook();
	p_points = new Points();
	p_fish = new Fish();
	sendCollision(p_hook, p_fish);
	sendMouse(p_hook, df::CLICKED, df::Mouse::LEFT, 10, 0);
	check("left click at the top row lands the held fish",
		p_points->getValue() == T_FISH_POINTS);

	// error condition - fishCaught() with no fish held
	clearWorld();
	p_hook = new Hook();
	p_points = new Points();
	p_hook->fishCaught();
	check("fishCaught() with no fish held scores nothing and does not crash",
		p_points->getValue() == 0);

	// error condition - 'hook' sprite never loaded: no crash
	clearWorld();
	RM.unloadSprite("hook");
	Hook* p_nosprite = new Hook();
	check("Hook with no 'hook' sprite loaded still constructs",
		p_nosprite->getType() == "Hook" && p_nosprite->getLives() == T_START_LIVES);
	loadSprite("hook");

	clearWorld();
}

// ======================================================================
// Hook - game over
// ======================================================================
static void testHookGameOver() {
	printHeader("Hook - game over and leaving");
	clearWorld();

	// happy path - three junk hits end the game after the countdown
	Hook* p_hook = new Hook();
	Lives* p_lives = new Lives();
	Junk* j1 = new Junk();
	Junk* j2 = new Junk();
	Junk* j3 = new Junk();
	sendCollision(p_hook, j1);
	sendCollision(p_hook, j2);
	check("2 hits: still 1 life left", p_hook->getLives() == 1);
	sendCollision(p_hook, j3);
	check("3 hits: 0 lives left", p_hook->getLives() == 0);
	check("Lives display shows 0", p_lives->getValue() == 0);
	check("GAME OVER does not leave instantly", countOfType("TitleScreen") == 0);

	// edge case - the GAME OVER message stays for the full countdown
	sendStep(p_hook, T_GAME_OVER_STEPS - 1);
	check("still showing GAME OVER one step before the countdown ends",
		countOfType("TitleScreen") == 0);
	sendStep(p_hook);
	check("title screen appears when the countdown reaches 0",
		countOfType("TitleScreen") == 1);
	WM.update();
	check("hook is removed after the countdown", countOfType("Hook") == 0);

	// edge case - after game over the hook ignores the mouse
	clearWorld();
	p_hook = new Hook();
	new Lives();
	for (int i = 0; i < T_START_LIVES; i++)
		sendCollision(p_hook, new Junk());
	float y_before = p_hook->getPosition().getY();
	sendMouse(p_hook, df::MOVED, df::Mouse::UNDEFINED_MOUSE_BUTTON, 10, 15);
	check("mouse is ignored once the game is over",
		nearly(p_hook->getPosition().getY(), y_before));

	// error condition - extra junk after game over costs no more lives
	sendCollision(p_hook, new Junk());
	check("junk hit after game over does not go below 0 lives",
		p_hook->getLives() == 0);

	// happy path - Q leaves on the next step
	clearWorld();
	p_hook = new Hook();
	pressKey(p_hook, df::Keyboard::Q);
	check("Q alone does not remove anything until the next step",
		countOfType("TitleScreen") == 0);
	sendStep(p_hook);
	check("Q then step shows the title screen", countOfType("TitleScreen") == 1);

	// edge case - returning to the title only ever happens once
	sendStep(p_hook, 5);
	check("extra steps after leaving do not create more title screens",
		countOfType("TitleScreen") == 1);
	WM.update();

	// error condition - after Q, collisions must not cost lives
	clearWorld();
	p_hook = new Hook();
	pressKey(p_hook, df::Keyboard::Q);
	sendCollision(p_hook, new Junk());
	check("junk hit after Q does not cost a life",
		p_hook->getLives() == T_START_LIVES);

	clearWorld();
}

// ======================================================================
// TitleScreen
// ======================================================================
static void testTitleScreen() {
	printHeader("TitleScreen");
	clearWorld();

	// happy path - constructor
	TitleScreen* p_title = new TitleScreen();
	check("type is \"TitleScreen\"", p_title->getType() == "TitleScreen");
	check("altitude is MAX_ALTITUDE (drawn on top)",
		p_title->getAltitude() == df::MAX_ALTITUDE);
	check("centered in the world",
		nearly(p_title->getPosition().getX(), WM.getBoundary().getHorizontal() / 2.0f) &&
		nearly(p_title->getPosition().getY(), WM.getBoundary().getVertical() / 2.0f));
	check("registered for keyboard and step events", p_title->getEventCount() == 2);
	check("draw() returns 0", p_title->draw() == 0);

	// happy path - step events are handled
	df::EventStep step;
	check("step event is handled (returns 1)", p_title->eventHandler(&step) == 1);

	// edge case - draw() still fine after the blink prompt has toggled
	sendStep(p_title, 40);
	check("draw() returns 0 after the prompt has blinked", p_title->draw() == 0);

	// edge case - unrelated events are not handled
	df::EventOut out;
	check("OUT event is not handled (returns 0)", p_title->eventHandler(&out) == 0);

	// edge case - other keys and key *releases* do not start the game
	pressKey(p_title, df::Keyboard::A);
	pressKey(p_title, df::Keyboard::SPACE, df::KEY_RELEASED);
	WM.update();
	check("a random key and a SPACE release do not start the game",
		countOfType("Hook") == 0 && countOfType("Fish") == 0 &&
		countOfType("TitleScreen") == 1);

	// happy path - Q on the title screen quits the whole program
	GM.setGameOver(false);
	pressKey(p_title, df::Keyboard::Q);
	check("Q on the title screen sets game over", GM.getGameOver());
	GM.setGameOver(false);   // don't let the test suite itself quit

	// happy path - SPACE starts the game
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
	verifyFreshGame("SPACE starts a game");

	// error condition - game still starts if resources are not loaded
	clearWorld();
	RM.unloadSprite("fish");
	RM.unloadSprite("junk");
	RM.unloadSprite("hook");
	RM.unloadSprite("boat");
	RM.unloadMusic("background-music");
	p_title = new TitleScreen();
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
	check("SPACE with no sprites/music loaded still creates the game objects",
		countOfType("Hook") == 1 && countOfType("Fish") == T_START_FISH &&
		countOfType("Junk") == T_START_JUNK);
	clearWorld();
	loadGameResources();

	// edge case - SPACE pressed twice before the title is deleted must not
	// start two games on top of each other
	TitleScreen* p_title2 = new TitleScreen();
	pressKey(p_title2, df::Keyboard::SPACE);
	pressKey(p_title2, df::Keyboard::SPACE);
	WM.update();
	check("double SPACE in one frame creates only 1 Hook",
		countOfType("Hook") == 1);
	check("double SPACE in one frame creates only the starting fish",
		countOfType("Fish") == T_START_FISH);
	check("double SPACE in one frame creates only 1 Points display",
		countViewObjects(POINTS_STRING) == 1);

	clearWorld();
}

// ======================================================================
// Restart - exiting with Q, then playing again
// ======================================================================
static void testRestartAfterQuit() {
	printHeader("Restart - quit with Q, then play again");

	// first game, played a bit: score some points, lose a life, hold a fish
	startNewGame();
	verifyFreshGame("game 1 start");
	int objects_in_fresh_game = WM.getAllObjects(true).getCount();

	Hook* p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	sendCollision(p_hook, firstOfType("Fish"));
	sendStep(p_hook);
	p_hook->fishCaught();                       // +5 points
	sendCollision(p_hook, firstOfType("Junk")); // -1 life
	sendCollision(p_hook, firstOfType("Fish")); // now holding another fish
	check("game 1: score is 5", sumViewValues(POINTS_STRING) == T_FISH_POINTS);
	check("game 1: lives shown is 2", sumViewValues(LIVES_STRING) == T_START_LIVES - 1);

	// leave the game
	quitToTitle();
	verifyOnlyTitleScreen("after Q");

	// restart - everything must look brand new (nothing continued)
	TitleScreen* p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	if (p_title != NULL) {
		pressKey(p_title, df::Keyboard::SPACE);
		WM.update();
	}
	verifyFreshGame("game 2 start");
	check("game 2 has the same number of objects as game 1",
		WM.getAllObjects(true).getCount() == objects_in_fresh_game);

	// a fresh hook must not remember the fish held in game 1
	p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	if (p_hook != NULL) {
		int score_before = sumViewValues(POINTS_STRING);
		p_hook->fishCaught();
		check("game 2: new hook is not holding a fish from game 1",
			sumViewValues(POINTS_STRING) == score_before);
	}

	// edge case - a game that is left immediately, before anything happens
	clearWorld();
	startNewGame();
	quitToTitle();
	verifyOnlyTitleScreen("quit immediately after starting");

	// edge case - repeat the quit/restart cycle several times; nothing
	// should pile up from one cycle to the next
	startNewGame();
	bool stable = true;
	for (int cycle = 0; cycle < 5; cycle++) {
		quitToTitle();
		TitleScreen* p_t = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
		if (p_t == NULL) { stable = false; break; }
		pressKey(p_t, df::Keyboard::SPACE);
		WM.update();
		if (WM.getAllObjects(true).getCount() != objects_in_fresh_game)
			stable = false;
	}
	check("after 5 quit/restart cycles the object count never grows", stable);

	// error condition - Q pressed twice in a row must not break the restart
	clearWorld();
	startNewGame();
	p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	pressKey(p_hook, df::Keyboard::Q);
	pressKey(p_hook, df::Keyboard::Q);
	sendStep(p_hook);
	sendStep(p_hook);
	WM.update();
	check("double Q still leaves exactly 1 TitleScreen",
		countOfType("TitleScreen") == 1);

	clearWorld();
}

// ======================================================================
// Restart - game over, then play again
// ======================================================================
static void testRestartAfterGameOver() {
	printHeader("Restart - game over, then play again");

	startNewGame();
	int objects_in_fresh_game = WM.getAllObjects(true).getCount();

	// earn some points, then lose every life
	Hook* p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	sendCollision(p_hook, firstOfType("Fish"));
	sendStep(p_hook);
	p_hook->fishCaught();
	sendCollision(p_hook, firstOfType("Fish"));    // holding a fish at game over
	loseAllLivesToTitle();
	verifyOnlyTitleScreen("after game over");

	// restart
	TitleScreen* p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	if (p_title != NULL) {
		pressKey(p_title, df::Keyboard::SPACE);
		WM.update();
	}
	verifyFreshGame("game after game over");
	check("restarted game has the same number of objects as the first",
		WM.getAllObjects(true).getCount() == objects_in_fresh_game);

	// the new game must be playable right through to a second game over
	loseAllLivesToTitle();
	verifyOnlyTitleScreen("after a second game over");

	// edge case - game over, restart, quit, restart
	p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	if (p_title != NULL) {
		pressKey(p_title, df::Keyboard::SPACE);
		WM.update();
	}
	quitToTitle();
	p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	if (p_title != NULL) {
		pressKey(p_title, df::Keyboard::SPACE);
		WM.update();
	}
	verifyFreshGame("game over -> quit -> restart");

	// error condition - Junk hits during the game-over countdown must not
	// restart or extend it
	clearWorld();
	startNewGame();
	p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	for (int i = 0; i < T_START_LIVES; i++)
		sendCollision(p_hook, new Junk());
	sendStep(p_hook, 10);
	sendCollision(p_hook, new Junk());
	sendStep(p_hook, T_GAME_OVER_STEPS - 10);
	check("junk hit during the countdown does not extend the countdown",
		countOfType("TitleScreen") == 1);

	clearWorld();
}

// ======================================================================
// Restart - score and lives displays
// ======================================================================
static void testRestartScoreAndLives() {
	printHeader("Restart - score and lives do not carry over");

	startNewGame();
	Hook* p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));

	// score 15 points and lose 2 lives in game 1
	for (int i = 0; i < 3; i++) {
		sendCollision(p_hook, firstOfType("Fish"));
		sendStep(p_hook);
		p_hook->fishCaught();
	}
	sendCollision(p_hook, new Junk());
	sendCollision(p_hook, new Junk());
	check("game 1: score shown is 15", sumViewValues(POINTS_STRING) == 15);
	check("game 1: lives shown is 1", sumViewValues(LIVES_STRING) == 1);

	quitToTitle();
	TitleScreen* p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();

	// happy path - the new game starts from scratch
	check("game 2: total score shown starts at 0",
		sumViewValues(POINTS_STRING) == 0);
	check("game 2: total lives shown starts at 3",
		sumViewValues(LIVES_STRING) == T_START_LIVES);

	// edge case - points earned in game 2 are added to 0, not to game 1's 15
	p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	sendCollision(p_hook, firstOfType("Fish"));
	sendStep(p_hook);
	p_hook->fishCaught();
	check("game 2: first catch brings the score to 5, not 20",
		sumViewValues(POINTS_STRING) == T_FISH_POINTS);

	// edge case - a life lost in game 2 shows 2, not (game 1's 1) - 1
	sendCollision(p_hook, firstOfType("Junk"));
	check("game 2: first junk hit shows 2 lives",
		sumViewValues(LIVES_STRING) == T_START_LIVES - 1);
	check("game 2: hook agrees with the display",
		p_hook->getLives() == T_START_LIVES - 1);

	// error condition - score earned just before game over must not leak
	// into the next game
	loseAllLivesToTitle();
	p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
	check("game 3 (after game over): total score shown starts at 0",
		sumViewValues(POINTS_STRING) == 0);

	clearWorld();
}

// ======================================================================
// Restart - junk difficulty ramp
// ======================================================================
static void testRestartJunkDifficulty() {
	printHeader("Restart - junk difficulty ramp resets");

	startNewGame();
	Junk::resetPassCount();     // known baseline: no junk has passed yet

	// game 1: exactly ONE junk drifts off the right edge (1 pass, no spawn)
	df::Object* p_junk = firstOfType("Junk");
	p_junk->setPosition(df::Vector(10, 10));
	sendOut(p_junk);
	check("game 1: one junk passes, no extra junk yet",
		countOfType("Junk") == T_START_JUNK);

	quitToTitle();
	TitleScreen* p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();

	// game 2: the first junk to pass should again be the 1st pass of a
	// new game (no extra junk). If the pass count carried over from game 1
	// it would be the 2nd pass and an extra junk would spawn immediately.
	check("game 2: starts with the starting amount of junk",
		countOfType("Junk") == T_START_JUNK);
	p_junk = firstOfType("Junk");
	p_junk->setPosition(df::Vector(10, 10));
	sendOut(p_junk);
	check("game 2: first junk to pass does not spawn extra junk (ramp reset)",
		countOfType("Junk") == T_START_JUNK);

	// edge case - the junk count itself does not carry over
	quitToTitle();
	check("after leaving, no junk remains to count towards the cap",
		countOfType("Junk") == 0);

	// error condition - a game that filled up to the junk cap must start
	// the next game back at the starting amount
	clearWorld();
	startNewGame();
	for (int i = 0; i < T_JUNK_MAX_COUNT; i++)
		new Junk();
	quitToTitle();
	p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
	check("game after a full-junk game starts with only the starting junk",
		countOfType("Junk") == T_START_JUNK);

	clearWorld();
}

// ======================================================================
// Restart - music
// ======================================================================
static void testRestartMusic() {
	printHeader("Restart - background music");

	df::Music* p_music = RM.getMusic("background-music");
	if (p_music == NULL) {
		skip("music restart checks", "background-music.wav was not loaded");
		return;
	}

	// find out whether this machine can actually play audio
	p_music->play(true);
	bool audio_ok = musicPlaying();
	p_music->stop();
	if (!audio_ok) {
		skip("music restart checks", "no audio device available");
		return;
	}

	// happy path - music plays during the game, stops on leaving
	startNewGame();
	check("music is playing during the game", musicPlaying());
	quitToTitle();
	check("music stops after quitting with Q", !musicPlaying());

	// restart - music plays again
	TitleScreen* p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
	check("music plays again in the restarted game", musicPlaying());

	// edge case - music stops as soon as the game is over, before the
	// GAME OVER message finishes
	Hook* p_hook = dynamic_cast<Hook*>(firstOfType("Hook"));
	for (int i = 0; i < T_START_LIVES; i++)
		sendCollision(p_hook, new Junk());
	check("music stops at game over", !musicPlaying());
	sendStep(p_hook, T_GAME_OVER_STEPS);
	WM.update();
	check("music still stopped on the title screen after game over",
		!musicPlaying());

	// error condition - leaving quickly must not leave music running
	p_title = dynamic_cast<TitleScreen*>(firstOfType("TitleScreen"));
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
	quitToTitle();
	check("music stops when leaving immediately after starting", !musicPlaying());

	clearWorld();
}

// ======================================================================
// Restart - whole engine (GameManager) shut down and started again
// ======================================================================
static void testRestartEngine() {
	printHeader("Restart - GameManager shut down and started again");

	// get the game into a "used" state: objects exist, loop has run, game
	// over was set to end the run
	startNewGame();
	GM.setGameOver(false);
	std::thread stopper([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		GM.setGameOver(true);
		});
	GM.run();
	stopper.join();
	check("before restart: loop ran (step count > 0)", GM.getStepCount() > 0);
	check("before restart: game over flag is set", GM.getGameOver());
	check("before restart: game objects exist", countOfType("Hook") == 1);

	// happy path - shut everything down and start up again
	GM.shutDown();
	check("isStarted() false after shutDown()", !GM.isStarted());
	int result = GM.startUp();
	check("startUp() after shutDown() returns 0", result == 0);
	if (result != 0)
		return;

	check("after restart: game over flag is cleared", !GM.getGameOver());
	if (GM.getStepCount() != 0)
		printf("  [NOTE] engine keeps its step count (%d) across a restart; "
			"the game code never reads it, so this is not checked\n",
			GM.getStepCount());
	check("after restart: world is empty", WM.getAllObjects(true).getCount() == 0);
	check("after restart: no Hook carried over", countOfType("Hook") == 0);
	check("after restart: no Fish carried over", countOfType("Fish") == 0);
	check("after restart: no Junk carried over", countOfType("Junk") == 0);
	check("after restart: no Points/Lives displays carried over",
		countViewObjects(POINTS_STRING) == 0 && countViewObjects(LIVES_STRING) == 0);

	// edge case - after the restart a full game can be played from the title
	loadGameResources();
	TitleScreen* p_title = new TitleScreen();
	pressKey(p_title, df::Keyboard::SPACE);
	WM.update();
	verifyFreshGame("game after engine restart");

	// error condition - the first loop after a restart runs cleanly
	GM.setGameOver(false);
	std::thread stopper2([]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		GM.setGameOver(true);
		});
	bool threw = false;
	try {
		GM.run();
	}
	catch (...) {
		threw = true;
	}
	stopper2.join();
	check("running the loop after an engine restart does not throw", !threw);
	check("loop after an engine restart advances the step count",
		GM.getStepCount() > 0);

	clearWorld();
}

// ======================================================================
// main
// ======================================================================
/*
int main(int argc, char* argv[]) {
	printf("Dragonfly Fishing Test Suite\n");
	printf("============================\n");

	// The game classes need a running engine (window, world, resources).
	if (GM.startUp() != 0) {
		printf("Could not start the game engine (is a display available?)\n");
		return 2;
	}
	loadGameResources();

	testBoat();
	testWaterline();
	testFish();
	testJunk();
	testLives();
	testPoints();
	testHook();
	testHookGameOver();
	testTitleScreen();
	testRestartAfterQuit();
	testRestartAfterGameOver();
	testRestartScoreAndLives();
	testRestartJunkDifficulty();
	testRestartMusic();
	testRestartEngine();    // keep last: shuts the engine down and restarts it

	GM.shutDown();

	printf("\n============================\n");
	printf("%d / %d checks passed.\n", g_tests_passed, g_tests_run);

	return (g_tests_passed == g_tests_run) ? 0 : 1;
} */