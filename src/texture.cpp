#include "texture.h"
#include "lodepng.h"

#include <sstream>

//#define TEXTURE_EXTRACT_ALL_MIPMAPS
#define TEXTURE_FLIP

Chunk* Texture::create()
{
    return new Texture();
}

uint8_t Texture::getNumberOfFiles() const
{
#ifdef TEXTURE_EXTRACT_ALL_MIPMAPS
    return m_header->m_mipmaps;
#else
    return 1;
#endif
}

std::string Texture::getFilename(uint8_t file) const
{
#ifdef TEXTURE_EXTRACT_ALL_MIPMAPS
    uint32_t width = m_header->m_width >> file;
    uint32_t height = m_header->m_height >> file;

    std::ostringstream oss;
    oss << m_name << "_" << width << "x" << height;

    return oss.str();
#else
    return std::string(m_name);
#endif
}

bool Texture::extract(uint8_t file, std::ostream& output, const char* tmpPath) const
{
    uint32_t width = m_header->m_width >> file;
    uint32_t height = m_header->m_height >> file;

    uint32_t offset = 0;
    while (file > 0)
    {
        offset += (m_header->m_width >> (file - 1)) * (m_header->m_height >> (file - 1)) * 4;
        --file;
    }

    unsigned char* buffer = reinterpret_cast<unsigned char*>(m_buffer) + offset;

#ifdef TEXTURE_FLIP
    // (╯°□°）╯︵ ┻━┻
    uint32_t rowLength = width * 4;
    unsigned char* flippedBuffer = new unsigned char[rowLength * height];
    for (uint32_t y = 0; y < height; ++y)
    {
        memcpy(flippedBuffer + y * rowLength, buffer + (height - y - 1) * rowLength, rowLength);
    }
    buffer = flippedBuffer;
#endif

    unsigned char* encodedData = nullptr;
    size_t encodedSize = 0;
    lodepng_encode32(&encodedData, &encodedSize, buffer, width, height);
    output.write(reinterpret_cast<char*>(encodedData), encodedSize);
    free(encodedData);

#ifdef TEXTURE_FLIP
    delete[] flippedBuffer;
#endif

    return true;
}