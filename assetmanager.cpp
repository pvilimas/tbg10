#include "assetmanager.hpp"

AssetManager::AssetManager() {
    fonts = std::unordered_map<std::string, Font>();
    textures = std::unordered_map<std::string, Texture2D>();
}

AssetManager::~AssetManager() {
    
    for (auto& [_, f] : fonts) {
        UnloadFont(f);
    }
    
    for (auto& [_, t] : textures) {
        UnloadTexture(t);
    }

    fonts.clear();
    textures.clear();
}

void AssetManager::CreateFont(const char *name, const char *filename) {
    Font font = LoadFontEx(filename, fontSize, nullptr, 0);
    fonts.insert(
        std::make_pair(std::string(name), font)
    );
}

Font AssetManager::GetFont(const char *name) {
    return fonts.at(std::string(name));
}

void AssetManager::DeleteFont(const char *name) {
    fonts.erase(std::string(name));
}

void AssetManager::CreateTexture(const char *name, const char *filename) {
    Texture2D texture = LoadTexture(filename);
    textures.insert(
        std::make_pair(std::string(name), texture)
    );
}

Texture2D AssetManager::GetTexture(const char *name) {
    return textures.at(std::string(name));
}

void AssetManager::DeleteTexture(const char *name) {
    textures.erase(std::string(name));
}