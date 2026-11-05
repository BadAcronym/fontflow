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

    FF_ASSERT(end >= start, "parser error, offsetNext (%"PRIu32") cannot be smaller "
              "than offset (%"PRIu32").", end, start);

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
    ++numberOfPoints;

    u08 *flags = 0;
    pdArrReserve(flags, numberOfPoints);

    for(u16 i = 0; i < numberOfPoints; ++i)
    {
        u08 flag = glyphData[start++];
        FF_ASSERT((flag & FF_FLAG_RESERVED) == 0, "flag reserved bit needs to be 0.");

        pdArrPush(flags, flag);

        if(flag & FF_FLAG_REPEAT)
        {
            u08 repeat = glyphData[start++];
            i += repeat;

            for(u08 j = 0; j < repeat; ++j)
            {
                pdArrPush(flags, flag);
            }
        }
    }

    FF_ASSERT(pdArrSize(flags) == numberOfPoints,
              "must read exactly numberOfPoints flags. read: %"
              PRIu64", expected: %"PRIu16,
              pdArrSize(flags), numberOfPoints);

    i16 xCoordPrev = 0;
    for(u16 i = 0; i < numberOfPoints; ++i)
    {
        i16 xCoord = 0;
        if(flags[i] & FF_FLAG_X_SHORT_VECTOR)
        {
            xCoord = glyphData[start++];

            if(!(flags[i] & FF_FLAG_X_IS_SAME_POSX))
            {
                xCoord *= -1;
            }
        }
        else if(flags[i] & FF_FLAG_X_IS_SAME_POSX)
        {
            xCoord = 0;
        }
        else
        {
            xCoord  = (i16)(glyphData[start++] << 8);
            xCoord |= (i16)(glyphData[start++]);
        }

        FF_TRACE("xCoord: %"PRIi16, xCoord + xCoordPrev);

        FF_ASSERT(xCoord + xCoordPrev <= glyf.xMax,
                  "coordinate (%"PRIi16") cannot be bigger than xMax (%"PRIi16")",
                  xCoord + xCoordPrev, glyf.xMax);

        FF_ASSERT(xCoord + xCoordPrev >= glyf.xMin,
                  "coordinate (%"PRIi16") cannot be smaller than xMin (%"PRIi16")",
                  xCoord + xCoordPrev, glyf.xMin);

        pdArrPush(glyf.xCoordinates, xCoord + xCoordPrev);
        xCoordPrev += xCoord;
    }

    i16 yCoordPrev = 0;
    for(u16 i = 0; i < numberOfPoints; ++i)
    {
        i16 yCoord = 0;
        if(flags[i] & FF_FLAG_Y_SHORT_VECTOR)
        {
            yCoord = glyphData[start++];

            if(!(flags[i] & FF_FLAG_Y_IS_SAME_POSY))
            {
                yCoord *= -1;
            }
        }
        else if(flags[i] & FF_FLAG_Y_IS_SAME_POSY)
        {
            yCoord = 0;
        }
        else
        {
            yCoord  = (i16)(glyphData[start++] << 8);
            yCoord |= (i16)(glyphData[start++]);
        }

        FF_TRACE("yCoord: %"PRIi16, yCoord + yCoordPrev);

        FF_ASSERT(yCoord + yCoordPrev <= glyf.yMax,
                  "coordinate (%"PRIi16") cannot be bigger than yMax (%"PRIi16")",
                  yCoord + yCoordPrev, glyf.yMax);

        FF_ASSERT(yCoord + yCoordPrev >= glyf.yMin,
                  "coordinate (%"PRIi16") cannot be smaller than yMin (%"PRIi16")",
                  yCoord + yCoordPrev, glyf.yMin);

        pdArrPush(glyf.yCoordinates, yCoord + yCoordPrev);
        yCoordPrev += yCoord;
    }

    // TODO: assert that we made it to the end as desired

    return glyf;
}
