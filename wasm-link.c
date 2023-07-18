#ifdef TARGET_DEFS_ONLY
/* I don't handle elf, not exe format for now, so let's reuse riscv */
#define EM_TCC_TARGET EM_RISCV

#define R_DATA_32  R_RISCV_32
#define R_DATA_PTR R_RISCV_64
#define R_JMP_SLOT R_RISCV_JUMP_SLOT
#define R_GLOB_DAT R_RISCV_64
#define R_COPY     R_RISCV_COPY
#define R_RELATIVE R_RISCV_RELATIVE

#define R_NUM      R_RISCV_NUM

#define ELF_START_ADDR 0x00010000
#define ELF_PAGE_SIZE 0x1000

#define PCRELATIVE_DLLPLT 1
#define RELOCATE_DLLPLT 1

#else

#include "tcc.h"

ST_FUNC unsigned create_plt_entry(TCCState *s1, unsigned got_offset, struct sym_attr *attr)
{
	printf("create_plt_entry(s1, %u, attr)\n", got_offset);
	return 0;
}

ST_FUNC void relocate_plt(TCCState *s1)
{
	printf("relocate_plt(s1)\n");
}

void relocate(TCCState *s1, ElfW_Rel *rel, int type, unsigned char *ptr,
              addr_t addr, addr_t val)
{
	printf("realocate(s1, rel, %d, %s, %ld, %ld)\n",
	       type, ptr, addr, val);
}

/* Returns an enumerator to describe whether and when the relocation needs a
   GOT and/or PLT entry to be created. See tcc.h for a description of the
   different values. */
int gotplt_entry_type (int reloc_type)
{
	printf("gotplt_entry_type(%d)\n", reloc_type);
}

/* Returns 1 for a code relocation, 0 for a data relocation. For unknown
   relocations, returns -1. */
int code_reloc (int reloc_type)
{
	printf("code_reloc(%d)\n", reloc_type);
}

#endif /* ndef TARGET_DEFS_ONLY */
