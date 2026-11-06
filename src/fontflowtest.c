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
        pdArrPush(alphabet, (u16)('A' + i));
    }

    ffFindGlyphs(&font, alphabet);

    free(file.data);
}
