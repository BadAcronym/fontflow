#include "fontflow.h"
#include "ff_print_macros.h"

#include "pd_util.h"
#include "pd_dyn_arr.h"

#define FONT_PATH "fonts/dejavu/DejaVuSans.ttf"

int main
(
    void
){
    BinBuffer file = pdReadBinFile(FONT_PATH);

    FontData font = {0};
    font.data = file.data;

    ffReadFont(&font);

    u16 *alphabet = 0;
    for(u16 i = 0; i < 26; ++i)
    {
        pdArrPush(alphabet, (u16)('a' + i));
        pdArrPush(alphabet, (u16)('A' + i));
    }

    glyf *glyphs = ffFindGlyphs(&font, alphabet);

    for(u16 i = 0; i < pdArrSize(glyphs); ++i)
    {
        pdArrFree(glyphs[i].flags);
        pdArrFree(glyphs[i].xCoordinates);
        pdArrFree(glyphs[i].yCoordinates);
    }
    pdArrFree(glyphs);
    pdArrFree(alphabet);
    free(file.data);
}
