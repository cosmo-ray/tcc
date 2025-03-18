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

#define USING_GLOBALS
#include "tcc.h"

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
    printf("f section size: %d\n", wasm_func_ind);
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
    printf("t section size: %d\n", type_ind);
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

/*
 * a number literal in the code, take only the place it require
 * so even if it's a VT_INT, it still might need to be shrink
 */
static void g_code_int(int i)
{
    char len_bytes[5] = {0};
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
    char len_bytes[5] = {0};
    char cur;

  again:
    cur = i & 0x7f;
    i = (i & 0xffffff80) >> 7;
    if (i) {
	cur |= 0x80;
	printf("i: %x - c: %x\n", i, (unsigned int)cur);
	g_mem(cur);
	goto again;
    }
    printf("i: %x - cur: %x\n", i, (unsigned)cur);
    g_mem(cur);
}

ST_FUNC void o(unsigned int c)
{
    printf("o(%d)\n", c);
}

ST_FUNC void gsym_addr(int t_, int a_)
{
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
ST_FUNC void load(int r, SValue *sv)
{
    uint32_t or = sv->r & VT_VALMASK;
    CType t = sv->type;

    printf("==== load(%d, sv)=====\n", r);
    printf("sv: %ld ", sv->c.i);
    printf("r: %x\n", r);
    printf("SV->R:");
    //print_r_mask(sv->r, t);
    if (vtop[-1].sym) {
	    printf("(%p - %ld - %s)\n", sv->sym, sv->sym ? sv->sym->c : -1, get_tok_str(sv->sym->v, NULL));
    } else {
	    printf("(ny sym)\n");
    }
    printf("\n");
    if (or == VT_CONST) {
	int local_idx = cur_function->nb_params + (-1 * (sv->c.i / 4)) - 1;
	printf("cur func: %d\n", cur_function->nb_params);

	/* load const into mem */
	/* sv->c.i value if VT_INT */
	if (((t.t & VT_BTYPE) == VT_INT)) {
	    g_code(0x41);
	    g_code_int(sv->c.i);
	    g_code(0x21);
	    g_code_int(cur_function->stack_len++);
	    printf("need to store at %d\n", local_idx);
	}
    }

}

/* store thing from wasm stack into emulated stack (aka memory)  */
ST_FUNC void store(int r, SValue *sv)
{
    uint32_t or = sv->r & VT_VALMASK;
    CType t = sv->type;

    printf("store(%d. sv)\n", r);
    printf("sv: %ld ", sv->c.i);
    printf("SV->R: ");
    //print_r_mask(sv->r, sv->type);
    if (vtop[-1].sym) {
	    printf("(%p - %ld - %s)\n", sv->sym, sv->sym ? sv->sym->c : -1, get_tok_str(sv->sym->v, NULL));
    } else {
	    printf("(ny sym)\n");
    }
    if (or == VT_LOCAL) {
	/* load const into mem */
	/* sv->c.i value if VT_INT */
	/* if there is 2  param, then param at index 2, is the first non param argument*/
	int local_idx = cur_function->nb_params + (-1 * (sv->c.i / 4)) - 1;
	printf("nb param: %d\n", cur_function->nb_params);
	printf("sv->c.i: %d\n", sv->c.i);
	printf("store wasm stack index: %d\n",
	       cur_function->nb_params + 1 + (-1 * (sv->c.i / 4)));
	if (((t.t & VT_BTYPE) == VT_INT)) {
	    int tmp = sv->c.i * -1;

	    printf("store int !");
	    /* g_code_int(&sv->c.i); */
	    g_code(0x20); // local get
	    g_code_int(local_idx); // local index
	    g_code(0x36); // store instruction
	    g_code(0); // store alignement
	    g_code_int(tmp); // store offset
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

int memory_limit = 0x8002;

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
    Sym *sym;
    int func_call;
    CType *func_type = &func_sym->type;
    int nb_args = 0;
    struct wasm_type type = {0};
    int type_idx;
    int i;

    if (!mem_ind) {
	printf("init mem\n");
	init_mem();
    }
    type.ti.type = 0x60;

    sym = func_type->ref;
    func_call = sym->f.func_call;
    loc = 0;
    func_vc = 0;
    while ((sym = sym->next) != NULL) {
	sym_push(sym->v & ~SYM_FIELD, &sym->type,
                 VT_LOCAL | VT_LVAL,
                 loc);
	loc++;
	type.params[nb_args].type = 0x7f;
	nb_args++;
    }
    type.ti.nb_params = nb_args;
    type.ti.stack_len = nb_args;
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
	g_type(type.ti.ret);
	if (type.ti.ret) {
	    g_type(1);
	    g_type(type.ti.ret);
	}
    }
    cur_function = &all_types[type_idx];
    g_func(type_idx);

    func_size_ind = ind;
    g_code(0); /* size pos, to fixup at epilog */
    g_code(0); /* number of local aruments (local decl count), need to be fixup too */
    ++nb_func;

    printf("gfunc_prolog %s(func_sym) [fc: %d, nargs: %d]\n", get_tok_str(func_sym->v, NULL), func_call, nb_args);
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

    printf("gfunc_epilog()\n");
    printf("vtop: %p\n", vtop);
    func_size = ind - func_size_ind;
    g_code(0x0b);
    if (func_size > 255) {
	tcc_error_noabort("function too big, wasm need fixup !");
	/* too fixup: memmove all fucntion byte code */
	return;
    }
    code_section->data[func_size_ind] = func_size;
}

ST_FUNC void gen_va_start(void)
{
    printf("gen_va_start()\n");
}

ST_FUNC void gen_fill_nops(int bytes)
{
    printf("gen_fill_nops(%d)\n", bytes);
}

ST_FUNC int gjmp(int t)
{
    printf("gjmp(%d)\n", t);
    return 0;
}

ST_FUNC void gjmp_addr(int a)
{
    printf("gjmp_addr(%d)\n", a);
}

ST_FUNC int gjmp_cond(int op, int t)
{
    printf("gjmp_cond(%d, %d)\n", op, t);
    return 0;
}

ST_FUNC int gjmp_append(int n, int t)
{
    printf("gjmp_append(%d, %d)\n", n, t);
    return 0;
}

ST_FUNC void gen_opi(int op)
{
    int d = get_reg(RC_INT);
    CType arg0_t = vtop[-1].type;
    CType arg1_t = vtop[0].type;

    printf("gen_opi(%d - '%c')\n", op, op);
    printf("vtop -1 r (%x): ", vtop[-1].type.t);
    // print_r_mask(vtop[-1].r, arg0_t);
    printf("vtop -1: %lx - %ld ", vtop[-1].c.i, vtop[-1].c.i);
    if (vtop[-1].sym) {
	printf("(%p - %ld - %s)\n", vtop[-1].sym, vtop[-1].sym ? vtop[-1].sym->c : -1, get_tok_str(vtop[-1].sym->v, NULL));
    } else {
	printf("(ny sym)\n");
    }
    printf("vtop 0 r (%x): ", vtop[0].type.t);
    // print_r_mask(vtop[0].r, arg0_t);

    printf("vtop 0: %lx - %ld ", vtop[0].c.i, vtop[0].c.i);
    if (vtop[0].sym) {
	printf("(%p - %ld - %s)\n", vtop[0].sym, vtop[0].sym ? vtop[0].sym->c : 0, get_tok_str(vtop[0].sym->v, NULL));
    } else {
	printf("(no sym)\n");
    }

    printf("%x - %x\n", vtop[-1].r, vtop[0].r);
    switch (op) {
    case '+':
	    g_code(0x6a);
	    break;
    default:
	    printf("%d - '%c' unimplemented\n", op, op);
    }
    --vtop;
    vtop[0].r = d;
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
