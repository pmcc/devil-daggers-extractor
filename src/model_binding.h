#ifndef MODEL_BINDING_H
#define MODEL_BINDING_H

#include "chunk.h"

class ModelBinding : public Chunk
{
public:

    static Chunk* create();

    void init(char* buffer) override;
    uint8_t getNumberOfFiles() const override;
    std::string getFilename(uint8_t file) const override;
    bool extract(uint8_t file, std::ostream& output, const char* tmpPath) const override;
};

#endif // MODEL_BINDING_H