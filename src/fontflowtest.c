#include "fontflow.h"
#include "ff_print_macros.h"

#include <inttypes.h>

#define FONT_PATH "fonts/notosansgothic/NotoSansGothic-Regular.ttf"

i32 main
(
    void
){
    FILE *file = fopen(FONT_PATH, "rb");
    if(!file)
    {
        FF_ERROR("failed to open font file: %s", FONT_PATH);
    }

    OffsetTable ofsTable = {0};

    u08 byte = 0;
    for(u08 i = 0; i < 4; ++i)
    {
        if(fread(&byte, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read scalerType from offsetTable.");
        }

        ofsTable.scalerType |= (u32)(byte << (3 - i));
    }

    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&ofsTable.numTables, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read numTables from offsetTable.");
        }

        ofsTable.scalerType |= (u32)(byte << (1 - i));
    }

    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&ofsTable.searchRange, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read searchRange from offsetTable.");
        }

        ofsTable.searchRange |= (byte << (1 - i));
    }
    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&byte, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read entrySelector from offsetTable.");
        }

        ofsTable.entrySelector |= (byte << (1 - i));
    }
    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&byte, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read rangeShift from offsetTable.");
        }

        ofsTable.rangeShift |= (byte << (1 - i));
    }

    FF_DEBUG("reading %"PRIu16" tables.", ofsTable.numTables);
    for(u16 i = 0; i < ofsTable.numTables; ++i)
    {
        // read 4-byte ident
        // if()
        // {
        // }
    }

    fclose(file);
}
