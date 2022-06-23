#ifndef __ASSETMANAGER__
#define __ASSETMANAGER__

#include <iostream>
#include <string>
#include <unordered_map>

#include "raylib.h"

/*
    Manages all fonts and textures, stored with string keys in separate hashmaps.
    Takes care of all deallocation.
*/

class AssetManager {

    private:

    std::unordered_map<std::string, Font> fonts;
    std::unordered_map<std::string, Texture2D> textures;

    public:

    static constexpr int fontSize = 20;

    AssetManager();
    ~AssetManager();

    void CreateFont(const char *name, const char *filename);
    Font GetFont(const char *name);
    void DeleteFont(const char *name);
    
    void CreateTexture(const char *name, const char *filename);
    Texture2D GetTexture(const char *name);
    void DeleteTexture(const char *name);

};

#endif /* __ASSETMANAGER__ */