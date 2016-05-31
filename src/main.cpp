#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include <map>
#include <functional>

#include "chunk.h"
#include "texture.h"
#include "shader.h"
#include "audio.h"
#include "model_binding.h"
#include "model.h"

std::map<uint16_t, std::function<Chunk*()>> Chunk::m_factory;

#define MAKE_MAGIC(a,b,c,d) (a | b << 8 | c << 16 | d << 24)

#define MAGIC_1     MAKE_MAGIC(0x3AULL,0x68ULL,0x78ULL,0x3AULL)
#define MAGIC_2     MAKE_MAGIC(0x72ULL,0x67ULL,0x3AULL,0x01ULL)

#define CHUNK_TOC_END       0x00
#define CHUNK_MODEL         0x01
#define CHUNK_TEXTURE       0x02
#define CHUNK_SHADER        0x10
#define CHUNK_AUDIO         0x20
#define CHUNK_MODEL_BINDS   0x80 // I'm really (not) good at naming things

struct Header
{
    uint32_t m_magicNumber1;
    uint32_t m_magicNumber2;
    uint32_t m_tocSize;
};

const char* getExtension(uint16_t type)
{
    switch (type)
    {
        case CHUNK_TEXTURE: return ".png";
        case CHUNK_MODEL: return ".fbx";
        case CHUNK_AUDIO: return ".wav";
        case CHUNK_SHADER: return ".glsl";
        case CHUNK_MODEL_BINDS: return ".txt";
    }

    return nullptr;
}

const char* getFolder(uint16_t type)
{
    switch (type)
    {
        case CHUNK_MODEL: return "models";
        case CHUNK_TEXTURE: return "textures";
        case CHUNK_AUDIO: return "audio";
        case CHUNK_SHADER: return "shaders";
        case CHUNK_MODEL_BINDS: return "model_bindings";
    }

    return nullptr;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: dd_extract.exe \"input\" \"output\"\n"
            "- input: devil daggers package \n"
            "- output: where to extract the files, will be created if not found\n"
            "example: dd_extract.exe \"C:\\Program Files (x86)\\Steam\\steamapps\\common\\devildaggers\\res\\audio\" \"audio_extracted\"" << std::endl;
        return 1;
    }

    std::tr2::sys::path inputPath = argv[1];
    std::tr2::sys::path outputPath = argv[2];

    if (!std::tr2::sys::exists(inputPath))
    {
        std::cout << "error: cannot find input file" << std::endl;
        return 1;
    }

    if (!std::tr2::sys::is_regular_file(inputPath))
    {
        std::cout << "error: input path is not a file" << std::endl;
        return 1;
    }

    if (std::tr2::sys::exists(outputPath) && !std::tr2::sys::is_directory(outputPath))
    {
        std::cout << "error: output path is not a directory!" << std::endl;
        return 1;
    }

    std::ifstream file;
    file.open(inputPath, std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "error: failed to open input file" << std::endl;
        return 1;
    }

    Chunk::registerType(CHUNK_AUDIO, std::bind(Audio::create));
    Chunk::registerType(CHUNK_TEXTURE, std::bind(Texture::create));
    Chunk::registerType(CHUNK_SHADER, std::bind(Shader::create));
    Chunk::registerType(CHUNK_MODEL_BINDS, std::bind(ModelBinding::create));
    Chunk::registerType(CHUNK_MODEL, std::bind(Model::create));

    Header archiveHeader;

    file.read((char*)&archiveHeader, sizeof(Header));

    if (archiveHeader.m_magicNumber1 != MAGIC_1 && archiveHeader.m_magicNumber2 != MAGIC_2)
    {
        std::cout << "error: invalid file format" << std::endl;
        file.close();
        return 1;
    }

    char* tocBuffer = new char[archiveHeader.m_tocSize];
    file.read(tocBuffer, archiveHeader.m_tocSize);

    std::vector<Chunk*> chunks;

    // Read toc and create chunks
    char* pos = tocBuffer;
    while (*(uint16_t*)pos != CHUNK_TOC_END)
    {
        uint16_t type = *(uint16_t*)pos;
        const char* name = (char*)(pos) + sizeof(uint16_t);
        size_t len = strlen(name) + 1;
        uint32_t offset = *(uint32_t*)(name + len);
        uint32_t size = *((uint32_t*)(name + len) + 1);
        uint32_t unknown = *((uint32_t*)(name + len) + 2);

        Chunk* chunk = Chunk::create(type, name, offset, size, unknown);
        if (chunk)
        {
            chunks.emplace_back(chunk);
        }

        pos += 14 + len;
    }

    std::cout << "found " << chunks.size() << " chunk(s)" << std::endl;

    for (Chunk* chunk : chunks)
    {
        file.seekg(chunk->getStartOffset(), std::ios_base::beg);

        if (file.bad())
        {
            std::cout << "error: failed to extract " << chunk->getName() << std::endl;
            break;
        }

        uint16_t type = chunk->getType();
        uint32_t size = chunk->getSize();

        // Allocate the input buffer for the chunk
        char* buffer = new char[size];
        file.read(buffer, size);

        if (file.gcount() != size)
        {
            std::cout << "error: failed to extract " << chunk->getName() << std::endl;
            break;
        }

        chunk->init(buffer);

        for (uint8_t file = 0; file < chunk->getNumberOfFiles(); ++file)
        {
            // temporary until I write a FbxStream wrapper for std::ostream... quite lazy at the moment :)
            const char* tmpPath = nullptr;

            // open destination file
            std::ofstream output;
            const char* folder = getFolder(type);
            const char* extension = getExtension(type);

            std::tr2::sys::path directory = outputPath;
            if (folder)
            {
                directory /= folder;
            }
            else
            {
                directory /= std::to_string(type);
            }

            if (!std::tr2::sys::exists(directory))
            {
                std::tr2::sys::create_directories(directory);
            }

            std::tr2::sys::path filename = directory / chunk->getFilename(file);
            if (extension)
            {
                filename.replace_extension(extension);
            }

            std::string tmpString = filename.string();
            if (type == CHUNK_MODEL)
            {
                tmpPath = tmpString.c_str();
            }

            if (!tmpPath)
            {
                output.open(filename, std::ios::binary);
            }

            // extract the content!
            chunk->extract(file, output, tmpPath);

            if (output.is_open())
            {
                output.close();
            }
        }

        delete[] buffer;
    }

    delete[] tocBuffer;

    file.close();

    for (Chunk* chunk : chunks)
    {
        delete chunk;
    }

    return 0;
}