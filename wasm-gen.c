#ifdef TARGET_DEFS_ONLY

#define LDOUBLE_SIZE 16
#define LDOUBLE_ALIGN 16

#define NB_REGS 19

#define PTR_SIZE 8

#define TREG_R(x) (x) // x = 0..7
#define TREG_F(x) (x + 8) // x = 0..7

#define RC_INT (1 << 0)
#define RC_FLOAT (1 << 1)
#define RC_R(x) (1 << (2 + (x))) // x = 0..7
#define RC_F(x) (1 << (10 + (x))) // x = 0..7

#define RC_IRET (RC_R(0)) // int return register class
#define RC_IRE2 (RC_R(1)) // int 2nd return register class
#define RC_FRET (RC_F(0)) // float return register class

#define TREG_RA 17
#define TREG_SP 18

#define REG_IRET (TREG_R(0)) // int return register number
#define REG_IRE2 (TREG_R(1)) // int 2nd return register number
#define REG_FRET (TREG_F(0)) // float return register number

#define MAX_ALIGN 16

#else

enum wasm_instructions {
	UNREACHABLE = 0x00,
	NOP = 0x01,
	BLOCK = 0x02,
	LOOP = 0x03,
	END = 0x0b,
	BR = 0x0c,
	BR_IF = 0x0d,
	BR_TABLE = 0x0e,
	LOCAL_GET = 0x20,
	LOCAL_SET = 0x21,
	I32_LOAD = 0x28,
	I64_LOAD = 0x29,
	F32_LOAD = 0x2a,
	F64_LOAD = 0x2b,
	I32_STORE = 0x36,
	I64_STORE = 0x37,
	F32_STORE = 0x38,
	F64_STORE = 0x39,
	VOID = 0x40,
	I32_CONST = 0x41,
	I32_EQZ = 0x45,
	I32_EQ = 0x46,
	I32_NE = 0x47,
	I32_LT_S = 0x48,
	I32_GT_S = 0x4a,
	I32_LE_S = 0x4c,
	I32_LE_U = 0x4d,
	I32_GE_S = 0x4e,
	I32_ADD = 0x6a,
	I32_SUB = 0x6b,
	I32_MUL = 0x6c,
	I32_DIV_S = 0x6d // DIV_S is for signed integer
};

enum wasm_type_instruction {
	WASM_FLOAT_64 = 0x7c,
	WASM_FLOAT_32 = 0x7d,
	WASM_INT_64 = 0x7e,
	WASM_INT_32 = 0x7f
};

#define USING_GLOBALS
#include "tcc.h"

static int block_cnt;

ST_DATA const char * const target_machine_defs =
    "__wasm__\0"
    "__wasm\0"
    ;

ST_DATA const int reg_classes[NB_REGS] = {
    RC_INT | RC_R(0),
    RC_INT | RC_R(1),
    RC_INT | RC_R(2),
    RC_INT | RC_R(3),
    RC_INT | RC_R(4),
    RC_INT | RC_R(5),
    RC_INT | RC_R(6),
    RC_INT | RC_R(7),
    RC_FLOAT | RC_F(0),
    RC_FLOAT | RC_F(1),
    RC_FLOAT | RC_F(2),
    RC_FLOAT | RC_F(3),
    RC_FLOAT | RC_F(4),
    RC_FLOAT | RC_F(5),
    RC_FLOAT | RC_F(6),
    RC_FLOAT | RC_F(7),
    0,
    1 << TREG_RA,
    1 << TREG_SP
};

#define WASM_MAX_PARAMS 256
#define WASM_MAX_TYPES 2048

struct wasm_type_info {
    char type;
    char ret;
    short int nb_params;
    short int stack_len;
    short int nb_i32;
    short int nb_i64;
    short int nb_f32;
    short int nb_f64;
    short int cmp_i32_loc;
    int block_cnt;
};

struct wasm_type_info *cur_function;

struct wasm_type {
    struct wasm_type_info ti;
    struct wasm_type_info params[WASM_MAX_PARAMS];
};

static struct wasm_type all_types[WASM_MAX_TYPES];


#if defined(CONFIG_TCC_BCHECK)
static addr_t func_bound_offset;
static unsigned long func_bound_ind;
ST_DATA int func_bound_add_epilog;
#endif

int func_size_ind;

static void g_func(char c)
{
    int ind1;
    if (nocode_wanted)
        return;
    ind1 = wasm_func_ind + 1;
    if (ind1 > function_section->data_allocated)
        section_realloc(function_section, ind1);
    function_section->data[wasm_func_ind] = c;
    wasm_func_ind = ind1;
}

static void g_type(unsigned char c)
{
    int ind1;
    if (nocode_wanted)
        return;
    ind1 = type_ind + 1;
    if (ind1 > type_section->data_allocated)
        section_realloc(type_section, ind1);
    type_section->data[type_ind] = c;
    type_ind = ind1;
}

static void g_code(char c)
{
    int ind1;
    if (nocode_wanted)
        return;
    ind1 = ind + 1;
    if (ind1 > code_section->data_allocated)
        section_realloc(code_section, ind1);
    code_section->data[ind] = c;
    ind = ind1;
}

static void g_export(char c)
{
    int ind1;
    if (nocode_wanted)
        return;
    ind1 = export_ind + 1;
    if (ind1 > export_section->data_allocated)
        section_realloc(export_section, ind1);
    export_section->data[export_ind] = c;
    export_ind = ind1;
}

/*
 * a number literal in the code, take only the place it require
 * so even if it's a VT_INT, it still might need to be shrink
 */
static void g_code_int(int i)
{
    char cur;

  again:
    cur = i & 0x7f;
    i = (i & 0xffffff80) >> 7;
    if (i) {
	cur |= 0x80;
	g_code(cur);
	goto again;
    }
    g_code(cur);
}

static void g_mem(char byte)
{
    int ind1;
    if (nocode_wanted)
        return;
    ind1 = mem_ind + 1;
    if (ind1 > mem_ind)
        section_realloc(memory_section, ind1);
    memory_section->data[mem_ind] = byte;
    mem_ind = ind1;
}

static void g_mem_int(int i)
{
    char cur;

  again:
    cur = i & 0x7f;
    i = (i & 0xffffff80) >> 7;
    if (i) {
	cur |= 0x80;
	g_mem(cur);
	goto again;
    }
    g_mem(cur);
}

ST_FUNC void o(unsigned int c)
{
    printf("o(%d)\n", c);
}

ST_FUNC void gsym_addr(int t_, int a_)
{
    g_code(END);
    printf("gsym_addr(%d %d)\n", t_, a_);
}

void print_r_mask(uint32_t r, CType t)
{
    uint32_t or = r & VT_VALMASK;
    printf("VT_LVAL: %d ", !!(r & VT_LVAL));
    printf("VT_CONST: %d ", or == VT_CONST);
    printf("VT_LLOCAL: %d\n", or == VT_LLOCAL);
    printf("VT_LOCAL: %d ", or == VT_LOCAL);
    printf("VT_CMP: %d ", or == VT_CMP);
    printf("VT_JMP: %d ", or == VT_JMP);
    printf("VT_JMPI: %d ", or == VT_JMPI);
    printf("VT_SYM: %d\n", or == VT_SYM);
    printf("VT_INT: %d ", ((t.t & VT_BTYPE) == VT_INT));
    /* printf("VT_PTR: %d ", ((t.t & VT_BTYPE) == VT_PTR)); */
    /* printf("VT_UNSIGNED: %d\n", !!(t.t & VT_UNSIGNED)); */
    printf("(r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST: %d\n",
	    (r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST);
    printf("\n");
}

/* store something into wasm stack */
/* to store a variable from a local to was stack, use local.get */
ST_FUNC void load(int r, SValue *sv)
{
    uint32_t or = sv->r & VT_VALMASK;
    CType t = sv->type;

    printf("==== load(%d, sv, lval %d)===== ", r);
    /* printf("sv: %ld ", sv->c.i); */
    /* printf("r: %x\n", r); */
    /* printf("SV->R:"); */
    //print_r_mask(sv->r, t);
    /* if (vtop[-1].sym) { */
    /* 	    printf("(%p - %d - %s)\n", sv->sym, sv->sym ? sv->sym->c : -1, get_tok_str(sv->sym->v, NULL)); */
    /* } else { */
    /* 	    printf("(ny sym)\n"); */
    /* } */
    if (or == VT_CONST) {
	int local_idx = cur_function->nb_params + (-1 * (sv->c.i / 4)) - 1;
	/* printf("cur func: %d\n", cur_function->nb_params); */

	/* load const into mem */
	/* sv->c.i value if VT_INT */
	if (((t.t & VT_BTYPE) == VT_INT)) {
	    g_code(I32_CONST);
	    g_code_int(sv->c.i);
	    /* printf("need to store at %d\n", local_idx); */
	}
    } else if (or == VT_LOCAL) {
	    printf("%d ", sv->c.i);
	    g_code(I32_CONST);
	    if ((int64_t)sv->c.i < 0) {
		int idx = cur_function->nb_params + (-1 * (sv->c.i / 4)) - 1;
		/* if sv->c.i < 0, then is on stack, and pos in byte
		 * no idea how i'm gona mix with variables of diferent bytes
		 * so this need to be convert to a wasm local pos */
		g_code_int(idx * 4);
	    } else {
		g_code_int(sv->c.i * 4); // local index
	    }
	    g_code(I32_LOAD);
	    g_code_int(2); /* alignement */
	    g_code_int(0);  /* offset */
	    /* g_code(LOCAL_GET); // local set */
    }
    printf("\n");
}

static char *byte_to_str[] = {
    "VT_VOID",
    "VT_BYTE",
    "VT_SHORT",
    "VT_INT",
    "VT_LLONG",
    "VT_PTR",
    "VT_FUNC",
    "VT_STRUCT",
    "VT_FLOAT",
    "VT_DOUBLE",
    "VT_LDOUBLE",
    "VT_BOOL",
    "VT_QLONG",
    "VT_QFLOAT"
};

/* store thing from wasm stack into wasm local  */
ST_FUNC void store(int r, SValue *sv)
{
    uint32_t or = sv->r & VT_VALMASK;
    CType t = sv->type;

    printf("---- store(%d. sv) ----\n", r);
    /* printf("sv: %ld ", sv->c.i); */
    /* printf("SV->R: "); */
    /* //print_r_mask(sv->r, sv->type); */
    /* if (vtop[-1].sym) { */
    /* 	    printf("(%p - %d - %s)\n", sv->sym, sv->sym ? sv->sym->c : -1, get_tok_str(sv->sym->v, NULL)); */
    /* } else { */
    /* 	    printf("(ny sym)\n"); */
    /* } */
    if (or == VT_LOCAL) {
	/* load stack into local */
	/* sv->c.i value if VT_INT */
	/* if there is 2  param, then param at index 2, is the first non param argument*/
	int local_idx = cur_function->nb_params + (-1 * (sv->c.i / 4)) - 1;
	/* printf("nb param: %d\n", cur_function->nb_params); */
	/* printf("sv->c.i: %ld\n", sv->c.i); */
	/* printf("store wasm stack index: %ld\n", */
	/*        cur_function->nb_params + 1 + (-1 * (sv->c.i / 4))); */
	if (((t.t & VT_BTYPE) == VT_INT)) {
	    /* printf("store int !"); */
	    /* g_code_int(&sv->c.i); */
	    g_code(LOCAL_SET);
	    g_code_int(local_idx); // local index

	    g_code(I32_CONST); // local set
	    g_code_int(local_idx * 4); // local index

	    g_code(LOCAL_GET);
	    g_code_int(local_idx); // local index

	    g_code(I32_STORE);
	    cur_function->stack_len++;
	    cur_function->nb_i32++;
	} else {
	    printf("TODO: %s", byte_to_str[t.t & VT_BTYPE]);
	}
    } else if (or == VT_CONST) {
	printf("or == VT_CONST\n");
    } else if (sv->r & VT_LVAL) {
	printf("sv->r & VT_LVAL\n");
    } else if (or != r) {
	printf("or != r !!!\n");
    } else {
	printf("gre nieu nieu\n");
    }

    printf("\n");
}

static char find_type(struct wasm_type *type)
{
    int i = 0;

    for (i = 0; i < wasm_type_cnt; ++i) {
	if (!memcmp(&all_types[i], type, sizeof *type))
	    return i;
    }
    return -1;
}

ST_FUNC void gfunc_call(int nb_args)
{
    int i;

    printf("gfunc_call(%d)\n", nb_args);
    for(i = 0; i < nb_args; i++) {
	vtop--;
    }
    vtop--;
}

int memory_limit = 0x7000;

void init_mem(void)
{
    int tmp = 0x01;

    g_mem(tmp); /* flags (same as emcc, don't know what it is) */
    /* steal thoses V value from emcc */
    g_mem_int(memory_limit); /* initial limit */
    g_mem_int(memory_limit); /* max limit */
}

ST_FUNC void gfunc_prolog(Sym *func_sym)
{
    Sym *sym, *osym;
    int func_call;
    CType *func_type = &func_sym->type;
    int nb_args = 0;
    struct wasm_type type = {0};
    int type_idx;
    int i;
    char *tmp;


    char *to_export = get_tok_str(func_sym->v, 0);

    g_export(strlen(to_export));
    for (; *to_export; ++to_export)
	    g_export(*to_export);
    g_export(0);
    ++nb_export;
    g_export(nb_func);
// push get_tok_str(func_sym->v, 0) in export func
    // write_section()


    if (!mem_ind) {
	init_mem();
    }
    type.ti.type = 0x60;

    sym = func_type->ref;
    func_call = sym->f.func_call;
    loc = 0;
    func_vc = 0;
    osym = sym;
    while ((sym = sym->next) != NULL) {
	CType *t;
	int bt;

	t = &sym->type;
	bt = t->t & VT_BTYPE;

	sym_push(sym->v & ~SYM_FIELD, &sym->type,
                 VT_LOCAL | VT_LVAL,
                 loc);
	loc++;
	if (bt == VT_FLOAT) {
	    type.ti.nb_f32++;
	    type.params[nb_args].type = WASM_FLOAT_32;
	} else if (bt == VT_DOUBLE) {
	    type.ti.nb_f64++;
	    type.params[nb_args].type = WASM_FLOAT_64;
	} else if (bt == VT_LLONG) {
	    type.ti.nb_i64++;
	    type.params[nb_args].type = WASM_INT_64;
	} else {
	    type.ti.nb_i32++;
	    type.params[nb_args].type = WASM_INT_32;
	}
	nb_args++;
    }
    type.ti.nb_params = nb_args;
    type.ti.stack_len = nb_args;
    type.ti.block_cnt = 0;

    type_idx = find_type(&type);
    if (type_idx < 0) {
	type_idx = wasm_type_cnt++;
	all_types[type_idx] = type;
	g_type(type.ti.type);
	printf("g_type: %x- params: %d\n", type.ti.type, type.ti.nb_params);
	printf("gtype %p el0: %x\n", type_section, type_section->data[0]);
	g_type(type.ti.nb_params);
	for (i = 0; i < type.ti.nb_params; ++i) {
		printf("param type %x\n", type.params[i].type);
		g_type(type.params[i].type);
	}
    }

    if ((func_vt.t & VT_BTYPE) == VT_VOID) {
	    g_type(0);
    } else {
	    g_type(1);
	    switch (func_vt.t & VT_BTYPE) {
	    case VT_INT:
		    g_type(WASM_INT_32);
		    break;
	    case VT_LLONG:
		    g_type(WASM_INT_64);
		    break;
	    case VT_FLOAT:
		    g_type(WASM_FLOAT_32);
		    break;
	    case VT_DOUBLE:
		    g_type(WASM_FLOAT_64);
		    break;
	    }
    }

    cur_function = (void *)&all_types[type_idx];
    g_func(type_idx);

    func_size_ind = ind;
    g_code(0); /* size pos, to fixup at epilog */
    g_code(0); /* nb locals, to fixup at epilog */
    ++nb_func;

    sym = osym;
    i = 0;
    while ((sym = sym->next) != NULL) {
	CType *t;
	int bt;

	t = &sym->type;
	bt = t->t & VT_BTYPE;

	g_code(I32_CONST);
	g_code_int(i * 4);
	g_code(LOCAL_GET);
	g_code_int(i++);
	switch (bt) {
	case VT_INT:
	    g_code(I32_STORE);
	    break;
	case VT_DOUBLE:
	    g_code(F64_STORE);
	    break;
	case VT_FLOAT:
	    g_code(F32_STORE);
	    break;
	case VT_LLONG:
	    g_code(I64_STORE);
	    break;
	}
	g_code_int(2); /* alignement */
	g_code_int(0);  /* offset */
    }
    printf("========= gfunc_prolog %s(func_sym) [fc: %d, nargs: %d] =======\n", get_tok_str(func_sym->v, NULL), func_call, nb_args);
}

ST_FUNC int gfunc_sret(CType *vt, int variadic, CType *ret,
                       int *ret_align, int *regsize)
{
    printf("gfunc_sret(vt, %d, ret, ret_align, regsize)\n", variadic);
    return 0;
}

ST_FUNC void arch_transfer_ret_regs(int aftercall)
{
    printf("arch_transfer_ret_regs(%d)\n", aftercall);
}

ST_FUNC void gfunc_epilog(void)
{
    int func_size;
    int func_nb_local = cur_function->stack_len;

    printf("^^^^ gfunc_epilog() ^^^^^\n");
    func_size = ind - func_size_ind;
    g_code(END);
    if (func_size > 255) {
	tcc_error_noabort("function too big, wasm need fixup !");
	/* too fixup: memmove all fucntion byte code */
	return;
    }
    if (func_nb_local) {
	    int nb_types = 0 + cur_function->nb_i32 ? 1 : 0 + cur_function->nb_i64 ? 1 : 0
		+ cur_function->nb_f64 ? 1 : 0 + cur_function->nb_f32 ? 1 : 0;
	    int code_tot_size = ind + nb_types * 2;
	    int i, i2 = 2;

	    if (code_tot_size > code_section->data_allocated)
		    section_realloc(code_section, code_tot_size);
	    memmove(&code_section->data[func_size_ind + 2 + nb_types * 2],
		    &code_section->data[func_size_ind + 2], func_size);
	    code_section->data[func_size_ind + 1] = nb_types;

#define PUSH_LOC(what, byte)						\
	    if (cur_function->nb_##what) {				\
		code_section->data[func_size_ind + i2] = cur_function->nb_i32; \
		++i2;							\
		code_section->data[func_size_ind + i2] = byte;		\
	    }

	    PUSH_LOC(i32, WASM_INT_32);
	    PUSH_LOC(i64, WASM_INT_64);
	    PUSH_LOC(f32, WASM_FLOAT_32);
	    PUSH_LOC(f64, WASM_FLOAT_64);

#undef PUSH_LOC

	    code_section->data[func_size_ind] = func_size + nb_types * 2;
	    ind += nb_types * 2;
    } else {
	    code_section->data[func_size_ind] = func_size;
    }
}

ST_FUNC void gen_va_start(void)
{
    printf("gen_va_start()\n");
}

ST_FUNC void gen_fill_nops(int bytes)
{
    printf("gen_fill_nops(%d)\n", bytes);
    while (bytes--)
	g_code(NOP);
}

static void mk_block(void)
{
    g_code(BLOCK);
    g_code(VOID);
}

ST_FUNC int gjmp(int t)
{
    mk_block();
    printf("gjmp(%d)\n", t);
    return t;
}

ST_FUNC void gjmp_addr(int a)
{
    mk_block();
    printf("gjmp_addr(%d)\n", a);
}

ST_FUNC int gjmp_cond(int op, int t)
{
    mk_block();
    g_code(LOCAL_GET);
    g_code(cur_function->cmp_i32_loc);
    g_code(I32_EQZ);
    g_code(BR_IF);
    g_code(cur_function->block_cnt++);
    printf("gjmp_cond(%d, %d)\n", op, t);
    t = ind;
    return t;
}

ST_FUNC int gjmp_append(int n, int t)
{
    /* mk_block(); */
    printf("gjmp_append(%d, %d)\n", n, t);
    return t;
}

ST_FUNC void gen_opi(int op)
{
    /*
     * NOTE: vtop[-1/0] are the 2 "thing"" on which i should do operations
     * it contain .r, which tell me the type, if they are constant, or a variable
     * if vtop->c contain a constant value, so i contain the int
     * otherwise, I need to use gv()/gv2() to convert vtop[0].r into a register location
     */

    /* don't know why this is done */
    int d = get_reg(RC_INT);
    /* CType arg0_t = vtop[-1].type; */
    /* CType arg1_t = vtop[0].type; */

    printf("------ gen_opi(%x - '%c') ------ \n", op, op);
    /* printf("vtop -1 r (%x): ", vtop[-1].type.t); */
    // print_r_mask(vtop[-1].r, arg0_t);
    /* printf("vtop -1: %lx - %ld ", vtop[-1].c.i, vtop[-1].c.i); */
    /* if (vtop[-1].sym) { */
    /* 	    printf("(%p - %ld - %s)\n", vtop[-1].sym, vtop[-1].sym ? vtop[-1].sym->c : -1L, get_tok_str(vtop[-1].sym->v, NULL)); */
    /* } else { */
    /* 	printf("(ny sym)\n"); */
    /* } */
    /* printf("vtop 0 r (%x): ", vtop[0].type.t); */
    // print_r_mask(vtop[0].r, arg0_t);

    /* printf("vtop 0: %lx - %ld ", vtop[0].c.i, vtop[0].c.i); */
    /* if (vtop[0].sym) { */
    /* 	printf("(%p - %ld - %s)\n", vtop[0].sym, vtop[0].sym ? vtop[0].sym->c : 0L, get_tok_str(vtop[0].sym->v, NULL)); */
    /* } else { */
    /* 	printf("(no sym)\n"); */
    /* } */

    /* printf("%x - %x\n", vtop[-1].r, vtop[0].r); */
    if (!vtop[-1].r) {
	    gv(RC_INT);
    } else {
	    gv2(RC_INT, RC_INT);
    }
    /* printf("OP: '%c'\n", op); */
    switch (op) {
    case '+':
	    g_code(I32_ADD);
	    break;
    case '-':
	    g_code(I32_SUB);
	    break;
    case '*':
	    g_code(I32_MUL);
	    break;
    case '/':
	    g_code(I32_DIV_S);
	    break;
    case TOK_EQ:
	    g_code(I32_EQ);
	    break;
    case TOK_NE:
	    g_code(I32_NE);
	    break;
    case TOK_LT:
	    g_code(I32_LT_S);
	    break;
    case TOK_GT:
	    g_code(I32_GT_S);
	    break;
    case TOK_LE:
	    g_code(I32_LE_S);
	    break;
    case TOK_GE:
	    g_code(I32_GE_S);
	    break;
    default:
	    printf("%d - '%c' unimplemented\n", op, op);
    }
    --vtop;
      /* set result to 0, so tcc doesn't try to load this,
       * vtop[0] can be use to cary information about what to do with resul,
       * and is unused here */
    vtop[0].r = 0;
    if (op >= TOK_ULT && op <= TOK_GT) {
	vset_VT_CMP(op);
	g_code(LOCAL_SET); // local set
	g_code_int(cur_function->stack_len);
	cur_function->cmp_i32_loc = cur_function->stack_len;
	cur_function->stack_len++;
	cur_function->nb_i32++;
    }
}

ST_FUNC void gen_opl(int op)
{
    printf("gen_opl(%d)\n", op);
}

ST_FUNC void gen_opf(int op)
{
    printf("gen_opf(%d)\n", op);
}

ST_FUNC void gen_cvt_sxtw(void)
{
    printf("gen_cvt_sxtw()\n");
}

ST_FUNC void gen_cvt_itof(int t)
{
    printf("gen_cvt_itof(%d)\n", t);
}

ST_FUNC void gen_cvt_ftoi(int t)
{
    printf("gen_cvt_ftoi(%d)\n", t);
}

ST_FUNC void gen_cvt_ftof(int dt)
{
    printf("gen_cvt_ftof(%d)\n", dt);
}

ST_FUNC void gen_increment_tcov(SValue *sv)
{
    printf("gen_increment_tcov(sv)\n");
}

ST_FUNC void ggoto(void)
{
    printf("ggoto()\n");
}

ST_FUNC void gen_vla_sp_save(int addr)
{
    printf("gen_vla_sp_save(%d)\n", addr);
}

ST_FUNC void gen_vla_sp_restore(int addr)
{
    printf("gen_vla_sp_restore(%d)\n", addr);
}

ST_FUNC void gen_vla_alloc(CType *type, int align)
{
    printf("gen_vla_alloc(type, %d)\n", align);
}

#endif /* ndef TARGET_DEFS_ONLY */
