#ifndef MODEL_H
#define MODEL_H

#include "chunk.h"

#define ENABLE_FBX  0

#if ENABLE_FBX

#include <fbxsdk.h>

#endif

#pragma pack(push)
#pragma pack(2)
struct ModelHeader
{
    uint32_t m_indexCount;
    uint32_t m_vertexCount;
    uint16_t m_unknown;
};
#pragma pack(pop)

class Model final : public HeaderedChunk<ModelHeader>
{
public:

    virtual ~Model();

    static Chunk* create();

    uint8_t getNumberOfFiles() const;
    std::string getFilename(uint8_t file) const;

    bool extract(uint8_t file, std::ostream& output) const override;

private:

    struct Vertex
    {
        float m_pos[3];
        float m_normal[3];
        float m_uv[2];
    };

#if ENABLE_FBX

private:

    static FbxManager* s_fbxManager;

#endif
};

#endif // MODEL_H