#ifndef __INSTRUCTIONTREE_CXX_
#define __INSTRUCTIONTREE_CXX_

#include <map>
#include <vector>
#include <cstdint>
#include <mapbox/variant.hpp>

typedef mapbox::util::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double> argument;

// Enum for internal representation of instructions
enum struct Instruction : uint16_t {
    nop,
    break_,

    // Basic data operations
    ldarg,
    ldloc,
    stloc,
    ldarga,
    starg,
    ldloca,
    ldnull,
    ldc_i4,
    ldc_i8,
    ldc_r4,
    ldc_r8,
    arglist,
    dup,
    pop,
    cpblk,
    initblk,

    // Indirect load 
    ldind_i1,
    ldind_u1,
    ldind_i2,
    ldind_u2,
    ldind_i4,
    ldind_u4,
    ldind_i8,
    ldind_i,
    ldind_r4,
    ldind_r8,
    ldind_ref,

    // Indirect store
    stind_i,
    stind_i1,
    stind_i2,
    stind_i4,
    stind_i8,
    stind_r4,
    stind_r8,
    stind_ref,

    // Arithmetics
    add,
    sub,
    mul,
    div,
    udiv,
    rem,
    urem,
    and_,
    or_,
    xor_,
    shl,
    shr,
    ushr,
    neg,
    not_,

    // Checked arithmetics
    add_ovf,
    mul_ovf,
    sub_ovf,
    add_ovf_un,
    mul_ovf_un,
    sub_ovf_un,
    ckfinite,

    // Conversion
    conv_i1,
    conv_i2,
    conv_i4,
    conv_i8,
    conv_r4,
    conv_r8,
    conv_u1,
    conv_u2,
    conv_i,
    conv_u,
    conv_r_u,

    // Checked conversion
    conv_ovf_i1,
    conv_ovf_i2,
    conv_ovf_i4,
    conv_ovf_i8,
    conv_ovf_u1,
    conv_ovf_u2,
    conv_ovf_u4,
    conv_ovf_u8,
    conv_ovf_i,
    conv_ovf_u,

    // Control flow
    jmp,
    call,
    calli,
    ret,
    br,
    brfalse,
    brtrue,
    beq,
    bne,
    bge,
    bgt,
    ble,
    blt,
    ubge,
    ubgt,
    uble,
    ublt,
    switch_,
    ceq,
    cgt,
    ucgt,
    clt,
    uclt,
    leave,
    endfinally,
    endfilter,
    rethrow,

    // Some object model instructions
    callvirt,
    cpobj,
    ldobj,
    ldstr,
    castclass,
    isinst,
    throw_,
    ldfld,
    ldflda,
    stfld,
    ldsfld,
    ldsflda,
    stsfld,
    stobj,
    ldlen,
    newarr,
    newobj,
    box,
    unbox,
    unbox_any,
    refanyval,
    mkrefany,
    ldtoken,
    ldftn,
    ldvirtftn,
    initobj,
    sizeof_,
       
    // Load element of array
    ldelem,
    ldelema,
    ldelem_i1,
    ldelem_u1,
    ldelem_i2,
    ldelem_u2,
    ldelem_i4,
    ldelem_u4,
    ldelem_i8,
    ldelem_i,
    ldelem_r4,
    ldelem_r8,
    ldelem_ref,

    // Store element into array
    stelem,
    stelem_i,
    stelem_i1,
    stelem_i2,
    stelem_i4,
    stelem_i8,
    stelem_r4,
    stelem_r8,
    stelem_ref
};

struct InstructionTree {
    // Branch targets, will be suitable for linking
    std::vector<int32_t> targets;
    // offset -> (instruction, [arg1, arg2, ...])
    std::map<int32_t, std::pair<Instruction, std::vector<argument> > > tree;
};

#endif
