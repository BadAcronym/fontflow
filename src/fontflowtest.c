#include "fontflow.h"
#include "ff_print_macros.h"

#include "pd_dyn_arr.h"
#include "pd_string_view.h"

#include <inttypes.h>

#define FONT_PATH "fonts/dejavu/DejaVuSans.ttf"

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

f_internal bool verifyIDs
(
    u16 platformID,
    u16 platformSpecificID
){
    FF_ASSERT(platformID != 2, "platformID of 2 is reserved.");

    if(platformID == FF_PLATFORM_ID_UNICODE)
    {
        if(platformSpecificID > 6 && platformSpecificID != 10)
        {
            FF_ERROR("platform ID is 0 (unicode) but platformSpecificID is %"PRIu16
                     ", which is not a valid identifier (0 - 6 | 10).",
                     platformSpecificID);
            return false;
        }
    }
    else if(platformID == FF_PLATFORM_ID_MACINTOSH)
    {
        FF_DEBUG("TODO: verify Macintosh QuickDraw script code");
    }
    else if(platformID == FF_PLATFORM_ID_MICROSOFT)
    {
        if(platformSpecificID > 5 && platformSpecificID != 10)
        {
            FF_ERROR("platform ID is 3 (microsoft) but platformSpecificID is %"PRIu16
                     ", which is not a valid identifier (0 - 5 | 10).",
                     platformSpecificID);
            return false;
        }
    }

    return true;
}

f_internal void readCmapFormat_0
(
    u08 *subtableData,
    u64 *subIndex
){
    u08 *glyphIndexArray = subtableData + (*subIndex);

    FF_WARN("cmap subtableFormat 0 unimplemented.");
}

f_internal void readCmapFormat_2
(
    u08 *subtableData,
    u64 *subIndex
){
    // u16 subHeaderKeys[256]
    // u16 * 4 subHeaders[variable]

    FF_WARN("cmap subtableFormat 2 unimplemented.");
}

// FIXME: I think something is still wrong here :(
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

f_internal void readCmapFormat_4
(
    FontData         *font,
    CmapSubtableInfo *info
){
    u08 *subtableData = info->subtableData;
    u64 *subIndex     = &info->subIndex;

    FF_TRACE("\nreading cmap format 4 @ %p.", subtableData);

    font->cmap->segCountX2  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap->segCountX2 |= subtableData[(*subIndex)++];
    u16 segCount = font->cmap->segCountX2 / 2;
    FF_TRACE("segCount:              %"PRIu16, segCount);

    font->cmap->searchRange  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap->searchRange |= subtableData[(*subIndex)++];
    FF_TRACE("searchRange:           %"PRIu16, font->cmap->searchRange);

    // assert that searchRange is 2 * (2**floor(log2(segCount)))

    font->cmap->entrySelector  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap->entrySelector |= subtableData[(*subIndex)++];
    FF_TRACE("entrySelector:         %"PRIu16, font->cmap->entrySelector);

    // assert that entrySelector is log2(searchRange/2)

    font->cmap->rangeShift  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap->rangeShift |= subtableData[(*subIndex)++];
    FF_TRACE("rangeShift:            %"PRIu16, font->cmap->rangeShift);

    FF_ASSERT(font->cmap->rangeShift == font->cmap->segCountX2 -
              font->cmap->searchRange,
              "rangeshift was expected to be %"PRIu16", instead got: %"PRIu16,
              font->cmap->segCountX2 - font->cmap->searchRange,
              font->cmap->rangeShift);

    font->cmap->endCode = (u16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    u16 reservedPad  = (u16)(subtableData[(*subIndex)++] << 8);
    reservedPad     |= subtableData[(*subIndex)++];
    FF_TRACE("reservedPad:           %"PRIu16, reservedPad);

    FF_ASSERT(!reservedPad, "reservedPad in cmap subtable format 4 needs to be 0.");

    font->cmap->startCode = (u16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    font->cmap->idDelta = (i16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    font->cmap->idRangeOffset = (u16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    font->cmap->glyphIdArray16 = (u16*)(&subtableData[(*subIndex)]);
}

f_internal void readCmapFormat_6
(
    u08 *subtableData,
    u64 *subIndex
){
    FF_WARN("cmap subtableFormat 6 unimplemented.");
}

f_internal void readCmapFormat_8
(
    u08 *subtableData,
    u64 *subIndex
){
    FF_WARN("cmap subtableFormat 8 unimplemented.");
}

f_internal void readCmapFormat_10
(
    u08 *subtableData,
    u64 *subIndex
){
    FF_WARN("cmap subtableFormat 10 unimplemented.");
}

f_internal void readCmapFormat_12
(
    u08 *subtableData,
    u64 *subIndex
){
    u32 nGroups = 0;
    for(u08 i = 0; i < 4; ++i)
    {
        nGroups |= (u32)subtableData[(*subIndex)++] << (3 - i) * 8;
    }
    FF_TRACE("nGroups:             %"PRIu32, nGroups);
    FF_WARN("cmap subtableFormat 12 unimplemented.");
}

f_internal void readCmapFormat_13
(
    u08 *subtableData,
    u64 *subIndex
){
    FF_WARN("cmap subtableFormat 13 unimplemented.");
}

f_internal void readCmapFormat_14
(
    u08 *subtableData,
    u64 *subIndex
){
    FF_WARN("cmap subtableFormat 14 unimplemented.");
}

f_internal void readTable_cmap
(
    FontData   *font,
    TableEntry *entry
){
    u64 index     = 0;
    u08 *cmapData = font->data + entry->offset;

    u16 cmapVersion  = (u16)(cmapData[index++] << 8);
    cmapVersion     |= cmapData[index++];

    u16 cmapSubtables  = (u16)(cmapData[index++] << 8);
    cmapSubtables     |= cmapData[index++];

    FF_TRACE("cmap version:          %"PRIu16, cmapVersion);
    FF_ASSERT(!cmapVersion, "cmap version must be set to 0.");

    FF_TRACE("cmap subtables:        %"PRIu16, cmapSubtables);

    CmapSubtableInfo *infos = 0;

    for(u16 i = 0; i < cmapSubtables; ++i)
    {
        CmapSubtableInfo info = {0};
        FF_TRACE("\ncmap subtable index:   %"PRIu16, i);

        info.platformID  = (u16)(cmapData[index++] << 8);
        info.platformID |= cmapData[index++];
        FF_TRACE("platformID:            %"PRIu16, info.platformID);

        info.platformSpecificID  = (u16)(cmapData[index++] << 8);
        info.platformSpecificID |= cmapData[index++];
        FF_TRACE("platformSpecificID:    %"PRIu16, info.platformSpecificID);

        u32 mappingOffset = 0;
        for(u08 j = 0; j < 4; ++j)
        {
            mappingOffset |= (u32)cmapData[index++] << (3 - j) * 8;
        }
        FF_TRACE("mappingOffset:         %"PRIu32, mappingOffset);

        if(!verifyIDs(info.platformID, info.platformSpecificID))
        {
            // parser error
            return;
        }

        info.subtableData    = cmapData + mappingOffset;
        info.subtableFormat  = (u16)(info.subtableData[info.subIndex++] << 8);
        info.subtableFormat |= info.subtableData[info.subIndex++];
        FF_TRACE("subtableData:          %p", info.subtableData);
        FF_TRACE("subtableFormat:        %"PRIu16, info.subtableFormat);

        pdArrPush(infos, info);
    }

    CmapSubtableInfo info = infos[0];

    // choose most preferrable subtable format to read
    u16 arrSize = (u16)(pdArrSize(infos));
    for(u16 i = 0; i < arrSize; ++i)
    {
        if(infos[i].subtableFormat == 4)
        {
            info = infos[i];
            goto chosen;
        }
    }

chosen:
    {
        u32 length   = 0;
        u32 language = 0;
        u16 reserved = 0;

        bool needsLenLang = info.subtableFormat == 0  || info.subtableFormat == 2 ||
                            info.subtableFormat == 4  || info.subtableFormat == 6;

        bool needsReserved = info.subtableFormat == 8  || info.subtableFormat == 10 ||
                             info.subtableFormat == 12 || info.subtableFormat == 12;

        bool needsLen32    = needsReserved || info.subtableFormat == 14;

        if(needsLenLang)
        {
            length  = (u16)(info.subtableData[info.subIndex++] << 8);
            length |= info.subtableData[info.subIndex++];
            FF_TRACE("length:                %"PRIu16, length);

            language  = (u16)(info.subtableData[info.subIndex++] << 8);
            language |= info.subtableData[info.subIndex++];
            FF_TRACE("language:              %"PRIu16, language);

            if(language && info.platformID != FF_PLATFORM_ID_MACINTOSH)
            {
                FF_ERROR("language code %"PRIu16" undefined for platform ID %"PRIu16".",
                         language, info.platformID);
            }
        }
        else if(needsReserved)
        {
            reserved  = (u16)(info.subtableData[info.subIndex++] << 8);
            reserved |= info.subtableData[info.subIndex++];
            FF_TRACE("reserved:              %"PRIu16, reserved);
        }

        if(needsLen32)
        {
            length = 0;
            for(u08 i = 0; i < 4; ++i)
            {
                length |= (u32)info.subtableData[info.subIndex++] << (3 - i) * 8;
            }
            FF_TRACE("length:                %"PRIu32, length);
        }
        if(needsReserved)
        {
            language = 0;
            for(u08 i = 0; i < 4; ++i)
            {
                language |= (u32)info.subtableData[info.subIndex++] << (3 - i) * 8;
            }
            FF_TRACE("language:              %"PRIu32, language);
        }

        font->cmap->subtableFormat = info.subtableFormat;
        font->cmap->length         = length;
        font->cmap->language       = language;

        if(info.subtableFormat == 0)
        {
            FF_ASSERT(length == 262, "length of cmap subtable format 0 has to be 262, "
                      "but is: %" PRIu16, length);

            readCmapFormat_0(info.subtableData, &info.subIndex);
        }
        else if(info.subtableFormat == 2)
        {
            readCmapFormat_2(info.subtableData, &info.subIndex);
        }
        else if(info.subtableFormat == 4)
        {
            readCmapFormat_4(font, &info);
        }
        else if(info.subtableFormat == 6)
        {
            readCmapFormat_6(info.subtableData, &info.subIndex);
        }
        else if(info.subtableFormat == 8)
        {
            FF_ASSERT(reserved == 0, "reserved needs to be 0 for cmap subtable format "
                      "8.");
            readCmapFormat_8(info.subtableData, &info.subIndex);
        }
        else if(info.subtableFormat == 10)
        {
            FF_ASSERT(reserved == 10, "reserved needs to be 0 for cmap subtable format "
                      "10.");
            readCmapFormat_10(info.subtableData, &info.subIndex);
        }
        else if(info.subtableFormat == 12)
        {
            FF_ASSERT(reserved == 0, "reserved needs to be 0 for cmap subtable format "
                      "12.");

            readCmapFormat_12(info.subtableData, &info.subIndex);
        }
        else if(info.subtableFormat == 13)
        {
            FF_ASSERT(reserved == 0, "reserved needs to be 0 for cmap subtable format "
                      "13.");
            readCmapFormat_13(info.subtableData, &info.subIndex);
        }
        else if(info.subtableFormat == 14)
        {
            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.",
                    info.subtableFormat);
            readCmapFormat_14(info.subtableData, &info.subIndex);
        }
        else
        {
            FF_ERROR("the cmap subtable format %"PRIu16" is unknown.",
                     info.subtableFormat);
            return;
        }
    }

    pdArrFree(infos);
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
    u08 *glyphData,
    u32 start,
    u32 end
){
    if(start == end)
    {
        return (glyf){0};
    }

    FF_ASSERT(start <= end, "start cannot be bigger than end: %"PRIu32" > %"PRIu32,
              start, end)

    glyf glyf = {0};

    glyf.numberOfContours  = (i16)(glyphData[start++] << 8);
    glyf.numberOfContours |= glyphData[start++];

    glyf.xMin = (i16)(glyphData[start++] << 8);
    glyf.xMin |= glyphData[start++];
    glyf.yMin = (i16)(glyphData[start++] << 8);
    glyf.yMin |= glyphData[start++];
    glyf.xMax = (i16)(glyphData[start++] << 8);
    glyf.xMax |= glyphData[start++];
    glyf.yMax = (i16)(glyphData[start++] << 8);
    glyf.yMax |= glyphData[start++];

    FF_ASSERT(start <= end, "current index is bigger than end, read too many bytes: %"
              PRIu32" > %"PRIu32, start, end)

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
    font.loca16 = &loca16;
    font.loca32 = &loca32;
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
    for(u16 i = 0; i < 26; ++i)
    {
        pdArrPush(alphabet, (char)('a' + i));
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

                FF_ASSERT(glyph < font.maxp->numGlyphs, "glyphIndex of %"PRIu16
                          " is outside the valid range (< %"PRIu16")",
                          glyph, font.maxp->numGlyphs);

                u32 offset      = 0;
                u32 offsetNext  = 0;

                if(font.loca16)
                {
                    offset  = (font.loca16[glyph] >> 8)
                    |   (u16)((font.loca16[glyph] & 0xFF) << 8);

                    offsetNext  = (font.loca16[glyph + 1] >> 8)
                    |       (u16)((font.loca16[glyph + 1] & 0xFF) << 8);

                    offset     *= 2;
                    offsetNext *= 2;
                }
                else // font.loca32
                {
                    offset  = ((font.loca32[glyph]) >> 24)
                    |    (u32)((font.loca32[glyph] & 0xFF0000) << 8)
                    |    (u32)((font.loca32[glyph] & 0x00FF00) << 16)
                    |    (u32)((font.loca32[glyph] & 0x0000FF) << 24);

                    offsetNext = ((font.loca32[glyph + 1]) >> 24)
                    |       (u32)((font.loca32[glyph + 1] & 0xFF0000) << 8)
                    |       (u32)((font.loca32[glyph + 1] & 0x00FF00) << 16)
                    |       (u32)((font.loca32[glyph + 1] & 0x0000FF) << 24);
                }

                FF_ASSERT(offsetNext >= offset, "parser error, offsetNext (%"PRIu32
                          ") cannot be smaller than offset (%"PRIu32").",
                          offsetNext, offset);

                glyf glyf = readGlyph(font.glyfData, offset, offsetNext);

                FF_DEBUG("glyphIndex of '%c' is %3"PRIu16". offset: %5"PRIu32
                         ", length: %4"PRIu32" contours: %2"PRIi16,
                         curr, glyph, offset, offsetNext - offset,
                         glyf.numberOfContours);

                if(glyf.numberOfContours == -1)
                {
                    FF_DEBUG("xMin: %3"PRIi16", yMin: %3"PRIi16", xMax: %3"PRIi16
                             ", yMax: %3"PRIi16,
                             glyf.xMin, glyf.yMin, glyf.xMax, glyf.yMax);
                }

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
