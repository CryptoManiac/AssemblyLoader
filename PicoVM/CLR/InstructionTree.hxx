#ifndef __INSTRUCTIONTREE_CXX_
#define __INSTRUCTIONTREE_CXX_

#include <map>
#include <vector>
#include <cstdint>
#include <mapbox/variant.hpp>

typedef mapbox::util::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double> argument;

// Enum for internal representation of instructions
enum struct Instruction : uint16_t {
    // "No operation"
    i_nop,

    // Basic data operations
    i_ldarg,
    i_ldloc,
    i_stloc,
    i_ldarga,
    i_starg,
    i_ldloca,
    i_ldnull,
    i_ldc_i4,
    i_ldc_i8,
    i_ldc_r4,
    i_ldc_r8,
    i_arglist,
    i_dup,
    i_pop,
    i_cpblk,
    i_initblk,

    // Indirect load 
    i_ldind_i1,
    i_ldind_u1,
    i_ldind_i2,
    i_ldind_u2,
    i_ldind_i4,
    i_ldind_u4,
    i_ldind_i8,
    i_ldind_i,
    i_ldind_r4,
    i_ldind_r8,
    i_ldind_ref,

    // Indirect store
    i_stind_i,
    i_stind_i1,
    i_stind_i2,
    i_stind_i4,
    i_stind_i8,
    i_stind_r4,
    i_stind_r8,
    i_stind_ref,

    // Arithmetics
    i_add,
    i_sub,
    i_mul,
    i_div,
    i_div_u,
    i_rem,
    i_rem_u,
    i_and,
    i_or,
    i_xor,
    i_shl,
    i_shr,
    i_shr_u,
    i_neg,
    i_not,

    // Checked arithmetics
    i_add_ovf,
    i_mul_ovf,
    i_sub_ovf,
    i_add_ovf_u,
    i_mul_ovf_u,
    i_sub_ovf_u,
    i_ckfinite,

    // Conversion
    i_conv_i1,
    i_conv_i2,
    i_conv_i4,
    i_conv_i8,
    i_conv_r4,
    i_conv_r8,
    i_conv_u1,
    i_conv_u2,
    i_conv_i,
    i_conv_u,
    i_conv_r_u,

    // Checked conversion
    i_conv_ovf_i1,
    i_conv_ovf_i2,
    i_conv_ovf_i4,
    i_conv_ovf_i8,
    i_conv_ovf_u1,
    i_conv_ovf_u2,
    i_conv_ovf_u4,
    i_conv_ovf_u8,
    i_conv_ovf_i,
    i_conv_ovf_u,

    // Control flow
    i_jmp,
    i_call,
    i_calli,
    i_ret,
    i_br,
    i_brfalse,
    i_brtrue,
    i_beq,
    i_bne,
    i_bge,
    i_bgt,
    i_ble,
    i_blt,
    i_bge_u,
    i_bgt_u,
    i_ble_u,
    i_blt_u,
    i_switch,
    i_ceq,
    i_cgt,
    i_cgt_u,
    i_clt,
    i_clt_u,
    i_leave,
    i_endfinally,
    i_endfilter,
    i_rethrow,

    // Some object model instructions
    i_callvirt,
    i_cpobj,
    i_ldobj,
    i_ldstr,
    i_castclass,
    i_isinst,
    i_throw_,
    i_ldfld,
    i_ldflda,
    i_stfld,
    i_ldsfld,
    i_ldsflda,
    i_stsfld,
    i_stobj,
    i_ldlen,
    i_newarr,
    i_newobj,
    i_box,
    i_unbox,
    i_unbox_any,
    i_refanyval,
    i_mkrefany,
    i_ldtoken,
    i_ldftn,
    i_ldvirtftn,
    i_initobj,
    i_sizeof_,

    // Load element of array
    i_ldelem,
    i_ldelema,
    i_ldelem_i1,
    i_ldelem_u1,
    i_ldelem_i2,
    i_ldelem_u2,
    i_ldelem_i4,
    i_ldelem_u4,
    i_ldelem_i8,
    i_ldelem_i,
    i_ldelem_r4,
    i_ldelem_r8,
    i_ldelem_ref,

    // Store element into array
    i_stelem,
    i_stelem_i,
    i_stelem_i1,
    i_stelem_i2,
    i_stelem_i4,
    i_stelem_i8,
    i_stelem_r4,
    i_stelem_r8,
    i_stelem_ref
};

struct InstructionTree {
    // Branch targets, will be suitable for linking
    std::vector<int32_t> targets;
    // offset -> (instruction, [arg1, arg2, ...])
    std::map<int32_t, std::pair<Instruction, std::vector<argument> > > tree;
};

#endif
