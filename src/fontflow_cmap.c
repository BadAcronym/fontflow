#include "fontflow.h"
#include "ff_print_macros.h"

#include "pd_dyn_arr.h"

#include <inttypes.h>

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
        FF_DEBUG("unimplemented: verify Macintosh QuickDraw script code?");
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

f_internal void readCmapFormat_4
(
    FontData         *font,
    CmapSubtableInfo *info
){
    u08 *subtableData = info->subtableData;
    u64 *subIndex     = &info->subIndex;

    FF_TRACE("\nreading cmap format 4 @ %p.", subtableData);

    font->cmap.segCountX2  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap.segCountX2 |= subtableData[(*subIndex)++];
    u16 segCount = font->cmap.segCountX2 / 2;
    FF_TRACE("segCount:              %"PRIu16, segCount);

    font->cmap.searchRange  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap.searchRange |= subtableData[(*subIndex)++];
    FF_TRACE("searchRange:           %"PRIu16, font->cmap.searchRange);

    // assert that searchRange is 2 * (2**floor(log2(segCount)))

    font->cmap.entrySelector  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap.entrySelector |= subtableData[(*subIndex)++];
    FF_TRACE("entrySelector:         %"PRIu16, font->cmap.entrySelector);

    // assert that entrySelector is log2(searchRange/2)

    font->cmap.rangeShift  = (u16)(subtableData[(*subIndex)++] << 8);
    font->cmap.rangeShift |= subtableData[(*subIndex)++];
    FF_TRACE("rangeShift:            %"PRIu16, font->cmap.rangeShift);

    FF_ASSERT(font->cmap.rangeShift == font->cmap.segCountX2 -
              font->cmap.searchRange,
              "rangeshift was expected to be %"PRIu16", instead got: %"PRIu16,
              font->cmap.segCountX2 - font->cmap.searchRange,
              font->cmap.rangeShift);

    font->cmap.endCode = (u16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    u16 reservedPad  = (u16)(subtableData[(*subIndex)++] << 8);
    reservedPad     |= subtableData[(*subIndex)++];
    FF_TRACE("reservedPad:           %"PRIu16, reservedPad);

    FF_ASSERT(!reservedPad, "reservedPad in cmap subtable format 4 needs to be 0.");

    font->cmap.startCode = (u16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    font->cmap.idDelta = (i16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    font->cmap.idRangeOffset = (u16*)(&subtableData[(*subIndex)]);
    (*subIndex) += segCount * 2;

    font->cmap.glyphIdArray16 = (u16*)(&subtableData[(*subIndex)]);
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

void readTable_cmap
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

        font->cmap.subtableFormat = info.subtableFormat;
        font->cmap.length         = length;
        font->cmap.language       = language;

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
