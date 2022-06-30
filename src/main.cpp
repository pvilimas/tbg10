#include "raylib/raylib.h"
#include "textbasedgame.hpp"

/*
    TODO:

    - clean up code
        - standardize anything that needs it
            - var naming: styleguide
            - make sure static stuff is accessed statically (qualnames)
            - use references where needed (esp params)
                - not std::string params tho, not if you want to pass them as const char*
            - .at() instead of [] where necessary
            - organize imports (alphabetically):
                <std1>
                <std2>
                <std3>

                "raylib"
                #define FMT_SHIT
                "fmt"

                "./local1"
                "./local2"
                "./local3"

            - make multiple passes through for each of these
    - removed unused stuff
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