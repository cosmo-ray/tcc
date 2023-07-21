#include <stdlib.h>
#include <string.h>
#include "tcc.h"

/*
 * Sections in wasm:
 * 0: magic <- in code fwrite
 * 1: types <- rodata_section
 * 2: ????
 * 3: functions (for declarations only) <- functions_sections
 * 4: tables <- symtab_section
 * 5: memory <- data_section
 * 6: Globals <- global_section
 * 7: export <- export_section
 * 8: ???
 * 9: ????
 * 10: code <- text_section
 */

static int write_section(char nb, FILE *fp, Section *s)
{
	char *len_bytes = (char *)&s->sh_size;
	int len_len = 0;

	if (len_bytes[3])
		len_len = 4;
	else if (len_bytes[2])
		len_len = 3;
	else if (len_bytes[1])
		len_len = 2;
	else
		len_len = 1;

	/* fixup latter: skip section with 0 bytes */

	if (fwrite(&nb, 1, 1, fp) < 0)
		return -1;
	if (fwrite(len_bytes, 1, len_len, fp) < 0)
		return -1;
	return fwrite(s->data, 1, s->sh_size, fp);
}

#define TYPE_SECTION_NB 0x01
#define FUNCTION_SECTION_NB 0x03
#define TABLE_SECTION_NB 0x04
#define MEMORY_SECTION_NB 0x05
#define GLOBAL_SECTION_NB 0x06
#define EXPORT_SECTION_NB 0x07
#define CODE_SECTION_NB 0x0A

int wasm_output_file(TCCState *s1, const char *filename)
{
	int mode;
	int file_type = s1->output_type;
	int fd;
	FILE *fp;
	char magic[8] = {0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00};

	if (file_type == TCC_OUTPUT_OBJ)
		mode = 0666;
	else {
		mode = 0777;
	}
	printf("wasm_output_file: %s\n", filename);
	unlink(filename);
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, mode);
	if (fd < 0 || (fp = fdopen(fd, "wb")) == NULL) {
		tcc_error_noabort("could not write '%s: %s'", filename, strerror(errno));
		return -1;
	}
	printf("type_section: %p\n"
	       "funtions_section %p\n"
	       "tables_section %p\n"
	       "memory_section %p\n"
	       "global_section %p\n"
	       "export_section %p\n"
	       "code_section   %p\n",
	       type_section, function_section, table_section, memory_section, global_section,
	       export_section, code_section);
	if (fwrite(magic, sizeof magic, 1, fp) < 0)
		return -1;
	write_section(TYPE_SECTION_NB, fp, type_section);
	write_section(FUNCTION_SECTION_NB, fp, function_section);
	write_section(TABLE_SECTION_NB, fp, table_section);
	write_section(MEMORY_SECTION_NB, fp, memory_section);
	write_section(GLOBAL_SECTION_NB, fp, global_section);
	write_section(EXPORT_SECTION_NB, fp, export_section);
	write_section(CODE_SECTION_NB, fp, code_section);
	return 0;
}
