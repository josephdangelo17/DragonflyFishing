//
// game.cpp
// 

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"

int main(int argc, char *argv[]) {

  // Start up game manager.
  if (GM.startUp() != 0)  {
    LM.writeLog("Error starting game manager!");
    return -1;
  }

  // Show splash screen.
  df::splash();

  // Shut everything down.
  GM.shutDown();

  // All is well.
  return 0;
}
