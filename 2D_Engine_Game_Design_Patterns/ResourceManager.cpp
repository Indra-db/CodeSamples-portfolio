#include "PotentiaPCH.h"
#include "ResourceManager.h"

// - Library includes -
#include <SDL_image.h>
#include <SDL_ttf.h>

#pragma warning(push,0)
#include <SDL_gpu.h>
#pragma warning(pop)

// - Project includes -
#include "Renderer.h"
#include "Font.h"

// ---- Destructor ----

ResourceManager::~ResourceManager()
{
    ClearAllData();
}

// ---- Functionality ----

/// @brief Initializes an image or font
/// @param dataPath the path to the resource
void ResourceManager::Init(const std::string& dataPath)
{
    m_DataPath = dataPath;

    // load support for png and jpg, this takes a while!

    if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        throw std::runtime_error(std::string("Failed to load support for png's: ") + SDL_GetError());
    }

    if((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) != IMG_INIT_JPG)
    {
        throw std::runtime_error(std::string("Failed to load support for jpg's: ") + SDL_GetError());
    }

    if(TTF_Init() != 0)
    {
        throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
    }
}

/// @brief release the allocated memory of all the textures
void ResourceManager::ClearTextures() noexcept
{
    for(auto& [key , texture] : m_Textures)
    {
        GPU_FreeImage(texture);
    }
    m_Textures.clear();
}

/// @brief release the allocated memory of all the fonts
void ResourceManager::ClearFonts() noexcept
{
    for(auto& [key , font] : m_Fonts)
    {
        SafeDelete(font);
    }
    m_Fonts.clear();
}


void ResourceManager::ClearAudioEffects() noexcept
{
    for(auto& [key , pAudio] : m_AudioEffects)
    {
        Mix_FreeChunk(pAudio);
    }
    m_AudioEffects.clear();
}

void ResourceManager::ClearAudioMusic() noexcept
{
    for(auto& [key , pAudio] : m_AudioMusic)
    {
        Mix_FreeMusic(pAudio);
    }
    m_AudioMusic.clear();
}

/// @brief release the allocated memory of all the fonts and textures
void ResourceManager::ClearAllData() noexcept
{
    ClearTextures();
    ClearFonts();
    ClearAudioEffects();
    ClearAudioMusic();
}

Mix_Chunk* const ResourceManager::AddAudioEffect(const std::string& audioId , const std::string& file)
{
    const auto fullPath = m_DataPath + file;
    Mix_Chunk* pAudio = Mix_LoadWAV(fullPath.c_str());
    m_AudioEffects.emplace(audioId , pAudio);
    return pAudio;
}
Mix_Music* const ResourceManager::AddAudioMusic(const std::string& audioId , const std::string& file)
{
    const auto fullPath = m_DataPath + file;
    Mix_Music* pAudio = Mix_LoadMUS(fullPath.c_str());
    m_AudioMusic.emplace(audioId , pAudio);
    return pAudio;
}

/// @brief Load in a texture from a given file into memory and let the resource manager take ownership of it
/// @param textureId the id/name of the texture
/// @param file the image file name found at the specified path given at initialization
/// @return return the image to the caller for further usage
GPU_Image* const ResourceManager::AddTexture(const std::string& textureId , const std::string& file)
{
    const auto fullPath = m_DataPath + file;
    GPU_Image* pTexture = GPU_LoadImage(fullPath.c_str());

    if(pTexture == nullptr)
    {
          //_TODO log
        throw std::runtime_error(std::string("Failed to load texture in LoadTexture"));
        return nullptr;
    }
    m_Textures.emplace(textureId , pTexture);
    return pTexture;
}

/// @brief Load in a texture from a given file into memory and let the resource manager take ownership of it
/// @param textureId the id/name of the texture
/// @param pSurface surface to crate the image from
/// @return return the image to the caller for further usage
GPU_Image* const ResourceManager::AddTexture(const std::string& textureId , SDL_Surface* pSurface)
{
    GPU_Image* pTexture = GPU_CopyImageFromSurface(pSurface);
    m_Textures.emplace(textureId , pTexture);
    return pTexture;
}

/// @brief Load in a Font with a given size from a given file into memory and let the resource manager take ownership of it
/// @param fontId the id/name given to the font for finding it later
/// @param file the font file name found at the specified path given at initialization
/// @param fontSize the size to use for the font
/// @return return the font to the caller for further usage
Font* const ResourceManager::AddFont(const std::string& fontId , const std::string& file , unsigned int fontSize)
{
    Font* font = new Font{m_DataPath + file, fontSize};
    m_Fonts.emplace(fontId , font);
    return font;
}

/// @brief delete and release the hold memory of the given texture with the given id
/// @param textureId the id of the texture to release
/// @return return true if the texture was found and successfully released
bool ResourceManager::DeleteTexture(const std::string& textureId) noexcept
{
    auto texture = m_Textures.find(textureId);
    if(texture != m_Textures.end())
    {
        GPU_FreeImage(texture->second);
        m_Textures.erase(textureId);
        return true;
    }
    return false;
}

/// @brief delete and release the hold memory of the given font with the given id
/// @param fontId the id of the font to release
/// @return return true if the font was found and successfully released 
bool ResourceManager::DeleteFont(const std::string& fontId) noexcept
{
    auto font = m_Fonts.find(fontId);
    if(font != m_Fonts.end())
    {
        SafeDelete(font->second);
        return true;
    }
    m_Fonts.erase(fontId);
    return false;
}