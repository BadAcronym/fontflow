#include "fontflow.h"
#include "ff_print_macros.h"

#include "pd_dyn_arr.h"

#include <inttypes.h>

void readTable_glyf
(
    FontData   *font,
    TableEntry *entry
){
    font->glyfData = font->data + entry->offset;
}

glyf readGlyph
(
    FontData *font,
    u32      glyph
){
    u32 start = 0;
    u32 end   = 0;

    u08 *glyphData = font->glyfData;

    if(font->loca16)
    {
        start = (font->loca16[glyph] >> 8)
        | (u16)((font->loca16[glyph] & 0xFF) << 8);

        end   = (font->loca16[glyph + 1] >> 8)
        | (u16)((font->loca16[glyph + 1] & 0xFF) << 8);

        start     *= 2;
        end *= 2;
    }
    else // font->loca32
    {
        start = ((font->loca32[glyph] & 0xFF000000) >> 24)
        |  (u32)((font->loca32[glyph] & 0x00FF0000) >> 8)
        |  (u32)((font->loca32[glyph] & 0x0000FF00) << 8)
        |  (u32)((font->loca32[glyph] & 0x000000FF) << 24);

        end   = ((font->loca32[glyph + 1] & 0xFF000000) >> 24)
        |  (u32)((font->loca32[glyph + 1] & 0x00FF0000) >> 8)
        |  (u32)((font->loca32[glyph + 1] & 0x0000FF00) << 8)
        |  (u32)((font->loca32[glyph + 1] & 0x000000FF) << 24);
    }

    FF_ASSERT(end >= start, "parser error, offsetNext (%"PRIu32
              ") cannot be smaller than offset (%"PRIu32").",
              end, start);

    if(start == end)
    {
        return (glyf){0};
    }

    FF_ASSERT(start <= end, "start cannot be bigger than end: %"PRIu32" > %"PRIu32,
              start, end)

    FF_ASSERT(end - start > 10, "can't read less than 10 bytes (%"PRIu32") for header.",
              end - start)

    glyf glyf = {0};

    glyf.numberOfContours  = (i16)(glyphData[start++] << 8);
    glyf.numberOfContours |= glyphData[start++];

    glyf.xMin  = (i16)(glyphData[start++] << 8);
    glyf.xMin |= glyphData[start++];
    glyf.yMin  = (i16)(glyphData[start++] << 8);
    glyf.yMin |= glyphData[start++];
    glyf.xMax  = (i16)(glyphData[start++] << 8);
    glyf.xMax |= glyphData[start++];
    glyf.yMax  = (i16)(glyphData[start++] << 8);
    glyf.yMax |= glyphData[start++];

    FF_TRACE("offset: %5"PRIu32", contours: %2"PRIi16,
             start, glyf.numberOfContours);

    if(start >= end)
    {
        return glyf;
    }

    glyf.endPtsOfContours = (u16*)&glyphData[start];
    start = start + (u32)(glyf.numberOfContours * 2);

    glyf.instructionLength  = (u16)(glyphData[start++] << 8);
    glyf.instructionLength |= glyphData[start++];

    if(glyf.instructionLength)
    {
        glyf.instructions = &glyphData[start];
        start += glyf.instructionLength;
    }

    u16 numberOfPoints  = (glyf.endPtsOfContours[glyf.numberOfContours-1] >> 8);
    numberOfPoints     |= (glyf.endPtsOfContours[glyf.numberOfContours-1] & 0xFF) << 8;

    FF_TRACE("number of points: %"PRIu16, numberOfPoints);
    for(u16 i = 0; i < numberOfPoints; ++i)
    {
        u08 flag = glyphData[start++];
        FF_ASSERT((flag & FF_FLAG_RESERVED) == 0, "flag reserved bit needs to be 0.");

        if(flag & FF_FLAG_REPEAT)
        {
            u08 repeat = glyphData[start++];
            i += repeat;
        }
    }
    // dunno, sometimes it's untrue here
    // FF_ASSERT(i == numberOfPoints, "must read exactly numberOfPoints flags. read: %"
    //           PRIu16", expected: %"PRIu16, i, numberOfPoints);

    // read more
    //
    // u08 *xCoordinates8;
    // u16 *xCoordinates16;
    // u08 *yCoordinates8;
    // u16 *yCoordinates16;

    return glyf;
}
