#ifndef FONTFLOW_H
#define FONTFLOW_H

#include "pd_int_types.h"

#define f_internal static
#define s_global   static

typedef struct OffsetTable
{
    u32 scalerType;
    u16 numTables;
    u16 searchRange;
    u16 entrySelector;
    u16 rangeShift;
}
OffsetTable;

#endif
