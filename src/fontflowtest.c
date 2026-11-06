#include "fontflow.h"
#include "ff_print_macros.h"

#include "pd_dyn_arr.h"

#include <inttypes.h>

#define FONT_PATH "fonts/dejavu/DejaVuSans.ttf"

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
        return 1;
    }

    cmap cmap   = {0};
    head head   = {0};
    u16  loca16 = {0};
    u32  loca32 = {0};
    maxp maxp   = {0};

    FontData font = {0};
    font.data = fileBuf;
    font.cmap = &cmap;
    font.head = &head;
    font.maxp = &maxp;

    if(fread(fileBuf, 1, filesize, file) != filesize)
    {
        FF_ERROR("failed to read entire file into fileBuf.");
        fclose(file);
        goto error;
    }
    fclose(file);

    ffReadFont(&font, fileBuf);

    u16 *alphabet = 0;
    for(u16 i = 0; i < 26; ++i)
    {
        pdArrPush(alphabet, (u16)('A' + i));
    }

    ffFindGlyphs(&font, alphabet);

error:
    free(fileBuf);
}
