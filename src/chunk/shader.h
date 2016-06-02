#ifndef SHADER_H
#define SHADER_H

#include "chunk.h"

struct ShaderHeader
{
    uint32_t m_nameLength;
    uint32_t m_vertSize;
    uint32_t m_fragSize;
};

class Shader final : public HeaderedChunk<ShaderHeader>
{
public:

    static Chunk* create();

    uint8_t getNumberOfFiles() const;
    std::string getFilename(uint8_t file) const;

    bool extract(uint8_t file, std::ostream& output) const override;

private:

    enum class Type
    {
        vertex,
        fragment,
    };

private:

    Type getType(uint8_t file) const;
};

#endif // SHADER_H