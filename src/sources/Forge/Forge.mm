#import "Forge/Forge.h"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

// C++ imports
#import <vector>
#import <string>
#import <unordered_map>

static inline NSUInteger ceilDiv(NSUInteger a, NSUInteger b) {
    return (a + b - 1) / b;
}

static inline void logErr(NSError*_Nullable err) {
    NSLog(@"%s", [[err localizedDescription] UTF8String]);
}


@implementation FGEShader {
    struct Buffer {
        id<MTLBuffer> buffer;
        unsigned long offset, index;
    };

    id<MTLDevice> _device;
    id<MTLLibrary> _library;

    MTLSize _groupSize;
    MTLSize _gridSize;

    std::vector<Buffer> _buffers;
    std::unordered_map<std::string, id<MTLComputePipelineState>> _kernels;
}

- (nonnull instancetype) init {
    self = [super init];
    return self;
}

- (nonnull instancetype) initWithDevice: (nonnull id<MTLDevice>) device {
    self = [self init];
    _device = device;
    return self;
}

- (void) compileSource: (nonnull NSString*) src compileOptions: (nonnull MTLCompileOptions*) options {
    NSError* err;
    _library = [_device newLibraryWithSource:src options:options error:&err];
    if (_library == nil) {
        NSLog(@"FGEShader::compileSource — Failed to load shader source.");
        logErr(err);
        return;
    }
}

- (void) compileSource: (nonnull NSString*) src {
    MTLCompileOptions* options = [[MTLCompileOptions alloc] init];
    [self compileSource:src compileOptions:options];
}

- (void) loadKernel: (nonnull NSString*) kernelName {
    NSError* err;
    id<MTLFunction> kernel = [_library newFunctionWithName:kernelName];
    if (kernel == nil) {
        NSLog(@"FGEShader::loadKernel — Failed to load kernel function.");
        return;
    }
    id<MTLComputePipelineState> computePSO = [_device newComputePipelineStateWithFunction:kernel error:&err];
    if (computePSO == nil) {
        NSLog(@"FGEShader::loadKernel — Failed to create compute PSO.");
        logErr(err);
        return;
    }
    _kernels[[kernelName UTF8String]] = computePSO;
}

- (nonnull const void*) createOutputBuffer: (NSUInteger) dataSize offset: (NSUInteger) offset index: (NSUInteger) index {
    Buffer buffer = {};
    buffer.buffer = [_device newBufferWithLength:dataSize options:MTLResourceStorageModeShared];
    buffer.offset = offset;
    buffer.index = index;
    _buffers.push_back(buffer);
    return [buffer.buffer contents];
}

- (void) addBuffer: (nullable const void*) data dataSize: (NSUInteger) dataSize offset: (NSUInteger) offset index: (NSUInteger) index {
    Buffer buffer = {};
    buffer.buffer = [_device newBufferWithBytes:data length:dataSize options:MTLResourceStorageModeShared];
    buffer.offset = offset;
    buffer.index = index;
    _buffers.push_back(buffer);
}

- (NSUInteger) getKernelMaxThreadsPerGroup: (nonnull NSString*) kernelName {
    if (_kernels.find([kernelName UTF8String]) == _kernels.end()) {
        NSLog(@"FGEShader::getKernelMaxThreadsPerGroup — Kernel \"%@\" is not loaded.", kernelName);
        return 0;
    }
    return [_kernels.at([kernelName UTF8String]) maxTotalThreadsPerThreadgroup];
}

- (void) setGroupSize: (MTLSize) groupSize {
    _groupSize = groupSize;
}

- (void) setGridSize: (MTLSize) gridSize {
    _gridSize = gridSize;
}

- (void) setTotalSize: (MTLSize) totalSize {
    _gridSize = MTLSizeMake(
        ceilDiv(totalSize.width, _groupSize.width),
        ceilDiv(totalSize.height, _groupSize.height),
        ceilDiv(totalSize.depth, _groupSize.depth)
    );
}

- (void) runKernel: (nonnull NSString*) kernelName {
    if (_kernels.find([kernelName UTF8String]) == _kernels.end()) {
        NSLog(@"FGEShader::runKernel — Cannot run kernel \"%@\", it must be loaded first.", kernelName);
        return;
    }
    id<MTLCommandQueue> queue = [_device newCommandQueue];
    if (queue == nil) {
        NSLog(@"FGEShader::runKernel — Failed to create command queue.");
        return;
    }
    id<MTLCommandBuffer> buffer = [queue commandBuffer];
    if (buffer == nil) {
        NSLog(@"FGEShader::runKernel — Failed to create command buffer.");
        return;
    }
    id<MTLComputeCommandEncoder> encoder = [buffer computeCommandEncoder];
    if (encoder == nil) {
        NSLog(@"FGEShader::runKernel — Failed to create compute encoder.");
        return;
    }
    [encoder setComputePipelineState:_kernels.at([kernelName UTF8String])];
    for (Buffer buffer : _buffers) {
        [encoder setBuffer:buffer.buffer offset:buffer.offset atIndex:buffer.index];
    }
    [encoder dispatchThreadgroups:_gridSize threadsPerThreadgroup:_groupSize];
    [encoder endEncoding];
    [buffer commit];
    [buffer waitUntilCompleted];
}

@end
