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

static int write_section(char section, FILE *fp, Section *s, unsigned char nb_stuff)
{
	int len_cp = s->sh_size;
	char len_bytes[5] = {0};
	char *len_bytes_ptr = len_bytes;
	int len_len = 1;

	/* assuming less than 255 functions per file */
	if (!s->sh_size)
		return 0;

	if (nb_stuff)
		len_cp += 1;

again:
	*len_bytes_ptr = len_cp & 0x7f;
	len_cp = (len_cp & 0xffffff80) >> 7;
	if (len_cp > 0x7f) {
		*len_bytes_ptr |= 0x80;
		++len_bytes_ptr;
		++len_len;
		goto again;
	}

	if (fwrite(&section, 1, 1, fp) < 0)
		return -1;
	if (fwrite(len_bytes, 1, len_len, fp) < 0)
		return -1;
	if (nb_stuff) {
		if (fwrite(&nb_stuff, 1, 1, fp) < 0)
			return -1;
	}
	return fwrite(s->data, 1, s->sh_size, fp);
}

#define TYPE_SECTION_NB 0x01
#define FUNCTION_SECTION_NB 0x03
#define TABLE_SECTION_NB 0x04
#define MEMORY_SECTION_NB 0x05
#define GLOBAL_SECTION_NB 0x06
#define EXPORT_SECTION_NB 0x07
#define CODE_SECTION_NB 0x0A

#define TRY(that) do { if ((that) < 0) return -1;} while (0)

int wasm_output_file(TCCState *s1, const char *filename)
{
	int mode;
	int file_type = s1->output_type;
	int fd;
	FILE *fp;
	char magic[8] = {0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00};
	int filename_l = strlen(filename);
	char *wasm_file = NULL;
	int ret = -1;

	if (file_type == TCC_OUTPUT_OBJ)
		mode = 0666;
	else {
		mode = 0777;
	}
	//  p *tcc_state->rodata_section
	if (filename_l > 3 && !strcmp(&filename[filename_l - 3], ".js")) {
		char js_p0[] = "const fs = require('fs');\n"
			"const buf = fs.readFileSync('./";
		char js_p1[] =
			"');\n"
			"const lib = WebAssembly.instantiate(new Uint8Array(buf)).\n"
			"then(res => {\n"
			"\tfor (var i=1;i<=10;i++) {\n"
			"\tlet r = res.instance.exports.";
		char js_p2[] ="(i, i+1)\n"
			"console.log(\"aB result: \"+i+\" = \"+r)\n"
			"\t}\n"
			"});\n";
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, mode);
		if (fd < 0 || (fp = fdopen(fd, "wb")) == NULL) {
			tcc_error_noabort("could not write '%s: %s'", filename,
					  strerror(errno));
			return -1;
		}
		wasm_file = tcc_malloc(filename_l + 2);
		strncpy(wasm_file, filename, filename_l - 3);
		strcpy(wasm_file + filename_l - 3, ".wasm");
		fwrite(js_p0, sizeof js_p0 -1, 1, fp);
		fwrite(wasm_file, filename_l + 2, 1, fp);
		fwrite(js_p1, sizeof js_p1 -1, 1, fp);
		fwrite("aB", 2, 1, fp);
		fwrite(js_p2, sizeof js_p2 -1, 1, fp);
	} else {
		wasm_file = filename;
	}
	unlink(wasm_file);
	fd = open(wasm_file, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, mode);
	if (fd < 0 || (fp = fdopen(fd, "wb")) == NULL) {
		tcc_error_noabort("could not write '%s: %s'", wasm_file, strerror(errno));
		goto out;
	}
	/* printf("type_section: %p\n" */
	/*        "funtions_section %p\n" */
	/*        "tables_section %p\n" */
	/*        "memory_section %p\n" */
	/*        "global_section %p\n" */
	/*        "export_section %p\n" */
	/*        "code_section   %p\n", */
	/*        type_section, function_section, table_section, memory_section, global_section, */
	/*        export_section, code_section); */
	/* printf("code_section size: %ld %ld\n", code_section->data_offset, text_section->data_offset); */
	code_section->sh_size = code_section->data_offset;
	type_section->sh_size = type_ind;
	export_section->sh_size = export_ind;
	function_section->sh_size = wasm_func_ind;
	global_section->sh_size = glob_ind;
	memory_section->sh_size = mem_ind;
	TRY(fwrite(magic, sizeof magic, 1, fp) < 0);
	printf("wasm_type_cnt: %d - %p\n", wasm_type_cnt, type_section);
	for (int i = 0; i < type_section->sh_size; ++i) {
		printf("%x", type_section->data[i]);
	}
	printf("\n");
	TRY(write_section(TYPE_SECTION_NB, fp, type_section, wasm_type_cnt));
	/* assuming function section contain exactly 1 byte per function */
	TRY(write_section(FUNCTION_SECTION_NB, fp, function_section, function_section->sh_size));
	TRY(write_section(TABLE_SECTION_NB, fp, table_section, 0));
	/* I guess mem should be output only in binaries */
	/* printf("mem len: %ld - %ld\n", memory_section->sh_size, mem_ind); */
	/* for now it seems there is always 1 mem */
	TRY(write_section(MEMORY_SECTION_NB, fp, memory_section, 1));
	/* 2 globals: 1 for stack function begin, and 1 for stack end */
	TRY(write_section(GLOBAL_SECTION_NB, fp, global_section, 2));
	TRY(write_section(EXPORT_SECTION_NB, fp, export_section, nb_export));
	TRY(write_section(CODE_SECTION_NB, fp, code_section, nb_func));
	type_ind = 0;
	nb_func = 0;
	wasm_func_ind = 0;
	mem_ind = 0;
	ret = 1;
out:
	if (wasm_file != filename)
		tcc_free(wasm_file);
	return ret;
}
