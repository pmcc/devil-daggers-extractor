#ifdef DEBUG
#define FBXSDK_LIB  "../ext/fbx/lib/debug/libfbxsdk.lib"
#else
#define FBXSDK_LIB  "../ext/fbx/lib/release/libfbxsdk.lib"
#endif

#pragma comment(lib, FBXSDK_LIB)