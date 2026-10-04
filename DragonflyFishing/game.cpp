//Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"

//Game includes
#include "Fish.h"
#include "Junk.h"
#include "Hook.h"

//prototypes
void loadResources(void);
void populateWorld(void);

int main(int argc, char* argv[]) {

    //Start up game manager.
    if (GM.startUp()) {
        LM.writeLog("Error starting game manager!");
        GM.shutDown();
        return 1;
    }

    //Set flush of logfile during development (when done, make false).
    LM.setFlush(true);

    //Show splash screen.
    df::splash();

    //Load game resources.
    loadResources();

    //Populate game world with some objects.
    populateWorld();

    //run the game
    GM.run();

    //Shut everything down.
    GM.shutDown();
    return 0;
}


//Load all sprites 
void loadResources() {
    RM.loadSprite("sprites/fish-spr.txt", "fish");
    RM.loadSprite("sprites/junk-spr.txt", "junk");
    RM.loadSprite("sprites/hook-spr.txt", "hook");
}

//Create the starting objects
void populateWorld() {
    new Hook();
    new Junk();
    for (int i = 0; i < 6; i++)
        new Fish();
}