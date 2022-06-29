#ifndef __ASSETMANAGER__
#define __ASSETMANAGER__

#include <iostream>
#include <string>
#include <unordered_map>

#include "raylib/raylib.h"

/*
    Manages all fonts and textures, stored with string keys in separate hashmaps.
    Takes care of all deallocation.
*/
class AssetManager {

    private:

    /*   map of all stored fonts   */
    std::unordered_map<std::string, Font> fonts;
    /*   map of all stored textures   */
    std::unordered_map<std::string, Texture2D> textures;

    public:
    /*
        if ever needed, add more config stuff right here
        like image default format or idk
    */

    /*   consistent font size to load the fonts with   */
    static constexpr int fontSize = 20;

    /*
        AssetManager constructor
        takes no args, initializes font and texture maps
    */
    AssetManager();
    /*
        AssetManager destructor
        calls Unload on every font and texture and clears the maps
    */
    ~AssetManager();

    
    /*   creates a font from a file, must be .ttf i think, and stores it under the given name   */
    void CreateFont(const char *name, const char *filename);
    
    /*   returns a font from the collection under a specific name   */
    Font GetFont(const char *name);
    
    /*   deletes a font, but ( TODO ) mysteriously does not Unload it   */
    void DeleteFont(const char *name);
    
    
    /*   creates a texture from a file and stores it under the given name   */
    void CreateTexture(const char *name, const char *filename);
    
    /*   returns a texture from the collection under a specific name   */
    Texture2D GetTexture(const char *name);
    
    /*   deletes a texture, but ( TODO ) mysteriously does not Unload it   */
    void DeleteTexture(const char *name);

};

#endif /* __ASSETMANAGER__ */