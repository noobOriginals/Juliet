#import "Forge/forge_c.h"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#import "Forge/Forge.h"

FGECSize fgecSizeMake(ulong width, ulong height, ulong depth) {
    return {width, height, depth};
}

static FGECSize castMtlToFgecSize(MTLSize size) {
    return fgecSizeMake(size.width, size.height, size.depth);
}

static MTLSize castFgecToMtlSize(FGECSize size) {
    return MTLSizeMake(size.width, size.height, size.depth);
}

static id<MTLDevice> DEVICE;

struct FGECShader {
    FGEShader* shader;
};

void fgecInit() {
    DEVICE = MTLCreateSystemDefaultDevice();
}

FGECShader* fgecCreateShader() {
    FGECShader* shader = new FGECShader;
    shader->shader = [[FGEShader alloc] initWithDevice:DEVICE];
    return shader;
}

void fgecDestroyShader(FGECShader* shader) {
    delete shader;
}

void fgecShaderCompileSource(FGECShader* shader, const char* src) {
    [shader->shader compileSource:[NSString stringWithUTF8String:src]];
}

void fgecShaderLoadKernel(FGECShader* shader, const char* kernelName) {
    [shader->shader loadKernel:[NSString stringWithUTF8String:kernelName]];
}

const void* fgecShaderCreateOutputBuffer(FGECShader* shader, ulong size, ulong offset, ulong index) {
    return [shader->shader createOutputBuffer:size offset:offset index:index];
}

void fgecShaderAddBuffer(FGECShader* shader, const void* data, ulong size, ulong offset, ulong index) {
    [shader->shader addBuffer:data dataSize:size offset:offset index:index];
}

ulong fgecShaderKernelGetMaxThreadsPerGroup(FGECShader* shader, const char* kernelName) {
    return [shader->shader getKernelMaxThreadsPerGroup: [NSString stringWithUTF8String:kernelName]];
}

void fgecShaderSetGroupSize(FGECShader* shader, FGECSize groupSize) {
    [shader->shader setGroupSize:castFgecToMtlSize(groupSize)];
}

void fgecShaderSetGridSize(FGECShader* shader, FGECSize gridSize) {
    [shader->shader setGridSize:castFgecToMtlSize(gridSize)];
}

void fgecShaderSetTotalSize(FGECShader* shader, FGECSize totalSize) {
    [shader->shader setTotalSize:castFgecToMtlSize(totalSize)];
}

void fgecShaderRunKernel(FGECShader* shader, const char* kernelName) {
    [shader->shader runKernel:[NSString stringWithUTF8String:kernelName]];
}
