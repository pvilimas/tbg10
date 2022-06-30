#include "graphics.hpp"

std::string Graphics::MakeStr(std::vector<char>& v) {
    return std::string(v.begin(), v.end());
}

Graphics::Graphics() {

    /* init everything */
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(Graphics::DefaultWinWidth, DefaultWinHeight, Graphics::TitleText);
    
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
    windowSize = Vector2 { Graphics::DefaultWinWidth, Graphics::DefaultWinHeight };
    currentImage = "Kitchen";
    renderTexture = LoadRenderTexture(windowSize.x, windowSize.y);
}

Graphics::~Graphics() {
    UnloadRenderTexture(renderTexture);
    CloseWindow();
}

void Graphics::NormalizeWindowSize() {

    float targetW, targetH;
    float monitor = GetCurrentMonitor();
    float w = GetMonitorWidth(monitor);
    float h = GetMonitorHeight(monitor);
    std::cout << fmt::format("w={}, h={}", w, h) << std::endl;
    
    if (w != windowSize.x || h != windowSize.y) {
        // arbitrary as fuck
        if (w >= 2560 && h >= 1600) {
            resolutionScaleFactor = Resolution::High4k;
            targetW = Graphics::DefaultWinWidth * 2;
            targetH = Graphics::DefaultWinHeight * 2;
        } else {
            resolutionScaleFactor = Resolution::Low1080p;
            targetW = Graphics::DefaultWinWidth;
            targetH = Graphics::DefaultWinHeight;
        }

        if (targetW != windowSize.x || targetH != windowSize.y) {
            windowSize = Vector2 { targetW, targetH };
            SetWindowSize(targetW, targetH);
            // SetWindowPosition(
            //   (w/2) - (windowSize.x/2), 
            //   (h/2) - (windowSize.y/2) 
            // );
            UnloadRenderTexture(renderTexture);
            renderTexture = LoadRenderTexture(windowSize.x, windowSize.y);
        }
    }

}

void Graphics::Draw() {

    // NormalizeWindowSize();

    bool addCharThisFrame = false;
    
    switch(textScrollSpeed) {
        case TextSpeed::Slow : addCharThisFrame = (frameCount % 5 == 0); break; 
        case TextSpeed::Medium : addCharThisFrame = (frameCount % 3 == 0); break; 
        case TextSpeed::Fast : addCharThisFrame = true; break; 
    }

    BeginTextureMode(renderTexture);
    
    if (purgeQueueNextFrame) {
        for (int i = 0; i < Graphics::LineOutCount; i++) {
            while (!textOutScroll.at(i).empty()) {
                char c = textOutScroll.at(i).front();
                textOut[i].push_back(c);
                textOutScroll.at(i).pop();
            }
        }
        purgeQueueNextFrame = false;
    } else if (addCharThisFrame) {
        for (int i = 0; i < Graphics::LineOutCount; i++) {
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
    DrawTexture(assets.GetTexture(currentImage.c_str()), 2, 24, WHITE);

    // title - centered
    //DrawTextEx(assets.GetFont("italic"), Graphics::TitleText, {(windowSize.x - MeasureTextEx(assets.GetFont("italic"), Graphics::TitleText, fontSize, fontSpacing).x) / 2, 2}, fontSize, fontSpacing, LIGHTGRAY);
    // title not centered
    DrawTextEx(assets.GetFont("title"), Graphics::TitleText, {4, 2}, Graphics::FontSize, Graphics::FontSpacing, RAYWHITE);
    // output lines
    DrawTextEx(assets.GetFont("italic"), MakeStr(textOut[0]).c_str(), {6, 388}, Graphics::FontSize, Graphics::FontSpacing, LIGHTGRAY);
    DrawTextEx(assets.GetFont("italic"), MakeStr(textOut[1]).c_str(), {6, 410}, Graphics::FontSize, Graphics::FontSpacing, LIGHTGRAY);
    DrawTextEx(assets.GetFont("italic"), MakeStr(textOut[2]).c_str(), {6, 432}, Graphics::FontSize, Graphics::FontSpacing, LIGHTGRAY);
    DrawTextEx(assets.GetFont("italic"), MakeStr(textOut[3]).c_str(), {6, 454}, Graphics::FontSize, Graphics::FontSpacing, LIGHTGRAY);
    // input line
    DrawTextEx(assets.GetFont("normal"), (Graphics::PlayerPrompt + MakeStr(textIn)).c_str(), Vector2 { 6, 480 }, Graphics::FontSize, Graphics::FontSpacing, RAYWHITE);

    // hint
    DrawTextEx(assets.GetFont("normal"), (std::string(textIn.size() + 2, ' ') + textInHint).c_str(), Vector2 { 6, 480 }, Graphics::FontSize, Graphics::FontSpacing, LIGHTGRAY);

    // title border
    DrawRectangleLinesEx(Rectangle {0, 0, 644, 24}, Graphics::FrameThick, Graphics::FrameColor);
    // picture border
    DrawRectangleLinesEx(Rectangle {0, 22, 644, 364}, Graphics::FrameThick, Graphics::FrameColor);
    // text border
    DrawRectangleLinesEx(Rectangle {0, 384, 644, 120}, Graphics::FrameThick, Graphics::FrameColor);
    // I/O separator
    DrawLineEx({0, 480}, {644, 480}, Graphics::FrameThick, Graphics::FrameColor);
    
    DrawCursor();
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

void Graphics::DrawCursor() {
    
    // cursor blink
    if (frameCount % 60 > 30) {
        return;
    }

    Vector2 textSize = MeasureTextEx(assets.GetFont("normal"), (Graphics::PlayerPrompt + MakeStr(textIn)).c_str(), Graphics::FontSize, Graphics::FontSpacing);
    Vector2 singleCharSize = MeasureTextEx(assets.GetFont("normal"), " ", Graphics::FontSize, Graphics::FontSpacing);
    
    switch(cursorStyle) {
        case CursorStyle::VerticalBar: {
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawLineEx({x, y - (textSize.y) + 3}, {x, y-1}, 1.25, WHITE);
            return;
        }
        case CursorStyle::Underline: {
            if (textIn.size() == Graphics::LineInLimit) {
                textSize = MeasureTextEx(assets.GetFont("normal"), (Graphics::PlayerPrompt + MakeStr(textIn)).c_str(), Graphics::FontSize, Graphics::FontSpacing);
            }
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawLineEx({x, y}, {x + singleCharSize.x, y}, 1.25, WHITE);
            return;
        }
        case CursorStyle::OutlineBox: {
            if (textIn.size() == Graphics::LineInLimit) {
                textSize = MeasureTextEx(assets.GetFont("normal"), (Graphics::PlayerPrompt + MakeStr(textIn)).c_str(), Graphics::FontSize, Graphics::FontSpacing);
            }
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawRectangleLinesEx({x - 1, y-textSize.y + 2, singleCharSize.x + 1, textSize.y - 2}, 1, WHITE);
            return;
        }
        case CursorStyle::TransparentBox: {
            if (textIn.size() == Graphics::LineInLimit) {
                textSize = MeasureTextEx(assets.GetFont("normal"), (Graphics::PlayerPrompt + MakeStr(textIn)).c_str(), Graphics::FontSize, Graphics::FontSpacing);
            }
            float x = textSize.x + 7;
            float y = textSize.y + 480;
            DrawRectangleRec({x - 1, y-textSize.y + 3, singleCharSize.x + 1, textSize.y - 3}, Color {180, 180, 180, 120});
            return;
        }
    }
}

std::string Graphics::GetTextIn() {
    return MakeStr(textIn);
}

std::string Graphics::GetTextOut(int line) {
    return MakeStr(textOut[line]);
}

void Graphics::SetTextIn(std::string str) {
    std::string s(str);
    textIn = std::vector<char>(s.begin(), s.end());
}

void Graphics::SetTextOut(std::string str, int line) {
    textOutScroll.at(line) = std::queue<char>();
    textOut[line].clear();
    //std::string s(str);
    //textOut[line] = std::vector<char>(s.begin(), s.end());
    for (char c : str) {
        textOutScroll.at(line).push(c);
    }
}

void Graphics::SetHint(std::string s) {
    textInHint = s;
}

void Graphics::AddHintToInput() {
    for (char c : textInHint) {
        textIn.push_back(c);
    }
    textInHint.clear();
}

void Graphics::AddCharIn(char c) {
    if (textIn.size() < Graphics::LineInLimit) {
        textIn.push_back(c);
    }
}

void Graphics::DelCharIn() {
    if (textIn.size() > 0) {
        textIn.pop_back();
    }
}

void Graphics::ChangeTextSpeed(Graphics::TextSpeed newSpeed) {
    textScrollSpeed = newSpeed;
}

void Graphics::ChangeCursorStyle(CursorStyle newStyle) {
    cursorStyle = newStyle;
}

bool Graphics::IsQueueEmpty() {
    for (auto& q : textOutScroll) {
        if (!q.empty()) {
            return false;
        }
    }
    return true;
}

void Graphics::DumpText() {
    purgeQueueNextFrame = true;
}

void Graphics::SetBackgroundImage(std::string newImageName) {
    currentImage = newImageName;
}
