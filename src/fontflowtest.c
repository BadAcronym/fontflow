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
        char tagBuf[4];

        StringView tag = {0};
        tag.data = tagBuf;
        tag.size = 4;

        if(fread(&tagBuf, 4, 1, file) != 1)
        {
            FF_ERROR("failed to read tag %"PRIu16" from offsetTable.", i);
            goto closefile;
        }

        FF_DEBUG("read tag: '"PRI_SV"'", ARG_SV(tag));

        if(pdSVSame(tag, GDEFIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, GPOSIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, GSUBIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, acntIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, ankrIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, avarIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, bdatIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, bhedIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, blocIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, bslnIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, cmapIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, cvarIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, cvtIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, EBSCIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, fdscIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, featIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, fmtxIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, fondIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, fpgmIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, fvarIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, gaspIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, gcidIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, glyfIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, gvarIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, hdmxIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, headIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, hheaIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, hmtxIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, hvglIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, hvpmIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, justIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, kernIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, kerxIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, lcarIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, locaIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, ltagIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, maxpIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, metaIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, mortIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, morxIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, nameIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, opbdIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, OS2Ident))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, postIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, prepIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, propIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, sbixIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, trakIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, vheaIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, vmtxIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, xrefIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else if(pdSVSame(tag, ZapfIdent))
        {
            // ignore its data for now
            fseek(file, 12, SEEK_CUR);
        }
        else
        {
            FF_ERROR("unknown tag: '"PRI_SV"'", ARG_SV(tag));
            goto closefile;
        }
    }

closefile:
    fclose(file);
}
