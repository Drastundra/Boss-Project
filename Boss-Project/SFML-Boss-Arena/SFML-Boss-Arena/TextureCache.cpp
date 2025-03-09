#include "TextureCache.h"

#include <SFML/Graphics.hpp>

#include <filesystem>

TextureCache::TextureCache(const std::string& execFilePath) : m_execFilePath(execFilePath)
{
}

TextureCache::~TextureCache()
{
    for (auto& textureInfo : m_allTextureInfos)
        delete textureInfo.texture;
}

std::string TextureCache::getAbsoluteFilepath(const std::string& filename)
{
    std::filesystem::path execFilePath(m_execFilePath);
    auto filepath = execFilePath.parent_path().parent_path().parent_path() / "Resources" / std::filesystem::path(filename);
    return filepath.string();
}

sf::Texture& TextureCache::getTexture(const std::string& filename)
{
    std::string path = getAbsoluteFilepath(filename);
    for (auto& textureInfo : m_allTextureInfos)
    {
        if (textureInfo.path == path)
            return *textureInfo.texture;
    }

    TextureInfo ti;
    ti.path = path;
    ti.texture = new sf::Texture;
    ti.texture->loadFromFile(path);
    m_allTextureInfos.push_back(ti);

    return *ti.texture;
}
