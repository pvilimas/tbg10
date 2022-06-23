#include "raylib.h"
#include "textbasedgame.hpp"

/*
    TODO:

    - impl NPC
    - hidden dialogoptions

    current assumptions:

    - inv starts empty

    - later: graphics scaling
*/

int main() {
    
    // for convenience
    SetTraceLogLevel(LOG_WARNING);
    // needed to resolve asset filepaths
    // maybe GetAppDir() instead or SearchAndSetResourceDir() - test later
    ChangeDirectory(GetApplicationDirectory());
    
    TextBasedGame tbg = TextBasedGame();
    try {
        tbg.run();
    } catch (TextBasedGame::ExitGameException e) {}

    return 0;
}