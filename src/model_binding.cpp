#include "model_binding.h"

Chunk* ModelBinding::create()
{
    return new ModelBinding();
}

void ModelBinding::init(char* buffer)
{
    m_buffer = buffer;
}

uint8_t ModelBinding::getNumberOfFiles() const
{
    return 1;
}

std::string ModelBinding::getFilename(uint8_t file) const
{
    return std::string(m_name);
}

bool ModelBinding::extract(uint8_t file, std::ostream& output, const char* tmpPath) const
{
    output.write(m_buffer, m_size);

    return true;
}
