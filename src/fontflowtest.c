#include "fontflow.h"

#include "ff_print_macros.h"

#include "pd_dyn_arr.h"
#include "pd_string_view.h"

#include <inttypes.h>

#define FONT_PATH "fonts/dejavu/DejaVuSans.ttf"

f_internal u16 getGlyphIndex
(
    cmap *cmap,
    u64  i,
    u16  c,
    u16  fixedStartCode
){
    i16 fixedIdDelta        = cmap->idDelta[i] >> 8;
    fixedIdDelta           |= (i16)((cmap->idDelta[i] & 0xFF) << 8);

    u16 fixedIdRangeOffset  = cmap->idRangeOffset[i] >> 8;
    fixedIdRangeOffset     |= (u16)((cmap->idRangeOffset[i] & 0xFF) << 8);

    if(!fixedIdRangeOffset)
    {
        return (u16)(fixedIdDelta + c);
    }

    u08 *glyphIndexAddr = (u08*)(fixedIdRangeOffset + 2 *
                                (c - fixedStartCode) + &cmap->idRangeOffset[i]);

    u16 glyphIndex  = (u16)(*glyphIndexAddr++ << 8);
    glyphIndex     |= *glyphIndexAddr;

    if(glyphIndex)
    {
        glyphIndex += fixedIdDelta;
    }

    return glyphIndex;
}

f_internal void readTable_glyf
(
    FontData   *font,
    TableEntry *entry
){
    font->glyfData = font->data + entry->offset;
}

f_internal void readTable_head
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *headData = font->data + entry->offset;

    font->head->majorVersion = 0;
    font->head->minorVersion = 0;

    font->head->majorVersion  = (u16)(headData[index++] << 8);
    font->head->majorVersion |= (u16)(headData[index++]);
    font->head->minorVersion  = (u16)(headData[index++] << 8);
    font->head->minorVersion |= (u16)(headData[index++]);

    font->head->fontRevision.before  = (i16)(headData[index++] << 8);
    font->head->fontRevision.before |= (i16)(headData[index++]);
    font->head->fontRevision.after   = (u16)(headData[index++] << 8);
    font->head->fontRevision.after  |= (u16)(headData[index++]);

    FF_TRACE("head version:          %"PRIx16".%"PRIx16,
             font->head->majorVersion, font->head->minorVersion);
    FF_TRACE("head revision:         %"PRIx16".%"PRIx16,
             font->head->fontRevision.before, font->head->fontRevision.after);

    font->head->checksumAdjustment = 0;
    for(u08 i = 0; i < 4; ++i)
    {
        font->head->checksumAdjustment |= (u32)(headData[index++]) << ((3 - i) * 8);
    }
    FF_TRACE("checksumAdj:           %"PRIu32, font->head->checksumAdjustment);

    font->head->magicNumber = 0;
    for(u08 i = 0; i < 4; ++i)
    {
        font->head->magicNumber |= (u32)(headData[index++]) << ((3 - i) * 8);
    }
    FF_TRACE("magicNumber:           0x%"PRIX32, font->head->magicNumber);

    FF_ASSERT(font->head->magicNumber == 0x5F0F3CF5, "magicNumber does not equal "
              "0x5F0F3CF5, instead is: 0x%"PRIX32, font->head->magicNumber);

    font->head->flags  = (u16)(headData[index++] << 8);
    font->head->flags |= (u16)(headData[index++]);
    FF_TRACE("flags:                 %"PRIu16, font->head->flags);

    font->head->unitsPerEm  = (u16)(headData[index++] << 8);
    font->head->unitsPerEm |= (u16)(headData[index++]);
    FF_TRACE("unitsPerEm:            %"PRIu16, font->head->unitsPerEm);

    font->head->created = 0;
    for(u08 i = 0; i < 8; ++i)
    {
        font->head->created |= (i64)(headData[index++]) << ((7 - i) * 8);
    }
    FF_TRACE("created:               %"PRIi64, font->head->created);

    font->head->modified = 0;
    for(u08 i = 0; i < 8; ++i)
    {
        font->head->modified |= (i64)(headData[index++]) << ((7 - i) * 8);
    }
    FF_TRACE("modified:              %"PRIi64, font->head->modified);

    font->head->xMin  = (i16)(headData[index++] << 8);
    font->head->xMin |= (i16)(headData[index++]);
    FF_TRACE("xMin:                  %"PRIi16, font->head->xMin);

    font->head->yMin  = (i16)(headData[index++] << 8);
    font->head->yMin |= (i16)(headData[index++]);
    FF_TRACE("yMin:                  %"PRIi16, font->head->yMin);

    font->head->xMax  = (i16)(headData[index++] << 8);
    font->head->xMax |= (i16)(headData[index++]);
    FF_TRACE("xMax:                  %"PRIi16, font->head->xMax);

    font->head->yMax  = (i16)(headData[index++] << 8);
    font->head->yMax |= (i16)(headData[index++]);
    FF_TRACE("yMax:                  %"PRIi16, font->head->yMax);

    font->head->macStyle  = (u16)(headData[index++] << 8);
    font->head->macStyle |= (u16)(headData[index++]);
    FF_TRACE("macStyle:              %"PRIu16, font->head->macStyle);

    font->head->lowestRecPPEM  = (u16)(headData[index++] << 8);
    font->head->lowestRecPPEM |= (u16)(headData[index++]);
    FF_TRACE("lowestRecPPEM:         %"PRIu16, font->head->lowestRecPPEM);

    font->head->fontDirectionHint  = (i16)(headData[index++] << 8);
    font->head->fontDirectionHint |= (i16)(headData[index++]);
    FF_TRACE("fontDirectionHint:     %"PRIi16, font->head->fontDirectionHint);

    font->head->indexToLocFormat  = (i16)(headData[index++] << 8);
    font->head->indexToLocFormat |= (i16)(headData[index++]);
    FF_TRACE("indexToLocFormat:      %"PRIi16, font->head->indexToLocFormat);

    font->head->glyphDataFormat  = (i16)(headData[index++] << 8);
    font->head->glyphDataFormat |= (i16)(headData[index++]);
    FF_TRACE("glyphDataFormat:       %"PRIi16, font->head->glyphDataFormat);
}

f_internal void readTable_hhea
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *hheaData = font->data + entry->offset;

    //
    FF_WARN("required table 'hhea' not implemented.");
    //
}

f_internal void readTable_hmtx
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *hmtxData = font->data + entry->offset;

    //
    FF_WARN("required table 'hmtx' not implemented.");
    //
}

f_internal void readTable_loca
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *locaData = font->data + entry->offset;

    if(font->head->indexToLocFormat == 1)
    {
        font->loca32 = (u32*)locaData;
        FF_TRACE("read offset size:      32");
    }
    else
    {
        font->loca16 = (u16*)locaData;
        FF_TRACE("read offset size:      16");
    }
}

f_internal void readTable_maxp
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *maxpData = font->data + entry->offset;

    FFL version = {0};

    version.before  = (i16)(maxpData[index++] << 8);
    version.before |= (i16)(maxpData[index++]);
    version.after   = (u16)(maxpData[index++] << 8);
    version.after  |= (u16)(maxpData[index++]);
    FF_TRACE("maxp version:          %"PRIx16".%"PRIx16,
             version.before, version.after);

    font->maxp->numGlyphs  = (u16)(maxpData[index++] << 8);
    font->maxp->numGlyphs |= (u16)(maxpData[index++]);
    FF_TRACE("numGlyphs:             %"PRIu16, font->maxp->numGlyphs);

    // if version is 0.5 (openType fonts with 'CFF ' table, we need to abort
    // reading past numGlyphs, because that's all that this table has.
    if(!version.before && version.after == 5)
    {
        return;
    }

    font->maxp->maxPoints  = (u16)(maxpData[index++] << 8);
    font->maxp->maxPoints |= (u16)(maxpData[index++]);
    FF_TRACE("maxPoints:             %"PRIu16, font->maxp->maxPoints);

    font->maxp->maxContours  = (u16)(maxpData[index++] << 8);
    font->maxp->maxContours |= (u16)(maxpData[index++]);
    FF_TRACE("maxContours:           %"PRIu16, font->maxp->maxContours);

    font->maxp->maxCompositePoints  = (u16)(maxpData[index++] << 8);
    font->maxp->maxCompositePoints |= (u16)(maxpData[index++]);
    FF_TRACE("maxCompositePoints:    %"PRIu16, font->maxp->maxCompositePoints);

    font->maxp->maxCompositeContours  = (u16)(maxpData[index++] << 8);
    font->maxp->maxCompositeContours |= (u16)(maxpData[index++]);
    FF_TRACE("maxCompositeContours:  %"PRIu16, font->maxp->maxCompositeContours);

    font->maxp->maxZones  = (u16)(maxpData[index++] << 8);
    font->maxp->maxZones |= (u16)(maxpData[index++]);
    FF_TRACE("maxZones:              %"PRIu16, font->maxp->maxZones);

    FF_ASSERT(font->maxp->maxZones == 2, "maxZones  %"PRIu16" is not 2.",
              font->maxp->maxZones);

    font->maxp->maxTwilightPoints  = (u16)(maxpData[index++] << 8);
    font->maxp->maxTwilightPoints |= (u16)(maxpData[index++]);
    FF_TRACE("maxTwilightPoints:     %"PRIu16, font->maxp->maxTwilightPoints);

    font->maxp->maxStorage  = (u16)(maxpData[index++] << 8);
    font->maxp->maxStorage |= (u16)(maxpData[index++]);
    FF_TRACE("maxStorage:            %"PRIu16, font->maxp->maxStorage);

    font->maxp->maxFunctionDefs  = (u16)(maxpData[index++] << 8);
    font->maxp->maxFunctionDefs |= (u16)(maxpData[index++]);
    FF_TRACE("maxFunctionDefs:       %"PRIu16, font->maxp->maxFunctionDefs);

    font->maxp->maxInstructionDefs  = (u16)(maxpData[index++] << 8);
    font->maxp->maxInstructionDefs |= (u16)(maxpData[index++]);
    FF_TRACE("maxInstructionDefs:    %"PRIu16, font->maxp->maxInstructionDefs);

    font->maxp->maxStackElements  = (u16)(maxpData[index++] << 8);
    font->maxp->maxStackElements |= (u16)(maxpData[index++]);
    FF_TRACE("maxStackElements:      %"PRIu16, font->maxp->maxStackElements);

    font->maxp->maxSizeOfInstructions  = (u16)(maxpData[index++] << 8);
    font->maxp->maxSizeOfInstructions |= (u16)(maxpData[index++]);
    FF_TRACE("maxSizeOfInstructions: %"PRIu16, font->maxp->maxSizeOfInstructions);

    font->maxp->maxComponentElements  = (u16)(maxpData[index++] << 8);
    font->maxp->maxComponentElements |= (u16)(maxpData[index++]);
    FF_TRACE("maxComponentElements:  %"PRIu16, font->maxp->maxComponentElements);

    font->maxp->maxComponentDepth  = (u16)(maxpData[index++] << 8);
    font->maxp->maxComponentDepth |= (u16)(maxpData[index++]);
    FF_TRACE("maxComponentDepth:     %"PRIu16, font->maxp->maxComponentDepth);
}

f_internal void readTable_name
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *nameData = font->data + entry->offset;

    //
    FF_WARN("required table 'name' not implemented.");
    //
}

f_internal void readTable_post
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *postData = font->data + entry->offset;

    //
    FF_WARN("required table 'post' not implemented.");
    //
}

f_internal glyf readGlyph
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
        |  (u16)((font->loca16[glyph] & 0xFF) << 8);

        end   = (font->loca16[glyph + 1] >> 8)
        | (u16)((font->loca16[glyph + 1] & 0xFF) << 8);

        start     *= 2;
        end *= 2;
    }
    else // font->loca32
    {
        start = ((font->loca32[glyph] & 0xFF000000) >> 24)
        |   (u32)((font->loca32[glyph] & 0x00FF0000) >> 8)
        |   (u32)((font->loca32[glyph] & 0x0000FF00) << 8)
        |   (u32)((font->loca32[glyph] & 0x000000FF) << 24);

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

    // FIX: we probably need to check more carefully here.
    // if(start >= end)
    // {
    //     return glyf;
    // }

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

int main
(
    void
){
    FILE *file = fopen(FONT_PATH, "rb");
    if(!file)
    {
        FF_ERROR("failed to open font file: %s", FONT_PATH);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    u64 filesize = (u64)ftell(file);
    fseek(file, 0, SEEK_SET);

    u08 *fileBuf = malloc(filesize);
    if(!fileBuf)
    {
        FF_ERROR("failed to allocate fileBuf of size %"PRIu64, filesize);
        return 2;
    }

    cmap cmap   = {0};
    head head   = {0};
    u16  loca16 = {0};
    u32  loca32 = {0};
    maxp maxp   = {0};

    FontData font = {0};
    font.data   = fileBuf;
    font.cmap   = &cmap;
    font.head   = &head;
    font.maxp   = &maxp;

    if(fread(fileBuf, 1, filesize, file) != filesize)
    {
        FF_ERROR("failed to read entire file into fileBuf.");
        fclose(file);
        return 3;
    }
    fclose(file);

    u08         byte     = 0;
    u64         index    = 0;
    OffsetTable ofsTable = {0};

    for(u08 i = 0; i < 4; ++i)
    {
        ofsTable.scalerType |= (u32)(fileBuf[index++]) << (3 - i) * 8;
    }
    for(u08 i = 0; i < 2; ++i)
    {
        ofsTable.numTables |= (u32)(fileBuf[index++]) << (1 - i) * 8;
    }
    for(u08 i = 0; i < 2; ++i)
    {
        ofsTable.searchRange |= (fileBuf[index++]) << (1 - i) * 8;
    }
    for(u08 i = 0; i < 2; ++i)
    {
        ofsTable.entrySelector |= (fileBuf[index++]) << (1 - i) * 8;
    }
    for(u08 i = 0; i < 2; ++i)
    {
        ofsTable.rangeShift |= (fileBuf[index++]) << (1 - i) * 8;
    }

    FF_DEBUG("reading %"PRIu16" tables.", ofsTable.numTables);
    for(u16 i = 0; i < ofsTable.numTables; ++i)
    {
        char tagBuf[4] = {0};

        TableEntry table = {0};

        table.tag.data = tagBuf;
        table.tag.size = 4;

        for(u08 j = 0; j < 4; ++j)
        {
            tagBuf[j] = (char)fileBuf[index++];
        }
        FF_DEBUG("\nread tag: '"PRI_SV"'", ARG_SV(table.tag));

        for(u08 j = 0; j < 4; ++j)
        {
            table.checksum |= (u32)(fileBuf[index++]) << (3 - j) * 8;
        }
        FF_TRACE("read table checksum:   0x%"PRIX32, table.checksum);

        for(u08 j = 0; j < 4; ++j)
        {
            table.offset |= (u32)(fileBuf[index++]) << (3 - j) * 8;
        }
        FF_TRACE("read table offset:     %"PRIu32, table.offset);

        for(u08 j = 0; j < 4; ++j)
        {
            table.length |= (u32)(fileBuf[index++]) << (3 - j) * 8;
        }
        FF_TRACE("read table length:     %"PRIu32, table.length);

        if(pdSVSame(table.tag, GDEFIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, GPOSIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, GSUBIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, acntIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, ankrIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, avarIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, bdatIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, bhedIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, blocIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, bslnIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, cmapIdent))
        {
            readTable_cmap(&font, &table);
        }
        else if(pdSVSame(table.tag, cvarIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, cvtIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, EBSCIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, fdscIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, featIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, FFTMIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, fmtxIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, fondIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, fpgmIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, fvarIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, gaspIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, gcidIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, glyfIdent))
        {
            readTable_glyf(&font, &table);
        }
        else if(pdSVSame(table.tag, gvarIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, hdmxIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, headIdent))
        {
            readTable_head(&font, &table);
        }
        else if(pdSVSame(table.tag, hheaIdent))
        {
            readTable_hhea(&font, &table);
        }
        else if(pdSVSame(table.tag, hmtxIdent))
        {
            readTable_hmtx(&font, &table);
        }
        else if(pdSVSame(table.tag, hvglIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, hvpmIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, justIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, kernIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, kerxIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, lcarIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, locaIdent))
        {
            readTable_loca(&font, &table);
        }
        else if(pdSVSame(table.tag, ltagIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, MATHIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, maxpIdent))
        {
            readTable_maxp(&font, &table);
        }
        else if(pdSVSame(table.tag, metaIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, mortIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, morxIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, nameIdent))
        {
            readTable_name(&font, &table);
        }
        else if(pdSVSame(table.tag, opbdIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, OS2Ident))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, postIdent))
        {
            readTable_post(&font, &table);
        }
        else if(pdSVSame(table.tag, prepIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, propIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, sbixIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, trakIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, vheaIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, vmtxIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, xrefIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, ZapfIdent))
        {
            // ignore its data for now
        }
        else
        {
            FF_ERROR("unknown tag: '"PRI_SV"'", ARG_SV(table.tag));
            goto error;
        }
    }

    // CURRENT: somehow make sense of the glyph index array?
    char *alphabet = 0;
    for(u16 i = 0; i < 26; ++i)
    {
        pdArrPush(alphabet, (char)('A' + i));
    }

    FF_TRACE("");
    u16 *startCode = font.cmap->startCode;
    u16 *endCode   = font.cmap->endCode;
    u16 segCount   = font.cmap->segCountX2 / 2;
    for(u16 i = 0; i < segCount; ++i)
    {
        u16 j = 0;
        while(j < pdArrSize(alphabet))
        {
            u16 fixedStartCode  = (startCode[i] >> 8);
            fixedStartCode     |= (startCode[i] & 0xFF) << 8;
            u16 fixedEndCode    = (endCode[i] >> 8);
            fixedEndCode       |= (endCode[i] & 0xFF) << 8;

            u16 curr = (u16)alphabet[j];
            if(fixedStartCode <= curr && fixedEndCode >= curr)
            {
                u16 glyph = getGlyphIndex(font.cmap, i, curr, fixedStartCode);

                FF_DEBUG("glyphIndex of '%c' is %3"PRIu16".", curr, glyph);

                FF_ASSERT(glyph < font.maxp->numGlyphs, "glyphIndex of %"PRIu16
                          " is outside the valid range (< %"PRIu16")",
                          glyph, font.maxp->numGlyphs);

                glyf glyf = readGlyph(&font, glyph);

                pdArrRemove(alphabet, j);
            }
            else
            {
                ++j;
            }
        }
    }

error:
    free(fileBuf);
}
