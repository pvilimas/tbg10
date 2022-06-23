#ifndef __GRAPHICS__
#define __GRAPHICS__

#include <iostream>
#include <queue>
#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "assetmanager.hpp"

class Graphics {
    public:

    enum class TextSpeed : uint8_t {
        Slow = 1,
        Medium = 2,
        Fast = 3,
        Default = Medium
    };

    // names not visible to player, all they get is 1234
    // color (or at least color scheme) remains the same
    enum class CursorStyle {
        VerticalBar = 1,
        Underline = 2,
        OutlineBox = 3,
        TransparentBox = 4,
        Default = VerticalBar
    };

    enum class Resolution {
        Low1080p = 1,
        High4k = 2,
        Default = Low1080p
    };

    private:

    AssetManager assets;
    Vector2 winSize;
    std::vector<char> textIn;
    std::vector<char> textOut[4];
    std::string textInHint;
    // image we are currently drawing, within assetmanager
    std::string currImage;
    // used to draw screen
    RenderTexture2D renderTexture;
    // controls screen size based on DPI
    Resolution resolutionScaleFactor;

    // manages text scrolling
    std::vector<std::queue<char>> textOutScroll;
    // used to control scroll speed of text and cursor blink
    int frameCount;
    // if true, clear the queue next frame (if user hit enter with no text in the box while queue is nonempty)
    bool purgeQueueNextFrame;
    // 1 2 or 3
    // 1 -> 1 char every 6 frames (S or slow)
    // 2 -> 2 char every 6 frames (M or med)
    // 3 -> 3 char every 6 frames (F or fast)
    TextSpeed textScrollSpeed;
    // 1 = vertical bar
    // 2 = underline
    // 3 = outline box
    // 4 = transparent box
    CursorStyle cursorStyle;

    public:

    static inline constexpr const char* titleText = "Textbasedgame";
    static inline constexpr const char* prompt = "> ";

    static inline constexpr int defaultWinWidth = 644;
    static inline constexpr int defaultWinHeight = 506;

    static inline constexpr int lineInLimit = 63; // not including "> " which makes it 65
    static inline constexpr int lineOutLimit = 65; // per line
    static inline constexpr int lineOutCount = 4;

    static inline constexpr Color frameColor = Color {0xAA, 0xAA, 0xAA, 255};
    static inline constexpr int frameThick = 2;
    static inline constexpr int fontSize = 20;
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