#include "fontflow.h"
#include "ff_print_macros.h"

#include "pd_string_view.h"

#include <inttypes.h>

#define FONT_PATH "fonts/notosansgothic/NotoSansGothic-Regular.ttf"

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
        if(platformSpecificID > 6)
        {
            FF_ERROR("platform ID is 0 (unicode) but platformSpecificID is %"PRIu16
                     ", which is not a valid identifier (0 - 6).", platformSpecificID);
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

    FF_TRACE("cmap version: %"PRIu16, cmapVersion);
    FF_ASSERT(!cmapVersion, "cmap version must be set to 0.");

    FF_TRACE("cmap subtables: %"PRIu16, cmapSubtables);

    for(u16 i = 0; i < cmapSubtables; ++i)
    {
        FF_TRACE("\ncmap subtable index: %"PRIu16, i);

        u16 platformID  = (u16)(cmapData[index++] << 8);
        platformID     |= cmapData[index++];
        FF_TRACE("platformID:          %"PRIu16, platformID);

        u16 platformSpecificID  = (u16)(cmapData[index++] << 8);
        platformSpecificID     |= cmapData[index++];
        FF_TRACE("platformSpecificID:  %"PRIu16, platformSpecificID);

        u32 mappingOffset = 0;
        for(u08 i = 0; i < 4; ++i)
        {
            mappingOffset |= (u32)cmapData[index++] << (3 - i) * 8;
        }
        FF_TRACE("mappingOffset:       %"PRIu32, mappingOffset);

        if(!verifyIDs(platformID, platformSpecificID))
        {
            return;
        }

        u64 subIndex        = 0;
        u08 *subtableData   = cmapData + mappingOffset;
        u16 subtableFormat  = (u16)(subtableData[subIndex++] << 8);
        subtableFormat     |= subtableData[subIndex++];
        FF_TRACE("subtableFormat:      %"PRIu16, subtableFormat);

        u32 length   = 0;
        u32 language = 0;
        u16 reserved = 0;

        bool needsLenLanguage = subtableFormat == 0  || subtableFormat == 2 ||
                                subtableFormat == 4  || subtableFormat == 6;

        bool needsReserved    = subtableFormat == 8  || subtableFormat == 10 ||
                                subtableFormat == 12 || subtableFormat == 12;

        bool needsLen32       = needsReserved || subtableFormat == 14;

        if(needsLenLanguage)
        {
            length  = (u16)(subtableData[subIndex++] << 8);
            length |= subtableData[subIndex++];
            FF_TRACE("length:              %"PRIu16, length);

            language  = (u16)(subtableData[subIndex++] << 8);
            language |= subtableData[subIndex++];
            FF_TRACE("language:            %"PRIu16, language);

            if(language && platformID != FF_PLATFORM_ID_MACINTOSH)
            {
                FF_ERROR("language code %"PRIu16" undefined for platform ID %"PRIu16".",
                         language, platformID);
            }
        }
        else if(needsReserved)
        {
            reserved  = (u16)(subtableData[subIndex++] << 8);
            reserved |= subtableData[subIndex++];
            FF_TRACE("reserved:            %"PRIu16, reserved);
        }

        if(needsLen32)
        {
            length = 0;
            for(u08 i = 0; i < 4; ++i)
            {
                length |= (u32)subtableData[subIndex++] << (3 - i) * 8;
            }
            FF_TRACE("length:              %"PRIu32, length);
        }
        if(needsReserved)
        {
            language = 0;
            for(u08 i = 0; i < 4; ++i)
            {
                language |= (u32)subtableData[subIndex++] << (3 - i) * 8;
            }
            FF_TRACE("language:            %"PRIu32, language);
        }

        if(subtableFormat == 0)
        {
            FF_ASSERT(length == 262, "length of cmap subtable format 0 has to be 262, "
                      "but is: %"PRIu16, length);

            u08 *glyphIndexArray = subtableData + subIndex;

            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 2)
        {
            // u16 subHeaderKeys[256]
            // u16 * 4 subHeaders[variable]

            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 4)
        {
            u16 segCountX2  = (u16)(subtableData[subIndex++] << 8);
            segCountX2     |= subtableData[subIndex++];
            u16 segCount = segCountX2 / 2;
            FF_TRACE("segCount:            %"PRIu16, language);

            u16 searchRange  = (u16)(subtableData[subIndex++] << 8);
            searchRange     |= subtableData[subIndex++];
            FF_TRACE("searchRange:         %"PRIu16, language);

            // u16 entrySelector
            // u16 rangeShift
            // u16 endCode[segCount]
            // u16 reservedPad
            // u16 startCode[segCount]
            // u16 idDelta[segCount]
            // u16 idRangeOffset[segCount]
            // u16 glyphIndexArray[Variable]
            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 6)
        {
            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 8)
        {
            FF_ASSERT(reserved == 0, "reserved needs to be 0 for cmap subtable format "
                      "8.");
            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 10)
        {
            FF_ASSERT(reserved == 10, "reserved needs to be 0 for cmap subtable format "
                      "10.");
            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 12)
        {
            FF_ASSERT(reserved == 0, "reserved needs to be 0 for cmap subtable format "
                      "12.");

            u32 nGroups = 0;
            for(u08 i = 0; i < 4; ++i)
            {
                nGroups |= (u32)subtableData[subIndex++] << (3 - i) * 8;
            }
            FF_TRACE("nGroups:             %"PRIu32, nGroups);

            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 13)
        {
            FF_ASSERT(reserved == 0, "reserved needs to be 0 for cmap subtable format "
                      "13.");
            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else if(subtableFormat == 14)
        {
            FF_WARN("cmap subtableFormat of %"PRIu16" unimplemented.", subtableFormat);
        }
        else
        {
            FF_ERROR("the cmap subtable format %"PRIu16" is unknown.", subtableFormat);
            return;
        }
    }
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

    FontData font = {0};
    font.data = fileBuf;

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

        for(u08 i = 0; i < 4; ++i)
        {
            tagBuf[i] = (char)fileBuf[index++];
        }
        FF_DEBUG("read tag: '"PRI_SV"'", ARG_SV(table.tag));

        for(u08 i = 0; i < 4; ++i)
        {
            table.checksum |= (u32)(fileBuf[index++]) << (3 - i) * 8;
        }
        FF_TRACE("read table checksum: 0x%"PRIX32, table.checksum);

        for(u08 i = 0; i < 4; ++i)
        {
            table.offset |= (u32)(fileBuf[index++]) << (3 - i) * 8;
        }
        FF_TRACE("read table offset: %"PRIu32, table.offset);

        for(u08 i = 0; i < 4; ++i)
        {
            table.length |= (u32)(fileBuf[index++]) << (3 - i) * 8;
        }
        FF_TRACE("read table length: %"PRIu32, table.length);

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
            // ignore its data for now
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
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, hheaIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, hmtxIdent))
        {
            // ignore its data for now
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
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, ltagIdent))
        {
            // ignore its data for now
        }
        else if(pdSVSame(table.tag, maxpIdent))
        {
            // ignore its data for now
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
            // ignore its data for now
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
            // ignore its data for now
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

error:
    free(fileBuf);
}
