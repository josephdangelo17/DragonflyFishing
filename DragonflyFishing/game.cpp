//Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"

//Game includes
#include "Fish.h"

//prototypes
void loadResources(void);
void populateWorld(void);

int main(int argc, char* argv[]) {

    // Start up game manager.
    if (GM.startUp()) {
        LM.writeLog("Error starting game manager!");
        GM.shutDown();
        return 1;
    }

    // Set flush of logfile during development (when done, make false).
    LM.setFlush(true);

    // Show splash screen.
    df::splash();

    // Load game resources.
    loadResources();

    // Populate game world with some objects.
    populateWorld();

    //run the game
    GM.run();

    // Shut everything down.
    GM.shutDown();
    return 0;
}


// Load all sprites (labels must match what the classes use).
void loadResources() {
    RM.loadSprite("sprites/fish-spr.txt", "fish");
}

// Create the starting fish.
void populateWorld() {
    for (int i = 0; i < 6; i++)
        new Fish();
}