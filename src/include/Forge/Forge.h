#ifndef __APPLE__
#error "This header is only available for MacOS."
#endif // __APPLE__

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

@interface FGEShader:NSObject

- (nonnull instancetype) init;
- (nonnull instancetype) initWithDevice: (nonnull id<MTLDevice>) device;
- (void) compileSource: (nonnull NSString*) src compileOptions: (nonnull MTLCompileOptions*) options;
- (void) compileSource: (nonnull NSString*) src;
- (void) loadKernel: (nonnull NSString*) kernelName;
- (nonnull const void*) createOutputBuffer: (NSUInteger) dataSize offset: (NSUInteger) offset index: (NSUInteger) index;
- (void) addBuffer: (nullable const void*) data dataSize: (NSUInteger) dataSize offset: (NSUInteger) offset index: (NSUInteger) index;
- (NSUInteger) getKernelMaxThreadsPerGroup: (nonnull NSString*) kernelName;
- (void) setGroupSize: (MTLSize) groupSize;
- (void) setGridSize: (MTLSize) gridSize;
- (void) setTotalSize: (MTLSize) totalSize;
- (void) runKernel: (nonnull NSString*) kernelName;

@end
