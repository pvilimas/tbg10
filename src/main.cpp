#include "raylib/raylib.h"
#include "textbasedgame.hpp"

/*
    TODO:

    - deal with any other TODOs
    
    - make branches for
        - more lines of text
        - graphics scaling
        - implementing NPCs
    in that order

    - other small changes
        - hidden dialogoptions

    current assumptions:
    - inv starts empty - keep it that way
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
    tbg.Init();

    try {
        tbg.Run();
    } catch (TextBasedGame::ExitGameException e) {
        // call ~tbg()
    }

    return 0;
}