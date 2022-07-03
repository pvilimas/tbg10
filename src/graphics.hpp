#ifndef __GRAPHICS__
#define __GRAPHICS__

#include <array>
#include <cctype>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "assetmanager.hpp"

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
    enum class TextSpeed {
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

     /*
        window title (invisible with FLAG_WINDOW_UNDECORATED btw)
        also actual title if WIN_UNDEC is used
    */
    static inline constexpr const char* TitleText = "Textbasedgame";
    
    /*
        prompt to display before user input, should always be "> "
        the length is most likely accounted for everywhere (prob no risk of segfault if diff len)
    */
    static inline constexpr const char* PlayerPrompt = "> ";

    /*  default and initial window width  */
    static inline constexpr int DefaultWinWidth = 644;

    /*  default and initial window height  */
    static inline constexpr int DefaultWinHeight = 550;

    /*
        how many characters the player can type in their box
        if len(textIn) = this, then any keyinput besides BACKSPACE will be ignored
        not including "> " which makes it 65
    */
    static inline constexpr size_t LineInLimit = 63;

    /*
        how many characters can go on each line of game text
        not sure about overflow/wrapping (drawing 66 characters)
        don't try that pls, make sure all messages do not overflow
    */
    static inline constexpr size_t LineOutLimit = 65;

    /*  how many lines of game text there are  */
    static inline constexpr size_t LineOutCount = 6;

    /*  the color of the frame/border  */
    static inline constexpr Color FrameColor = Color {0xAA, 0xAA, 0xAA, 255};

    /*  the line thickness of the frame/border  */
    static inline constexpr int FrameThick = 2;
    /*
        the font size used to draw fonts - should be the same as AssetManager::fontSize,
        which is used to load fonts
    */
    static inline constexpr int FontSize = AssetManager::FontSize; // 20
    
    /*
        the font spacing used to draw fonts - see above/todos
    */
    static inline constexpr float FontSpacing = 0.6;

    private:

    /*  manages all the images and fonts  */
    AssetManager assets;

    /*  current size of the window, xy  */
    Vector2 windowSize;
    
    /*  what the player currently has typed in their box  */
    std::vector<char> textIn;
    
    /*  the lines of game text currently displayed on screen  */
    std::array<std::vector<char>, Graphics::LineOutCount> textOut;
    
    /*
        the current hint being displayed
        when the user hits tab, this will be appended to textIn
    */
    std::string textInHint;

    /*  name of the image we are currently drawing, within assetmanager  */
    std::string currentImage;
    
    /*  used to draw screen, and allows for scaling if needed later  */
    RenderTexture2D renderTexture;
    
    /*  controls screen size based on DPI - currently unused  */
    Resolution resolutionScaleFactor;

    /*  manages text scrolling  */
    std::array<std::queue<char>, Graphics::LineOutCount> textOutScroll;
    
    /*  used to control scroll speed of text and cursor blink, and anything else later that is done every n frames  */
    int frameCount;
    
    /*  if true, dump the rest of the queue next frame (happens when user hits ENTER while queue is not empty)  */
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

    /*
        converts a vector of chars to a string
        thin wrapper around string(v.begin, v.end)
    */
    static std::string MakeStr(std::vector<char>& v);

    /*
        Graphics constructor, does the following:
        - Initializes window
        - Sets config flags, exit key, target FPS
        Maybe those 4 things ^ should be in their own method idk (Graphics::init)
        - Creates assetmanager, and loads:
        - texture for each room
        - normal font for textIn and hits
        - italic font for textOut
        - title font for title bar
    */
    Graphics();

    /*
        Graphics destructor - unloads the render texture, closes window
        Everything else is taken care of by assetmanager destructor tbh

    */
    ~Graphics();

    /*
        Currently unused (and useless)
        Scales the window size to the nearest integer multiple of (644, 506)
        based on screen resolution
    */
    void NormalizeWindowSize();


    /*
        Draw everything onto renderTexture and return it
    */
    RenderTexture& Render();

    /*
        draws everything to the screen, and some other stuff, including:
        - text scrolling/purge queue
        - counts frames (just adds 1 each time to the counter)
        - background
        - current texture
        - all 7 lines of text
        - cursor (w/ blinking)
        - rest of the UI

        future additions:
        - normalize window size + scaling render texture
    */
    void Draw();

    /*
        draws the cursor
        blinking is done with frameCount
        automatically updates based on the current cursor style
    */
    void DrawCursor();

    /*
        returns what the player has typed
        TBG calls this method when ENTER is pressed
    */
    std::string GetTextIn();
    
    /*  get the current game text on a specific line  */
    std::string GetTextOut(int line);
    
    /*  set the player's text to whatever  */
    void SetTextIn(std::string s);

    /*  set the game text on a specific line  */
    void SetTextOut(std::string s, int line);

    /*  set the player hint  */
    void SetHint(std::string s);
    
    /*
        append the rest of the hint to the player input, and clear the hint
        called when user hits tab
    */
    void AddHintToInput();
    
    
    /*  add a character to the player text  */
    void AddCharIn(char c);
    
    /*  remove a character from the end of player text  */
    void DelCharIn();
    
    /*  change the text speed setting  */
    void ChangeTextSpeed(TextSpeed newSpeed);
    
    /*  change the cursor style setting  */
    void ChangeCursorStyle(CursorStyle newStyle);

    /*
        is the text scroll queue empty?
        used to check if queue should be purged when user hits ENTER
    */
    bool IsQueueEmpty();

    /*
        on the next frame after this is called, purge the text scroll queue
        - see above
    */
    void DumpText();

    /*
        set the current image (parameter is whatever the image name is within assetmanager)
        updates next frame i think
    */
    void SetBackgroundImage(std::string newImageName);

};

#endif /* __GRAPHICS__ */