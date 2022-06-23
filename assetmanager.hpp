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

    /* map of all stored fonts */
    std::unordered_map<std::string, Font> fonts;
    /* map of all stored textures */
    std::unordered_map<std::string, Texture2D> textures;

    public:
    /*
        if ever needed, add more config stuff right here
        like image default format or idk
    */

    /* consistent font size to load the fonts with */
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