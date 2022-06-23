#ifndef __GRAPHICS__
#define __GRAPHICS__

#include <iostream>
#include <queue>
#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "assetmanager.hpp"

/*
    TODO
    - keep AssetManager::fontSize (used for loading fonts) and Graphics::fontSize (for drawing) the same
        - do this by passing in fontSize to AM::AM
        - if needed in future, make AssetManagerConfig for more args but not necessary prob
    - check if need to do the same thing for spacing - does loadfontex use fontSpacing?
*/

/*
    Handles drawing everything to the screen, events, graphics management, specifically:
    - Lines of game text ("textOut") and player input ("textIn")
    - Game images (for each room)
    - Player input cursor
    - Text scrolling (idk what to call it, uses the queue)
    - "Dumping" the rest of the queue onto the screen when the user hits ENTER
    - Displaying hints, and autofilling them if/when user hits TAB

    - Rescaling window to match resolution (in future)
    Only one of these will be running at a time, so static stuff is ok (static local vars and such)
*/
class Graphics {
    public:

    /*
        Different text speeds to display textOut at
        can be set either with s/m/f or 1/2/3
    */
    enum class TextSpeed : uint8_t {
        Slow = 1,
        Medium = 2,
        Fast = 3,
        Default = Medium
    };

    /*
        Different cursor styles for the UI
        names not visible to player, all they get is 1/2/3/4
        cursor color (or at least color scheme) remains the same
    */
    enum class CursorStyle {
        VerticalBar = 1,
        Underline = 2,
        OutlineBox = 3,
        TransparentBox = 4,
        Default = VerticalBar
    };

    /*
        Arbitrarily chosen resolution types
    */
    enum class Resolution {
        Low1080p = 1,
        High4k = 2,
        Default = Low1080p
    };

    private:

    /* manages all the images and fonts */
    AssetManager assets;
    /* current size of the window, xy */
    Vector2 winSize;
    /* what the player currently has typed in their box */
    std::vector<char> textIn;
    /* the 4 lines of game text currently displayed on screen */
    std::vector<char> textOut[4];
    /*
        the current hint being displayed
        when the user hits tab, this will be appended to textIn
    */
    std::string textInHint;
    /* name of the image we are currently drawing, within assetmanager */
    std::string currImage;
    /* used to draw screen, and allows for scaling if needed later */
    RenderTexture2D renderTexture;
    /* controls screen size based on DPI - currently unused */
    Resolution resolutionScaleFactor;

    /* manages text scrolling, 4 lines */
    std::vector<std::queue<char>> textOutScroll;
    /* used to control scroll speed of text and cursor blink, and anything else later that is done every n frames */
    int frameCount;
    /* if true, dump the rest of the queue next frame (happens when user hits ENTER while queue is not empty) */
    bool purgeQueueNextFrame;
    /*
        1 -> 1 char every 6 frames (S or slow)
        2 -> 2 char every 6 frames (M or med)
        3 -> 3 char every 6 frames (F or fast)
    */
    TextSpeed textScrollSpeed;
    /*
        1 = vertical bar
        2 = underline
        3 = outline box
        4 = transparent box
    */
    CursorStyle cursorStyle;

    public:

    /* window title - invisible with FLAG_WINDOW_UNDECORATED btw */
    static inline constexpr const char* titleText = "Textbasedgame";
    /*
        prompt to display before user input, should always be "> "
        the length is most likely accounted for everywhere (prob no risk of segfault if diff len)
    */
    static inline constexpr const char* prompt = "> ";

    /* default and initial window width */
    static inline constexpr int defaultWinWidth = 644;
    /* default and initial window height */
    static inline constexpr int defaultWinHeight = 506;

    /*
        how many characters the player can type in their box
        if len(textIn) = this, then any keyinput besides BACKSPACE will be ignored
        not including "> " which makes it 65
    */
    static inline constexpr int lineInLimit = 63;
    /*
        how many characters can go on each line of game text
        not sure about overflow/wrapping (drawing 66 characters)
        don't try that pls, make sure all messages do not overflow
    */
    static inline constexpr int lineOutLimit = 65;
    /* how many lines of game text there are */
    static inline constexpr int lineOutCount = 4;

    /* the color of the frame/border */
    static inline constexpr Color frameColor = Color {0xAA, 0xAA, 0xAA, 255};
    /* the line thickness of the frame/border */
    static inline constexpr int frameThick = 2;
    /*
        the font size used to draw fonts - should be the same as AssetManager::fontSize,
        which isn't linked to this. that is used to load them, so they should be the same.
    */
    static inline constexpr int fontSize = 20;
    /*
        the font spacing used to draw fonts - see above/todos
    */
    static inline constexpr float fontSpacing = 0.6;

    static std::string makeStr(std::vector<char> v);

    Graphics();
    ~Graphics();
    
    void normalizeWindowSize();

    void draw();
    void drawCursor();

    std::string getTextIn();
    std::string getTextOut(int line);
    
    void setTextIn(std::string s);
    void setTextOut(std::string s, int line);
    void setHint(std::string s);
    void addHintToInput();
    
    void addCharIn(char c);
    void delCharIn();

    void changeTextSpeed(TextSpeed newSpeed);
    void changeCursorStyle(CursorStyle newStyle);

    bool queueEmpty();
    // see above
    void dumpText();

    void setImage(std::string newImageName);

};

#endif /* __GRAPHICS__ */