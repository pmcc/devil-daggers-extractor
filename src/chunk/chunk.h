#ifndef CHUNK_H
#define CHUNK_H

#include <functional>
#include <map>
#include <string>
#include <ostream>

template <typename t_header>
class HeaderedChunk : public Chunk
{
protected:

    typedef t_header Header;

public:

    void init(char* buffer)
    {
        m_header = reinterpret_cast<Header*>(buffer);
        m_buffer = buffer + sizeof(Header);
    }

protected:

    Header* m_header;
};

class Chunk
{
public:

    static void registerType(uint16_t type, std::function<Chunk*()> func)
    {
        m_factory[type] = func;
    }

    static Chunk* create(uint16_t type, const char* name, uint32_t startOffset, uint32_t size, uint32_t unknown)
    {
        Chunk* chunk = nullptr;

        auto it = m_factory.find(type);
        if (it != m_factory.end())
        {
            chunk = it->second();
            chunk->m_name = name;
            chunk->m_startOffset = startOffset;
            chunk->m_size = size;
            chunk->m_unknown = unknown;
            chunk->m_type = type;
        }

        return chunk;
    }

    virtual void init(char* buffer) = 0;
    virtual uint8_t getNumberOfFiles() const = 0;
    virtual std::string getFilename(uint8_t file) const = 0;
    virtual bool extract(uint8_t file, std::ostream& output) const = 0;

    const char* getName() const { return m_name; }
    uint32_t getStartOffset() const { return m_startOffset; }
    uint32_t getSize() const { return m_size; }
    uint16_t getType() const { return m_type; }

protected:

    const char* m_name;
    uint32_t m_startOffset;
    uint32_t m_size;
    uint32_t m_unknown;
    uint16_t m_type;

    char* m_buffer;

private:

    static std::map<uint16_t, std::function<Chunk*()>> m_factory;
};

#endif // CHUNK_H