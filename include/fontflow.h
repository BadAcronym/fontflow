#ifndef FONTFLOW_H
#define FONTFLOW_H

#include "pd_int_types.h"
#include "pd_string_view.h"

#define f_internal static
#define s_global   static

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

typedef struct head
{
}
head;

typedef struct loca
{
}
loca;

typedef struct maxp
{
}
maxp;

typedef struct FontData
{
    u08  *data;
    cmap *cmap;
    head *head;
    loca *loca;
    maxp *maxp;
}
FontData;

#define FF_PLATFORM_ID_UNICODE   0
#define FF_PLATFORM_ID_MACINTOSH 1
#define FF_PLATFORM_ID_MICROSOFT 3

#endif
