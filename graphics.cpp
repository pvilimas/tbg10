#include "graphics.hpp"

/*
    converts a vector of chars to a string
    thin wrapper around string(v.begin, v.end)
*/
std::string Graphics::makeStr(std::vector<char> v) {
    return std::string(v.begin(), v.end());
}

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
Graphics::Graphics() {

    /* init everything */
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(defaultWinWidth, defaultWinHeight, "textbasedgame");
    
    /* only way to exit is by typing exit/q */
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    /*
        create all necessary assets
        3 fonts:
        - normal font for textIn and hits
        - italic font for textOut
        - title font for title bar
        + texture for each room
    */

    assets = AssetManager();
    assets.CreateTexture("Garden", "assets/images/garden.png");
    assets.CreateTexture("Kitchen", "assets/images/kitchen.png");
    assets.CreateTexture("Bedroom", "assets/images/bedroom.png");
    assets.CreateFont("normal", "assets/fonts/SourceCodePro-SemiBold.ttf");
    assets.CreateFont("italic", "assets/fonts/SourceCodePro-MediumItalic.ttf");
    assets.CreateFont("title", "assets/fonts/SourceCodePro-BoldItalic.ttf");

    /* don't think this does anything with FLAG_WINDOW_UNDECORATED */
    // SetWindowIcon(LoadImageFromTexture(assets.GetTexture("Bedroom")));

    /*
        initialize textIn, textOut[], hint
    */

    textOut[0] = std::vector<char>();
    textOut[1] = std::vector<char>();
    textOut[2] = std::vector<char>();
    textOut[3] = std::vector<char>();
    textIn = std::vector<char>();
    textInHint = "";

    /*
        initialize text scroll queues
    */

    textOutScroll = std::vector<std::queue<char>>{
        std::queue<char>(),
        std::queue<char>(),
        std::queue<char>(),
        std::queue<char>(),
    };

    /* counts up from 0 */
    frameCount = 0;

    /* default settings */
    textScrollSpeed = TextSpeed::Default;
    cursorStyle = CursorStyle::Default;
    resolutionScaleFactor = Resolution::Default;

    /*
        initialize everything else
    */
    winSize = Vector2 { defaultWinWidth, defaultWinHeight };
    currImage = "Kitchen";
    renderTexture = LoadRenderTexture(winSize.x, winSize.y);
}

/*
    Graphics destructor - unloads the render texture, closes window
    Everything else is taken care of by assetmanager destructor tbh

*/
Graphics::~Graphics() {
    UnloadRenderTexture(renderTexture);
    CloseWindow();
}

/*
    Currently unused (and useless)
    Scales the window size to the nearest integer multiple of (644, 506)
    based on screen resolution
*/
void Graphics::normalizeWindowSize() {

    float targetW, targetH;
    float monitor = GetCurrentMonitor();
    float w = GetMonitorWidth(monitor);
    float h = GetMonitorHeight(monitor);
    std::cout << fmt::format("w={}, h={}", w, h) << std::endl;
    
    if (w != winSize.x || h != winSize.y) {
        // arbitrary as fuck
        if (w >= 2560 && h >= 1600) {
            resolutionScaleFactor = Resolution::High4k;
            targetW = defaultWinWidth * 2;
            targetH = defaultWinHeight * 2;
        } else {
            resolutionScaleFactor = Resolution::Low1080p;
            targetW = defaultWinWidth;
            targetH = defaultWinHeight;
        }

        if (targetW != winSize.x || targetH != winSize.y) {
            winSize = Vector2 { targetW, targetH };
            SetWindowSize(targetW, targetH);
            // SetWindowPosition(
            //   (w/2) - (winSize.x/2), 
            //   (h/2) - (winSize.y/2) 
            // );
            UnloadRenderTexture(renderTexture);
            renderTexture = LoadRenderTexture(winSize.x, winSize.y);
        }
    }

}

/*
    draws everything to the screen, and some other stuff, including:
    - text scrolling/purge queue
    - counts frames (just adds 1 each time to the counter)
    - background
    - current texture
    - all 5 lines of text
    - cursor (w/ blinking)
    - rest of the UI

    future additions:
    - normalize window size + scaling render texture
*/
void Graphics::draw() {

    // normalizeWindowSize();

    bool addCharThisFrame = false;
    
    switch(textScrollSpeed) {
        case TextSpeed::Slow : addCharThisFrame = (frameCount % 5 == 0); break; 
        case TextSpeed::Medium : addCharThisFrame = (frameCount % 3 == 0); break; 
        case TextSpeed::Fast : addCharThisFrame = true; break; 
    }

    BeginTextureMode(renderTexture);
    
    if (purgeQueueNextFrame) {
        for (int i = 0; i < Graphics::lineOutCount; i++) {
            while (!textOutScroll.at(i).empty()) {
                char c = textOutScroll.at(i).front();
                textOut[i].push_back(c);
                textOutScroll.at(i).pop();
            }
        }
        purgeQueueNextFrame = false;
    } else if (addCharThisFrame) {
        for (int i = 0; i < Graphics::lineOutCount; i++) {
            if (!textOutScroll.at(i).empty()) {
                char c = textOutScroll.at(i).front();
                textOut[i].push_back(c);
                textOutScroll.at(i).pop();
                break;
            }
        }
    }

    // bg
    ClearBackground(Color {0x22, 0x22, 0x22, 255});
    // input line bg
    DrawRectangleRec({0, 476, 644, 30}, Color {0x20, 0x20, 0x20, 255});

    // picture
    DrawTexture(assets.GetTexture(currImage.c_str()), 2, 24, WHITE);

    // title - centered
    //DrawTextEx(assets.GetFont("italic"), titleText, {(winSize.x - MeasureTextEx(assets.GetFont("italic"), titleText, fontSize, fontSpacing).x) / 2, 2}, fontSize, fontSpacing, LIGHTGRAY);
    // title not centered
    DrawTextEx(assets.GetFont("title"), titleText, {4, 2}, fontSize, fontSpacing, RAYWHITE);
    // output lines
    DrawTextEx(assets.GetFont("italic"), makeStr(textOut[0]).c_str(), {6, 388}, fontSize, fontSpacing, LIGHTGRAY);
    DrawTextEx(assets.GetFont("italic"), makeStr(textOut[1]).c_str(), {6, 410}, fontSize, fontSpacing, LIGHTGRAY);
    DrawTextEx(assets.GetFont("italic"), makeStr(textOut[2]).c_str(), {6, 432}, fontSize, fontSpacing, LIGHTGRAY);
    DrawTextEx(assets.GetFont("italic"), makeStr(textOut[3]).c_str(), {6, 454}, fontSize, fontSpacing, LIGHTGRAY);
    // input line
    DrawTextEx(assets.GetFont("normal"), ("> " + makeStr(textIn)).c_str(), {6, 480}, fontSize, fontSpacing, RAYWHITE);

    // hint
    DrawTextEx(assets.GetFont("normal"), (std::string(textIn.size() + 2, ' ') + textInHint).c_str(), {6, 480}, fontSize, fontSpacing, LIGHTGRAY);

    // title border
    DrawRectangleLinesEx(Rectangle {0, 0, 644, 24}, frameThick, frameColor);
    // picture border
    DrawRectangleLinesEx(Rectangle {0, 22, 644, 364}, frameThick, frameColor);
    // text border
    DrawRectangleLinesEx(Rectangle {0, 384, 644, 120}, frameThick, frameColor);
    // I/O separator
    DrawLineEx({0, 480}, {644, 480}, frameThick, frameColor);
    
    drawCursor();
    EndTextureMode();
    
    BeginDrawing();
    
    // negative width to flip it
    DrawTexturePro(
        renderTexture.texture,
        Rectangle { 0, 0, (float) (renderTexture.texture.width), -(float) (renderTexture.texture.height) },
        Rectangle {
            0, 0,
            ((int) resolutionScaleFactor) * (float) (renderTexture.texture.width),
            ((int) resolutionScaleFactor) * (float) (renderTexture.texture.height)
        },
        Vector2 { 0, 0 },
        0.0f,
        WHITE
    );
    EndDrawing();

    frameCount++;
}

/*
    draws the cursor
    blinking is done with frameCount
    automatically updates based on the current cursor style
*/
void Graphics::drawCursor() {
    
    // cursor blink
    if (frameCount % 60 > 30) {
        return;
    }

    Vector2 textSize = MeasureTextEx(assets.GetFont("normal"), ("> " + makeStr(textIn)).c_str(), fontSize, fontSpacing);
    Vector2 singleCharSize = MeasureTextEx(assets.GetFont("normal"), " ", fontSize, fontSpacing);
    
    switch(cursorStyle) {
        case CursorStyle::VerticalBar: {
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawLineEx({x, y - (textSize.y) + 3}, {x, y-1}, 1.25, WHITE);
            return;
        }
        case CursorStyle::Underline: {
            if (textIn.size() == Graphics::lineInLimit) {
                textSize = MeasureTextEx(assets.GetFont("normal"), (">" + makeStr(textIn)).c_str(), fontSize, fontSpacing);
            }
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawLineEx({x, y}, {x + singleCharSize.x, y}, 1.25, WHITE);
            return;
        }
        case CursorStyle::OutlineBox: {
            if (textIn.size() == Graphics::lineInLimit) {
                textSize = MeasureTextEx(assets.GetFont("normal"), (">" + makeStr(textIn)).c_str(), fontSize, fontSpacing);
            }
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawRectangleLinesEx({x - 1, y-textSize.y + 2, singleCharSize.x + 1, textSize.y - 2}, 1, WHITE);
            return;
        }
        case CursorStyle::TransparentBox: {
            if (textIn.size() == Graphics::lineInLimit) {
                textSize = MeasureTextEx(assets.GetFont("normal"), (">" + makeStr(textIn)).c_str(), fontSize, fontSpacing);
            }
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawRectangleRec({x - 1, y-textSize.y + 3, singleCharSize.x + 1, textSize.y - 3}, Color {180, 180, 180, 120});
            return;
        }
    }
}

/*
    returns what the player has typed
    TBG calls this method when ENTER is pressed
*/
std::string Graphics::getTextIn() {
    return makeStr(textIn);
}

/*
    get the current game text on a specific line
*/
std::string Graphics::getTextOut(int line) {
    return makeStr(textOut[line]);
}

/* set the player's text to whatever */
void Graphics::setTextIn(std::string str) {
    std::string s(str);
    textIn = std::vector<char>(s.begin(), s.end());
}

/* set the game text on a specific line */
void Graphics::setTextOut(std::string str, int line) {
    textOutScroll.at(line) = std::queue<char>();
    textOut[line].clear();
    //std::string s(str);
    //textOut[line] = std::vector<char>(s.begin(), s.end());
    for (char c : str) {
        textOutScroll.at(line).push(c);
    }
}

/* set the player hint */
void Graphics::setHint(std::string s) {
    textInHint = std::string(s);
}

/*
    append the rest of the hint to the player input, and clear the hint
    called when user hits tab
*/
void Graphics::addHintToInput() {
    for (char c : textInHint) {
        textIn.push_back(c);
    }
    textInHint.clear();
}

/* add a character to the player text */
void Graphics::addCharIn(char c) {
    if (textIn.size() < lineInLimit) {
        textIn.push_back(c);
    }
}

/* remove a character from the end of player text */
void Graphics::delCharIn() {
    if (textIn.size() > 0) {
        textIn.pop_back();
    }
}

/* change the text speed setting */
void Graphics::changeTextSpeed(Graphics::TextSpeed newSpeed) {
    textScrollSpeed = newSpeed;
}

/* change the cursor style setting */
void Graphics::changeCursorStyle(CursorStyle newStyle) {
    cursorStyle = newStyle;
}

/*
    is the text scroll queue empty?
    used to check if queue should be purged when user hits ENTER
*/
bool Graphics::queueEmpty() {
    for (auto& q : textOutScroll) {
        if (!q.empty()) {
            return false;
        }
    }
    return true;
}

/*
    on the next frame after this is called, purge the text scroll queue
*/
void Graphics::dumpText() {
    purgeQueueNextFrame = true;
}

/*
    set the current image (parameter is whatever the image name is within assetmanager)
*/
void Graphics::setImage(std::string newImageName) {
    currImage = newImageName;
}