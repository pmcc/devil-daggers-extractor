#include "fbx_output_stream.h"

FbxOutputStream::FbxOutputStream()
    : m_stream(nullptr)
    , m_state(eClosed)
{

}

FBXSDK_NAMESPACE::FbxStream::EState FbxOutputStream::GetState()
{
    return m_state;
}

bool FbxOutputStream::Open(void* pStreamData)
{
    m_stream = static_cast<std::ostream*>(pStreamData);

    if (m_stream && (long)m_stream->tellp() != -1)
    {
        m_state = eOpen;
    }

    return m_state == eOpen;
}

bool FbxOutputStream::Close()
{
    m_state = eClosed;
    return true;
}

bool FbxOutputStream::Flush()
{
    m_stream->flush();
    return m_stream->good();
}

int FbxOutputStream::Write(const void* pData, int pSize)
{
    long start = (long)m_stream->tellp();
    m_stream->write(static_cast<const char*>(pData), pSize);
    long end = (long)m_stream->tellp();
    return end - start;
}

int FbxOutputStream::GetWriterID() const
{
    return -1;
}

void FbxOutputStream::Seek(const FBXSDK_NAMESPACE::FbxInt64& pOffset, const FBXSDK_NAMESPACE::FbxFile::ESeekPos& pSeekPos)
{
    std::ios_base::seekdir dir;

    switch (pSeekPos)
    {
    case FBXSDK_NAMESPACE::FbxFile::eBegin: dir = std::ios_base::beg; break;
    case FBXSDK_NAMESPACE::FbxFile::eCurrent: dir = std::ios_base::cur; break;
    case FBXSDK_NAMESPACE::FbxFile::eEnd: dir = std::ios_base::end; break;
    }

    m_stream->seekp(pOffset, dir);
}

long FbxOutputStream::GetPosition() const
{
    return (long)m_stream->tellp();
}

void FbxOutputStream::SetPosition(long pPosition)
{
    m_stream->seekp(pPosition, std::ios_base::beg);
}

int FbxOutputStream::GetError() const
{
    return m_stream && m_stream->good() ? 0 : 1;
}

void FbxOutputStream::ClearError()
{
    m_stream->clear();
}

int FbxOutputStream::Read(void* pData, int pSize) const
{
    return 0;
}

char* FbxOutputStream::ReadString(char* pBuffer, int pMaxSize, bool pStopAtFirstWhiteSpace)
{
    return nullptr;
}

int FbxOutputStream::GetReaderID() const
{
    return -1;
}
