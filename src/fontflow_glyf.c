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
    u32      glyphIndex
){
    u32 start = 0;
    u32 end   = 0;

    u08 *glyphData = font->glyfData;

    if(font->loca16)
    {
        start = (font->loca16[glyphIndex] >> 8)
        | (u16)((font->loca16[glyphIndex] & 0xFF) << 8);

        end   = (font->loca16[glyphIndex + 1] >> 8)
        | (u16)((font->loca16[glyphIndex + 1] & 0xFF) << 8);

        start     *= 2;
        end *= 2;
    }
    else // font->loca32
    {
        start = ((font->loca32[glyphIndex] & 0xFF000000) >> 24)
        |  (u32)((font->loca32[glyphIndex] & 0x00FF0000) >> 8)
        |  (u32)((font->loca32[glyphIndex] & 0x0000FF00) << 8)
        |  (u32)((font->loca32[glyphIndex] & 0x000000FF) << 24);

        end   = ((font->loca32[glyphIndex + 1] & 0xFF000000) >> 24)
        |  (u32)((font->loca32[glyphIndex + 1] & 0x00FF0000) >> 8)
        |  (u32)((font->loca32[glyphIndex + 1] & 0x0000FF00) << 8)
        |  (u32)((font->loca32[glyphIndex + 1] & 0x000000FF) << 24);
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

    glyf glyph = {0};

    glyph.numberOfContours  = (i16)(glyphData[start++] << 8);
    glyph.numberOfContours |= glyphData[start++];

    if(glyph.numberOfContours == -1)
    {
        FF_ERROR("composite glyphs are unimplemented.");
        return glyph;
    }

    glyph.xMin  = (i16)(glyphData[start++] << 8);
    glyph.xMin |= glyphData[start++];
    glyph.yMin  = (i16)(glyphData[start++] << 8);
    glyph.yMin |= glyphData[start++];
    glyph.xMax  = (i16)(glyphData[start++] << 8);
    glyph.xMax |= glyphData[start++];
    glyph.yMax  = (i16)(glyphData[start++] << 8);
    glyph.yMax |= glyphData[start++];

    FF_TRACE("offset: %5"PRIu32", contours: %2"PRIi16,
             start, glyph.numberOfContours);

    if(start > end)
    {
        goto error;
    }

    glyph.endPtsOfContours = (u16*)&glyphData[start];
    start = start + (u32)(glyph.numberOfContours * 2);

    if(start > end)
    {
        goto error;
    }

    if(start > end)
    {
        goto error;
    }

    glyph.instructionLength  = (u16)(glyphData[start++] << 8);
    glyph.instructionLength |= glyphData[start++];

    if(start > end)
    {
        goto error;
    }

    if(glyph.instructionLength)
    {
        glyph.instructions = &glyphData[start];
        start += glyph.instructionLength;
    }

    if(start > end)
    {
        goto error;
    }

    u16 numberOfPoints  = (glyph.endPtsOfContours[glyph.numberOfContours-1] >> 8);
    numberOfPoints     |= (glyph.endPtsOfContours[glyph.numberOfContours-1] & 0xFF) << 8;
    ++numberOfPoints;

    u08 *flags = 0;
    pdArrReserve(flags, numberOfPoints);

    for(u16 i = 0; i < numberOfPoints; ++i)
    {
        if(start > end)
        {
            FF_ERROR("read past the end of the glyph.");
        }
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
        if(start > end)
        {
            goto error;
        }
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

        FF_ASSERT(xCoord + xCoordPrev <= glyph.xMax,
                  "coordinate (%"PRIi16") cannot be bigger than xMax (%"PRIi16")",
                  xCoord + xCoordPrev, glyph.xMax);

        FF_ASSERT(xCoord + xCoordPrev >= glyph.xMin,
                  "coordinate (%"PRIi16") cannot be smaller than xMin (%"PRIi16")",
                  xCoord + xCoordPrev, glyph.xMin);

        pdArrPush(glyph.xCoordinates, xCoord + xCoordPrev);
        xCoordPrev += xCoord;
    }

    i16 yCoordPrev = 0;
    for(u16 i = 0; i < numberOfPoints; ++i)
    {
        if(start > end)
        {
            goto error;
        }
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

        FF_ASSERT(yCoord + yCoordPrev <= glyph.yMax,
                  "coordinate (%"PRIi16") cannot be bigger than yMax (%"PRIi16")",
                  yCoord + yCoordPrev, glyph.yMax);

        FF_ASSERT(yCoord + yCoordPrev >= glyph.yMin,
                  "coordinate (%"PRIi16") cannot be smaller than yMin (%"PRIi16")",
                  yCoord + yCoordPrev, glyph.yMin);

        pdArrPush(glyph.yCoordinates, yCoord + yCoordPrev);
        yCoordPrev += yCoord;
    }

    #ifdef FF_PRI_TRACE
    for(u16 i = 0; i < numberOfPoints; ++i)
    {
        FF_TRACE("point: (%"PRIi16", %"PRIi16")",
                 glyph.xCoordinates[i], glyph.yCoordinates[i]);
    }
    #endif

    if(start > end)
    {
        goto error;
    }

    return glyph;

error:
    FF_ERROR("read past the end of the glyph.");
    return (glyf){0};
}
