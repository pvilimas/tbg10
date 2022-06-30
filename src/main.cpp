#include "raylib/raylib.h"
#include "textbasedgame.hpp"

/*
    TODO:

    - remove unused stuff
    - deal with any other TODOs
    
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
    
    /*
        for convenience - in the final app, probably want LOG_NONE
        or figure out how to launch without terminal then no problem
    */
    SetTraceLogLevel(LOG_WARNING);

    /*
        needed to resolve asset filepaths
        maybe GetAppDir() instead or SearchAndSetResourceDir() - test later
    */
    ChangeDirectory(GetApplicationDirectory());
    
    TextBasedGame tbg = TextBasedGame();
    try {
        tbg.Run();
    } catch (TextBasedGame::ExitGameException e) {}

    return 0;
}