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

#if defined(CONFIG_TCC_BCHECK)
static addr_t func_bound_offset;
static unsigned long func_bound_ind;
ST_DATA int func_bound_add_epilog;
#endif

ST_FUNC void o(unsigned int c)
{
    printf("o(%d)\n", c);
}

ST_FUNC void gsym_addr(int t_, int a_)
{
    printf("gsym_addr(%d %d)\n", t_, a_);
}

ST_FUNC void load(int r, SValue *sv)
{
    printf("load(%d, sv)\n", r);
}

ST_FUNC void store(int r, SValue *sv)
{
    printf("store(%d. sv)\n", r);
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

ST_FUNC void gfunc_prolog(Sym *func_sym)
{
    Sym *sym;
    int func_call;
    CType *func_type = &func_sym->type;
    int nb_args = 0;

    sym = func_type->ref;
    func_call = sym->f.func_call;
    loc = 0;
    func_vc = 0;
    while ((sym = sym->next) != NULL) {
	sym_push(sym->v & ~SYM_FIELD, &sym->type,
                 VT_LOCAL | VT_LVAL,
                 loc);
	loc++;
	nb_args++;
    }
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
    printf("gfunc_epilog()\n");
    printf("vtop: %p\n", vtop);
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
    printf("gen_opi(%d - '%c')\n", op, op);
    int d = get_reg(RC_INT);
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
