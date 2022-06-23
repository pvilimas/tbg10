#include "raylib.h"
#include "textbasedgame.hpp"

/*
    TODO:

    - document all existing code
    - standardize anything that needs it
    - removed unused stuff
    
    - make branches for
        - more lines of text
        - graphics scaling
        - implementing NPCs
    in that order

    - other small changes
        - hidden dialogoptions

    current assumptions:
    - inv starts empty
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