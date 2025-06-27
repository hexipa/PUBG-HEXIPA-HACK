#import "utils/crossoffsets.h"

@implementation OffsetsManager

/*
    long gWorldFun;
    long gWorldData;
    long gNameFun;
    long gNameData;
    long ActorsOffset;
*/


+ (OffsetValues)getOffsetsForBundleID:(NSString *)bundleID {
    OffsetValues defaultOffsets = { 0x1029487B4, 0x10A44BEE0, 0x1046AEDD8, 0x109DAF390, 0x105D5D918};
if ([bundleID containsString:@"tencent"]) {
    return (OffsetValues){ 0x1029487B4, 0x10A44BEE0, 0x1046AEDD8, 0x109DAF390, 0x105D5D918};
} else if ([bundleID containsString:@"vng"]) {
    return (OffsetValues){ 0x1028791CC, 0x10A171A00, 0x104510EF0, 0x109AAA1A0, 0x105BB38A0};
} else if ([bundleID containsString:@"krmobile"]) {
    return (OffsetValues){ 0x102AD71F8, 0x10A47D400, 0x10476F14C, 0x109DB5940, 0x105E12068};
} else if ([bundleID containsString:@"rekoo"]) {
    return (OffsetValues){ 0x102AAAB0C, 0x10A453300, 0x104742830, 0x109D8B830, 0x105de51e0};
}

    return defaultOffsets;
}

@end

/*
pubgm vng 3.6 
gobject 0x109F5C2A0
gname_func 0x104510EF0
gname_data 0x109AAA1A0
gengine 0x10A1707F0
gworld func 0x1028791CC
gworld data 0x10A171A00

pubgm gl 3.6
gobject 0x10A066520
gname func 0x1045AA3E8
gname data 0x109BB4440
gengine 0x10A27AA70
gworld func 0x1029126C4
gworld data 0x10A27BC80

pubgm tw 3.6
gobject 0x10A23DBA0
gname func 0x104742830
gname data 0x109D8B830
gengine 0x10A4520F0
gworld func 0x102AAAB0C
gworld data 0x10A453300

pubgm kr 3.6
gobject 0x10A267CA0
gname func 0x10476F14C
gname data 0x109DB5940
gengine 0x10A47C1F0
gworld func 0x102AD71F8
gworld data 0x10A47D400

*/