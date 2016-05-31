#ifndef TEXTURE_H
#define TEXTURE_H

#include "chunk.h"

#pragma pack(push)
#pragma pack(1)
struct TextureHeader
{
    uint16_t m_unknown; // colour format?
    uint32_t m_width;
    uint32_t m_height;
    uint8_t m_mipmaps;
};
#pragma pack(pop)

class Texture final : public HeaderedChunk<TextureHeader>
{
public:

    static Chunk* create();

    uint8_t getNumberOfFiles() const;
    std::string getFilename(uint8_t file) const;

    bool extract(uint8_t file, std::ostream& output, const char* tmpPath) const override;
};

#endif // TEXTURE_H