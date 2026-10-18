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

    OffsetTable ofsTable = {0};

    u08 byte = 0;
    for(u08 i = 0; i < 4; ++i)
    {
        if(fread(&byte, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read scalerType from offsetTable.");
            goto closefile;
        }

        ofsTable.scalerType |= (u32)(byte << (3 - i) * 8);
    }

    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&ofsTable.numTables, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read numTables from offsetTable.");
            goto closefile;
        }

        ofsTable.scalerType |= (u32)(byte << (1 - i) * 8);
    }

    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&ofsTable.searchRange, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read searchRange from offsetTable.");
            goto closefile;
        }

        ofsTable.searchRange |= (byte << (1 - i));
    }
    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&byte, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read entrySelector from offsetTable.");
            goto closefile;
        }

        ofsTable.entrySelector |= (byte << (1 - i) * 8);
    }
    for(u08 i = 0; i < 2; ++i)
    {
        if(fread(&byte, 1, 1, file) != 1)
        {
            FF_ERROR("failed to read rangeShift from offsetTable.");
            goto closefile;
        }

        ofsTable.rangeShift |= (byte << (1 - i) * 8);
    }

    FF_DEBUG("reading %"PRIu16" tables.", ofsTable.numTables);
    for(u16 i = 0; i < ofsTable.numTables; ++i)
    {
        char tagBuf[4] = {0};

        TableEntry table = {0};

        table.tag.data = tagBuf;
        table.tag.size = 4;

        if(fread(&tagBuf, 4, 1, file) != 1)
        {
            FF_ERROR("failed to read tag from table entry %"PRIu16".", i);
            goto closefile;
        }
        FF_DEBUG("read tag: '"PRI_SV"'", ARG_SV(table.tag));

        for(u08 i = 0; i < 4; ++i)
        {
            if(fread(&byte, 1, 1, file) != 1)
            {
                FF_ERROR("failed to read checksum from table entry %"PRIu16".", i);
                goto closefile;
            }

            table.checksum |= (u32)(byte << (3 - i) * 8);
        }
        FF_DEBUG("read table checksum: 0x%"PRIX32, table.checksum);

        for(u08 i = 0; i < 4; ++i)
        {
            if(fread(&byte, 1, 1, file) != 1)
            {
                FF_ERROR("failed to read offset from table entry %"PRIu16".", i);
                goto closefile;
            }

            table.offset |= (u32)(byte << (3 - i) * 8);
        }
        FF_DEBUG("read table offset: %"PRIu32, table.offset);

        for(u08 i = 0; i < 4; ++i)
        {
            if(fread(&byte, 1, 1, file) != 1)
            {
                FF_ERROR("failed to read length from table entry %"PRIu16".", i);
                goto closefile;
            }

            table.length |= (u32)(byte << (3 - i) * 8);
        }
        FF_DEBUG("read table length: %"PRIu32, table.length);

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
            // ignore its data for now
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
            goto closefile;
        }
    }

closefile:
    fclose(file);
}
