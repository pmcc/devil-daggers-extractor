#ifndef FBX_OUTPUT_STREAM
#define FBX_OUTPUT_STREAM

#include <ostream>
#include <fbxsdk/core/fbxstream.h>

class FbxOutputStream final : public FBXSDK_NAMESPACE::FbxStream
{
public:

    explicit FbxOutputStream();

    EState GetState() override;
    bool Open(void* pStreamData) override;
    bool Close() override;
    bool Flush() override;
    int Write(const void* pData, int pSize);
    int GetWriterID() const;
    void Seek(const FBXSDK_NAMESPACE::FbxInt64& pOffset, const FBXSDK_NAMESPACE::FbxFile::ESeekPos& pSeekPos);
    long GetPosition() const;
    void SetPosition(long pPosition);
    int GetError() const;
    void ClearError();

    // Dummy implementation
    int Read(void* pData, int pSize) const;
    char* ReadString(char* pBuffer, int pMaxSize, bool pStopAtFirstWhiteSpace = false);
    int GetReaderID() const;

private:

    std::ostream* m_stream;
    EState m_state;
};

#endif // FBX_OUTPUT_STREAM