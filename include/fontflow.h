#ifndef FONTFLOW_H
#define FONTFLOW_H

#include "pd_int_types.h"
#include "pd_string_view.h"

#define f_internal static
#define s_global   static

#define SFNT_START 12

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

typedef struct FontData
{
    u08        *data;
    TableEntry *cmap;
}
FontData;

#define FF_PLATFORM_ID_UNICODE   0
#define FF_PLATFORM_ID_MACINTOSH 1
#define FF_PLATFORM_ID_MICROSOFT 3

#endif
