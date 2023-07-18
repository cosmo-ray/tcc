#ifdef TARGET_DEFS_ONLY
#else

#include "tcc.h"

ST_FUNC void g(int c)
{
    printf("wasm g(%d)\n", c);
}

ST_FUNC void gen_le16 (int i)
{
    printf("wasm gen_le16(%d)\n", i);
}

ST_FUNC void gen_le32 (int i)
{
    printf("wasm gen_le32(%d)\n", i);
}

#endif /* ndef TARGET_DEFS_ONLY */
