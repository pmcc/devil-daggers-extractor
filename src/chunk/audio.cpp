#include "audio.h"

Chunk* Audio::create()
{
    return new Audio();
}

void Audio::init(char* buffer)
{
    m_buffer = buffer;
}

uint8_t Audio::getNumberOfFiles() const
{
    return 1;
}

std::string Audio::getFilename(uint8_t file) const
{
    return std::string(m_name);
}

bool Audio::extract(uint8_t file, std::ostream& output) const
{
    output.write(m_buffer, m_size);

    return true;
}
