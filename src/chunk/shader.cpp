#include "shader.h"

Chunk* Shader::create()
{
    return new Shader();
}

uint8_t Shader::getNumberOfFiles() const
{
    uint8_t files = 0;

    if (m_header->m_vertSize > 0)
    {
        ++files;
    }

    if (m_header->m_fragSize > 0)
    {
        ++files;
    }

    return files;
}

std::string Shader::getFilename(uint8_t file) const
{
    std::string name = getName();
    if (getType(file) == Type::vertex)
    {
        name += "_vertex";
    }
    else
    {
        name += "_fragment";
    }

    return name;
}

bool Shader::extract(uint8_t file, std::ostream& output) const
{
    uint32_t offset = 0;
    uint32_t size = 0;

    switch (getType(file))
    {
        case Type::vertex:
        {
            offset = strlen(getName());
            size = m_header->m_vertSize;
        }
        break;

        case Type::fragment:
        default:
        {
            offset = strlen(getName()) + m_header->m_vertSize;
            size = m_header->m_fragSize;
        }
        break;
    }

    output.write(m_buffer + offset, size);

    return true;
}

Shader::Type Shader::getType(uint8_t file) const
{
    switch (file)
    {
        case 1: return Type::fragment;
        case 0:
        default:
        {
            return m_header->m_vertSize > 0 ? Type::vertex : Type::fragment;
        }
    }
}