#include "assetmanager.hpp"

/*
    AssetManager constructor
    takes no args, initializes font and texture maps
*/
AssetManager::AssetManager() {
    fonts = std::unordered_map<std::string, Font>();
    textures = std::unordered_map<std::string, Texture2D>();
}

/*
    AssetManager destructor
    calls Unload on every font and texture and clears the maps
*/
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

/* creates a font from a file, must be .ttf i think, and stores it under the given name */
void AssetManager::CreateFont(const char *name, const char *filename) {
    Font font = LoadFontEx(filename, fontSize, nullptr, 0);
    fonts.insert(
        std::make_pair(std::string(name), font)
    );
}

/* returns a font from the collection under a specific name */
Font AssetManager::GetFont(const char *name) {
    return fonts.at(std::string(name));
}

/* deletes a font, but (FIXME) mysteriously does not Unload it */
void AssetManager::DeleteFont(const char *name) {
    fonts.erase(std::string(name));
}

/* creates a texture from a file and stores it under the given name */
void AssetManager::CreateTexture(const char *name, const char *filename) {
    Texture2D texture = LoadTexture(filename);
    textures.insert(
        std::make_pair(std::string(name), texture)
    );
}

/* returns a texture from the collection under a specific name */
Texture2D AssetManager::GetTexture(const char *name) {
    return textures.at(std::string(name));
}

/* deletes a texture, but (FIXME) mysteriously does not Unload it */
void AssetManager::DeleteTexture(const char *name) {
    textures.erase(std::string(name));
}