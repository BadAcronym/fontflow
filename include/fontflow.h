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

typedef struct FontData
{
    u08        *data;
    TableEntry *cmap;
}
FontData;

#endif
