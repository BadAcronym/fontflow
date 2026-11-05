#ifndef FONTFLOW_H
#define FONTFLOW_H

#include "pd_int_types.h"
#include "pd_string_view.h"

#define f_internal static
#define s_global   static

#define FF_FLAG_ON_CURVE_POINT 0x01
#define FF_FLAG_X_SHORT_VECTOR 0x02
#define FF_FLAG_Y_SHORT_VECTOR 0x04
#define FF_FLAG_REPEAT         0x08
#define FF_FLAG_X_IS_SAME_POSX 0x10
#define FF_FLAG_Y_IS_SAME_POSY 0x20
#define FF_FLAG_OVERLAP_SIMPLE 0x40
#define FF_FLAG_RESERVED       0x80

s_global StringView GDEFIdent = { .data = "GDEF", .size = 4 };
s_global StringView GPOSIdent = { .data = "GPOS", .size = 4 };
s_global StringView GSUBIdent = { .data = "GSUB", .size = 4 };

s_global StringView acntIdent = { .data = "acnt", .size = 4 };
s_global StringView ankrIdent = { .data = "ankr", .size = 4 };
s_global StringView avarIdent = { .data = "avar", .size = 4 };
s_global StringView bdatIdent = { .data = "bdat", .size = 4 };
s_global StringView bhedIdent = { .data = "bhed", .size = 4 };
s_global StringView blocIdent = { .data = "bloc", .size = 4 };
s_global StringView bslnIdent = { .data = "bsln", .size = 4 };
s_global StringView cmapIdent = { .data = "cmap", .size = 4 };
s_global StringView cvarIdent = { .data = "cvar", .size = 4 };
s_global StringView cvtIdent  = { .data = "cvt ", .size = 4 };
s_global StringView EBSCIdent = { .data = "EBSC", .size = 4 };
s_global StringView fdscIdent = { .data = "fdsc", .size = 4 };
s_global StringView featIdent = { .data = "feat", .size = 4 };
s_global StringView FFTMIdent = { .data = "FFTM", .size = 4 };
s_global StringView fmtxIdent = { .data = "fmtx", .size = 4 };
s_global StringView fondIdent = { .data = "fond", .size = 4 };
s_global StringView fpgmIdent = { .data = "fpgm", .size = 4 };
s_global StringView fvarIdent = { .data = "fvar", .size = 4 };
s_global StringView gaspIdent = { .data = "gasp", .size = 4 };
s_global StringView gcidIdent = { .data = "gcid", .size = 4 };
s_global StringView glyfIdent = { .data = "glyf", .size = 4 };
s_global StringView gvarIdent = { .data = "gvar", .size = 4 };
s_global StringView hdmxIdent = { .data = "hdmx", .size = 4 };
s_global StringView headIdent = { .data = "head", .size = 4 };
s_global StringView hheaIdent = { .data = "hhea", .size = 4 };
s_global StringView hmtxIdent = { .data = "hmtx", .size = 4 };
s_global StringView hvglIdent = { .data = "hvgl", .size = 4 };
s_global StringView hvpmIdent = { .data = "hvpm", .size = 4 };
s_global StringView justIdent = { .data = "just", .size = 4 };
s_global StringView kernIdent = { .data = "kern", .size = 4 };
s_global StringView kerxIdent = { .data = "kerx", .size = 4 };
s_global StringView lcarIdent = { .data = "lcar", .size = 4 };
s_global StringView locaIdent = { .data = "loca", .size = 4 };
s_global StringView ltagIdent = { .data = "ltag", .size = 4 };
s_global StringView MATHIdent = { .data = "MATH", .size = 4 };
s_global StringView maxpIdent = { .data = "maxp", .size = 4 };
s_global StringView metaIdent = { .data = "meta", .size = 4 };
s_global StringView mortIdent = { .data = "mort", .size = 4 };
s_global StringView morxIdent = { .data = "morx", .size = 4 };
s_global StringView nameIdent = { .data = "name", .size = 4 };
s_global StringView opbdIdent = { .data = "opbd", .size = 4 };
s_global StringView OS2Ident  = { .data = "OS/2", .size = 4 };
s_global StringView postIdent = { .data = "post", .size = 4 };
s_global StringView prepIdent = { .data = "prep", .size = 4 };
s_global StringView propIdent = { .data = "prop", .size = 4 };
s_global StringView sbixIdent = { .data = "sbix", .size = 4 };
s_global StringView trakIdent = { .data = "trak", .size = 4 };
s_global StringView vheaIdent = { .data = "vhea", .size = 4 };
s_global StringView vmtxIdent = { .data = "vmtx", .size = 4 };
s_global StringView xrefIdent = { .data = "xref", .size = 4 };
s_global StringView ZapfIdent = { .data = "Zapf", .size = 4 };

typedef struct FFL
{
    i16 before;
    // unsure whether 'after' is u16 or i16, intuitively it's u16 tho
    u16 after;
}
FFL;

typedef struct SHR
{
    u16 firstCode;
    u16 entryCount;
    i16 idDelta;
    u16 idRangeOffset;
}
SHR;

typedef struct SMG
{
    u32 startCharCode;
    u32 endCharCode;
    u32 startGlyphID;
}
SMG;

typedef struct CMG
{
    u32 startCharCode;
    u32 endCharCode;
    u32 glyphID;
}
CMG;

typedef struct VSL
{
    u32 varSelector : 24;
    u32 defaultUVSOffset;
    u32 nonDefaultUVSOffset;
}
VSL;

typedef struct OffsetTable
{
    u32 scalerType;
    u16 numTables;
    u16 searchRange;
    u16 entrySelector;
    u16 rangeShift;
}
OffsetTable;

typedef struct TableEntry
{
    StringView tag;
    u32        checksum;
    u32        offset;
    u32        length;
}
TableEntry;

typedef struct CmapSubtableInfo
{
    u08 *subtableData;
    u64 subIndex;
    u16 subtableFormat;
    u16 platformID;
    u16 platformSpecificID;
}
CmapSubtableInfo;

typedef struct cmap
{
    u16 subtableFormat;
    u16 platformID;
    u16 platformSpecificID;

    u32 length;
    u32 language;

    // format 0
    u08 glyphIdArray8[256];

    // format 2
    u16 subHeaderKeys[256];
    SHR *subHeaders;

    // format 4
    u16 segCountX2;
    u16 searchRange;
    u16 entrySelector;
    u16 rangeShift;
    u16 *endCode;
    u16 *startCode;
    i16 *idDelta;
    u16 *idRangeOffset;

    // format 6
    u16 firstCode;
    u16 entryCount;

    // format 8
    u08 is32[8192];

    // format 8, 12, 13
    u32 numGroups;
    SMG *sequentialGroups;
    CMG *constantGroups;

    // format 10
    u32 startCharCode;
    u32 numChars;

    // format 14
    u32 numVarSelectorRecords;
    VSL *varSelector;

    // format 2, 4, 6, 10
    u16 *glyphIdArray16;
}
cmap;

typedef struct glyf
{
    i16 numberOfContours;
    i16 xMin;
    i16 yMin;
    i16 xMax;
    i16 yMax;
    u16 *endPtsOfContours;
    u16 instructionLength;
    u08 *instructions;
    u08 *flags;
    i16 *xCoordinates;
    i16 *yCoordinates;
}
glyf;

typedef struct head
{
    u16 majorVersion;
    u16 minorVersion;
    FFL fontRevision;
    u32 checksumAdjustment;
    u32 magicNumber;
    u16 flags;
    u16 unitsPerEm;
    i64 created;
    i64 modified;
    i16 xMin;
    i16 yMin;
    i16 xMax;
    i16 yMax;
    u16 macStyle;
    u16 lowestRecPPEM;
    i16 fontDirectionHint;
    i16 indexToLocFormat;
    i16 glyphDataFormat;
}
head;

typedef struct maxp
{
    FFL version;
    u16 numGlyphs;
    u16 maxPoints;
    u16 maxContours;
    u16 maxCompositePoints;
    u16 maxCompositeContours;
    u16 maxZones;
    u16 maxTwilightPoints;
    u16 maxStorage;
    u16 maxFunctionDefs;
    u16 maxInstructionDefs;
    u16 maxStackElements;
    u16 maxSizeOfInstructions;
    u16 maxComponentElements;
    u16 maxComponentDepth;
}
maxp;

typedef struct FontData
{
    u08  *data;
    cmap *cmap;
    u08  *glyfData;
    head *head;
    u16  *loca16;
    u32  *loca32;
    maxp *maxp;
}
FontData;

extern void readTable_cmap
(
    FontData   *font,
    TableEntry *entry
);

extern void readTable_glyf
(
    FontData   *font,
    TableEntry *entry
);

extern glyf readGlyph
(
    FontData *font,
    u32      glyph
);

#define FF_PLATFORM_ID_UNICODE   0
#define FF_PLATFORM_ID_MACINTOSH 1
#define FF_PLATFORM_ID_MICROSOFT 3

#endif
