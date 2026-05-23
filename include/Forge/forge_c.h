#ifndef FORGE_C_H
#define FORGE_C_H

#ifndef __APPLE__
#error "This header is only available for MacOS."
#endif // __APPLE__

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char schar;
typedef long long llong;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef float f32;
typedef double f64;

typedef struct {
    ulong width, height, depth;
} FGECSize;

FGECSize fgecSizeMake(ulong width, ulong height, ulong depth);

struct FGECShader;

void fgecInit();
FGECShader* fgecCreateShader();
void fgecDestroyShader(FGECShader* shader);
void fgecShaderCompileSource(FGECShader* shader, const char* src);
void fgecShaderLoadKernel(FGECShader* shader, const char* kernelName);
const void* fgecShaderCreateOutputBuffer(FGECShader* shader, ulong size, ulong offset, ulong index);
const void* fgecShaderAddBuffer(FGECShader* shader, const void* data, ulong size, ulong offset, ulong index);
ulong fgecShaderKernelGetMaxThreadsPerGroup(FGECShader* shader, const char* kernelName);
void fgecShaderSetGroupSize(FGECShader* shader, FGECSize groupSize);
void fgecShaderSetGridSize(FGECShader* shader, FGECSize gridSize);
void fgecShaderSetTotalSize(FGECShader* shader, FGECSize totalSize);
void fgecShaderRunKernel(FGECShader* shader, const char* kernelName);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FORGE_C_H
