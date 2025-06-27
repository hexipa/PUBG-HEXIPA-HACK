#import <Foundation/Foundation.h>

typedef struct {
    long gWorldFun;
    long gWorldData;
    long gNameFun;
    long gNameData;
    long ActorsOffset;
} OffsetValues;

@interface OffsetsManager : NSObject

+ (OffsetValues)getOffsetsForBundleID:(NSString *)bundleID;

@end