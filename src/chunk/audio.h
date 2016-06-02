#ifndef AUDIO_H
#define AUDIO_H

#include "chunk.h"

class Audio : public Chunk
{
public:

    static Chunk* create();

    void init(char* buffer) override;
    uint8_t getNumberOfFiles() const override;
    std::string getFilename(uint8_t file) const override;
    bool extract(uint8_t file, std::ostream& output) const override;
};

#endif // AUDIO_H