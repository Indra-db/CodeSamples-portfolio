#pragma once

// - Standard includes -
#include <unordered_map>

// - Project includes -
#include "Singleton.h"
#include <SDL.h>
#undef main

#pragma warning(push,0)
#include <SDL_gpu.h>
#include <SDL_mixer.h>
#pragma warning(pop)

// - Forward Declaration -
struct GPU_Image;
class Font;

/// @brief A manager that manages for all the resources currently in the game
class ResourceManager final : public Singleton<ResourceManager>
{
public:

      // ---- Destructor ----

    ~ResourceManager();

    // ---- Functionality ----

    void Init(const std::string& data);

    Font* const AddFont(const std::string& fontId , const std::string& file , unsigned int fontSize);
    GPU_Image* const AddTexture(const std::string& textureId , const std::string& file);
    GPU_Image* const AddTexture(const std::string& textureId , SDL_Surface* pSurface);
    Mix_Chunk* const AddAudioEffect(const std::string& audioId , const std::string& file);
    Mix_Music* const AddAudioMusic(const std::string& audioId , const std::string& file);
    bool DeleteTexture(const std::string& textureId) noexcept;
    bool DeleteFont(const std::string& fontId) noexcept;

    void ClearTextures() noexcept;
    void ClearFonts() noexcept;
    void ClearAudioEffects() noexcept;
    void ClearAudioMusic() noexcept;
    void ClearAllData() noexcept;

    // -- Getters --
    //TODO@IdB Error handling, write catches for the possible throws
    GPU_Image* const GetTexture(const std::string& textureId);
    Mix_Chunk* const GetEffectAudio(const std::string& audioID);
    Mix_Music* const GetMusicAudio(const std::string& audioID);
    Font* const GetFont(const std::string& fontId);

    // -- Setters --
    void SetTextureColoring(const std::string& textureId , const SDL_Color& color) noexcept;
    void SetTextureColoring(GPU_Image* pTexture , const SDL_Color& color) noexcept;
    void ResetTextureColoring(const std::string& textureId) noexcept;
    void ResetTextureColoring(GPU_Image* pTexture) noexcept;

private:

    friend class Singleton<ResourceManager>;

    // ---- Private Functions ----
    // -- Constructors --
    ResourceManager() = default;
    // ---- Data members ----

    std::string m_DataPath;
    std::unordered_map< std::string , GPU_Image*> m_Textures;
    std::unordered_map< std::string , Font*> m_Fonts;
    std::unordered_map< std::string , Mix_Chunk*> m_AudioEffects;
    std::unordered_map< std::string , Mix_Music*> m_AudioMusic;
};

// =============================================================================
//                               Inline Definitions
// =============================================================================

inline Mix_Chunk* const ResourceManager::GetEffectAudio(const std::string& audioID)
{
    return m_AudioEffects[audioID];
}

inline Mix_Music* const ResourceManager::GetMusicAudio(const std::string& audioID)
{
    return m_AudioMusic[audioID];
}

/// @brief Get the texture with the given textureId
/// @param textureId the id/name of the texture to find and return
/// @return return the found texture
inline GPU_Image* const ResourceManager::GetTexture(const std::string& textureId)
{
    return m_Textures[textureId];
}

/// @brief Get the font with the given fontid
/// @param fontId the id/name of the font to find and return
/// @return return the found font
inline Font* const ResourceManager::GetFont(const std::string& fontId)
{
    return m_Fonts[fontId];
}

inline void ResourceManager::SetTextureColoring(const std::string& textureId , const SDL_Color& color) noexcept
{
    GPU_SetColor(m_Textures[textureId] , color);
}

inline void ResourceManager::SetTextureColoring(GPU_Image* pTexture , const SDL_Color& color) noexcept
{
    GPU_SetColor(pTexture , color);
}

inline void ResourceManager::ResetTextureColoring(const std::string& textureId) noexcept
{
    GPU_UnsetColor(m_Textures[textureId]);
}

inline void ResourceManager::ResetTextureColoring(GPU_Image* pTexture) noexcept
{
    GPU_UnsetColor(pTexture);
}