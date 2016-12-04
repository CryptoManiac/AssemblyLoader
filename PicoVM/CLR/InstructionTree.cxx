#include "InstructionTree.hxx"
#include "EnumCasting.hxx"
#include "NumCasting.hxx"


#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

// One-byte opcodes
enum struct ShortCode : uint8_t {
    i_nop = 0x00,
    i_break = 0x01,
    i_ldarg_0 = 0x02,
    i_ldarg_1 = 0x03,
    i_ldarg_2 = 0x04,
    i_ldarg_3 = 0x05,
    i_ldloc_0 = 0x06,
    i_ldloc_1 = 0x07,
    i_ldloc_2 = 0x08,
    i_ldloc_3 = 0x09,
    i_stloc_0 = 0x0A,
    i_stloc_1 = 0x0B,
    i_stloc_2 = 0x0C,
    i_stloc_3 = 0x0D,
    i_ldarg_s = 0x0E,
    i_ldarga_s = 0x0F,
    i_starg_s = 0x10,
    i_ldloc_s = 0x11,
    i_ldloca_s = 0x12,
    i_stloc_s = 0x13,
    i_ldnull = 0x14,
    i_ldc_i4_m1 = 0x15,
    i_ldc_i4_0 = 0x16,
    i_ldc_i4_1 = 0x17,
    i_ldc_i4_2 = 0x18,
    i_ldc_i4_3 = 0x19,
    i_ldc_i4_4 = 0x1A,
    i_ldc_i4_5 = 0x1B,
    i_ldc_i4_6 = 0x1C,
    i_ldc_i4_7 = 0x1D,
    i_ldc_i4_8 = 0x1E,
    i_ldc_i4_s = 0x1F,
    i_ldc_i4 = 0x20,
    i_ldc_i8 = 0x21,
    i_ldc_r4 = 0x22,
    i_ldc_r8 = 0x23,
    i_dup = 0x25,
    i_pop = 0x26,
    i_jmp = 0x27,
    i_call = 0x28,
    i_calli = 0x29,
    i_ret = 0x2A,
    i_br_s = 0x2B,
    i_brfalse_s = 0x2C,
    i_brtrue_s = 0x2D,
    i_beq_s = 0x2E,
    i_bge_s = 0x2F,
    i_bgt_s = 0x30,
    i_ble_s = 0x31,
    i_blt_s = 0x32,
    i_bne_un_s = 0x33,
    i_bge_un_s = 0x34,
    i_bgt_un_s = 0x35,
    i_ble_un_s = 0x36,
    i_blt_un_s = 0x37,
    i_br = 0x38,
    i_brfalse = 0x39,
    i_brtrue = 0x3A,
    i_beq = 0x3B,
    i_bge = 0x3C,
    i_bgt = 0x3D,
    i_ble = 0x3E,
    i_blt = 0x3F,
    i_bne_un = 0x40,
    i_bge_un = 0x41,
    i_bgt_un = 0x42,
    i_ble_un = 0x43,
    i_blt_un = 0x44,
    i_switch = 0x45,
    i_ldind_i1 = 0x46,
    i_ldind_u1 = 0x47,
    i_ldind_i2 = 0x48,
    i_ldind_u2 = 0x49,
    i_ldind_i4 = 0x4A,
    i_ldind_u4 = 0x4B,
    i_ldind_i8 = 0x4C,
    i_ldind_i = 0x4D,
    i_ldind_r4 = 0x4E,
    i_ldind_r8 = 0x4F,
    i_ldind_ref = 0x50,
    i_stind_ref = 0x51,
    i_stind_i1 = 0x52,
    i_stind_i2 = 0x53,
    i_stind_i4 = 0x54,
    i_stind_i8 = 0x55,
    i_stind_r4 = 0x56,
    i_stind_r8 = 0x57,
    i_add = 0x58,
    i_sub = 0x59,
    i_mul = 0x5A,
    i_div = 0x5B,
    i_div_un = 0x5C,
    i_rem = 0x5D,
    i_rem_un = 0x5E,
    i_and = 0x5F,
    i_or = 0x60,
    i_xor = 0x61,
    i_shl = 0x62,
    i_shr = 0x63,
    i_shr_un = 0x64,
    i_neg = 0x65,
    i_not = 0x66,
    i_conv_i1 = 0x67,
    i_conv_i2 = 0x68,
    i_conv_i4 = 0x69,
    i_conv_i8 = 0x6A,
    i_conv_r4 = 0x6B,
    i_conv_r8 = 0x6C,
    i_conv_u4 = 0x6D,
    i_conv_u8 = 0x6E,
    i_callvirt = 0x6F,
    i_cpobj = 0x70,
    i_ldobj = 0x71,
    i_ldstr = 0x72,
    i_newobj = 0x73,
    i_castclass = 0x74,
    i_isinst = 0x75,
    i_conv_r_un = 0x76,
    i_unbox = 0x79,
    i_throw = 0x7A,
    i_ldfld = 0x7B,
    i_ldflda = 0x7C,
    i_stfld = 0x7D,
    i_ldsfld = 0x7E,
    i_ldsflda = 0x7F,
    i_stsfld = 0x80,
    i_stobj = 0x81,
    i_conv_ovf_i1_un = 0x82,
    i_conv_ovf_i2_un = 0x83,
    i_conv_ovf_i4_un = 0x84,
    i_conv_ovf_i8_un = 0x85,
    i_conv_ovf_u1_un = 0x86,
    i_conv_ovf_u2_un = 0x87,
    i_conv_ovf_u4_un = 0x88,
    i_conv_ovf_u8_un = 0x89,
    i_conv_ovf_i_un = 0x8A,
    i_conv_ovf_u_un = 0x8B,
    i_box = 0x8C,
    i_newarr = 0x8D,
    i_ldlen = 0x8E,
    i_ldelema = 0x8F,
    i_ldelem_i1 = 0x90,
    i_ldelem_u1 = 0x91,
    i_ldelem_i2 = 0x92,
    i_ldelem_u2 = 0x93,
    i_ldelem_i4 = 0x94,
    i_ldelem_u4 = 0x95,
    i_ldelem_i8 = 0x96,
    i_ldelem_i = 0x97,
    i_ldelem_r4 = 0x98,
    i_ldelem_r8 = 0x99,
    i_ldelem_ref = 0x9A,
    i_stelem_i = 0x9B,
    i_stelem_i1 = 0x9C,
    i_stelem_i2 = 0x9D,
    i_stelem_i4 = 0x9E,
    i_stelem_i8 = 0x9F,
    i_stelem_r4 = 0xA0,
    i_stelem_r8 = 0xA1,
    i_stelem_ref = 0xA2,
    i_ldelem = 0xA3,
    i_stelem = 0xA4,
    i_unbox_any = 0xA5,
    i_conv_ovf_i1 = 0xB3,
    i_conv_ovf_u1 = 0xB4,
    i_conv_ovf_i2 = 0xB5,
    i_conv_ovf_u2 = 0xB6,
    i_conv_ovf_i4 = 0xB7,
    i_conv_ovf_u4 = 0xB8,
    i_conv_ovf_i8 = 0xB9,
    i_conv_ovf_u8 = 0xBA,
    i_refanyval = 0xC2,
    i_ckfinite = 0xC3,
    i_mkrefany = 0xC6,
    i_ldtoken = 0xD0,
    i_conv_u2 = 0xD1,
    i_conv_u1 = 0xD2,
    i_conv_i = 0xD3,
    i_conv_ovf_i = 0xD4,
    i_conv_ovf_u = 0xD5,
    i_add_ovf = 0xD6,
    i_add_ovf_un = 0xD7,
    i_mul_ovf = 0xD8,
    i_mul_ovf_un = 0xD9,
    i_sub_ovf = 0xDA,
    i_sub_ovf_un = 0xDB,
    i_endfinally = 0xDC,
    i_leave = 0xDD,
    i_leave_s = 0xDE,
    i_stind_i = 0xDF,
    i_conv_u = 0xE0,
    p_multibyte = 0xFE // Two-byte instruction prefix
};

// Two-byte opcodes and prefixes
enum struct TwoByteCode : uint16_t {
    i_arglist = 0x00FE,
    i_ceq = 0x01FE,
    i_cgt = 0x02FE,
    i_cgt_un = 0x03FE,
    i_clt = 0x04FE,
    i_clt_un = 0x05FE,
    i_ldftn = 0x06FE,
    i_ldvirtftn = 0x07FE,
    i_ldarg = 0x09FE,
    i_ldarga = 0x0AFE,
    i_starg = 0x0BFE,
    i_ldloc = 0x0CFE,
    i_ldloca = 0x0DFE,
    i_stloc = 0x0EFE,
    i_localloc = 0x0FFE,
    i_endfilter = 0x11FE,
    p_unaligned = 0x12FE, // Prefix
    p_volatile = 0x13FE, // Prefix 
    p_tail = 0x14FE, // Prefix
    i_initobj = 0x15FE,
    p_constrained = 0x16FE, // Prefix 
    i_cpblk = 0x17FE,
    i_initblk = 0x18FE,
    p_no = 0x19FE, // Prefix 
    i_rethrow = 0x1AFE,
    i_sizeof = 0x1CFE,
    i_refanytype = 0x1DFE,
    p_readonly = 0x1EFE, // Prefix 
};

inline static int32_t read_int32(vector<uint8_t>::iterator& it) {
    it += 4;
    return *reinterpret_cast<int32_t*>(&(*(it - 4)));
}

inline static int64_t read_int64(vector<uint8_t>::iterator& it) {
    it += 8;
    return *reinterpret_cast<int64_t*>(&(*(it - 8)));
}

inline static float read_float(vector<uint8_t>::iterator& it) {
    it += 4;
    return *reinterpret_cast<float*>(&(*(it - 4)));
}

inline static double read_double(vector<uint8_t>::iterator& it) {
    it += 8;
    return *reinterpret_cast<double*>(&(*(it - 8)));
}

static bool is_branching(const pair<Instruction, vector<argument> >& instr, vector<ptrdiff_t>& targets) {

    switch(instr.first)
    {
        case Instruction::i_br:
        case Instruction::i_brfalse:
        case Instruction::i_brtrue:
        case Instruction::i_beq:
        case Instruction::i_bge:
        case Instruction::i_bgt:
        case Instruction::i_ble:
        case Instruction::i_blt:
        case Instruction::i_bne_un:
        case Instruction::i_bge_un:
        case Instruction::i_bgt_un:
        case Instruction::i_ble_un:
        case Instruction::i_blt_un:
        case Instruction::i_leave:
        {
            auto target = instr.second.begin()->get<ptrdiff_t>();
            targets.push_back(target);
            return true;
        }
        case Instruction::i_switch:
        {
            for (auto it = instr.second.begin(); it != instr.second.end(); ++it) {
                auto target = it->get<ptrdiff_t>();
                targets.push_back(target);
            }

            return true;
        }
        default:
            return false;
    }
}

static pair<Instruction, vector<argument> > loadOp(ptrdiff_t offset, vector<uint8_t>::iterator& it) {
    using sc = ShortCode;
    using tb = TwoByteCode;

    auto opcode = static_cast<sc>(*(it++));

    switch(opcode) {
        // Predefined aliases for ldarg [uint16_t num] where num is between 0 and 3.
        case sc::i_ldarg_0:
        case sc::i_ldarg_1:
        case sc::i_ldarg_2:
        case sc::i_ldarg_3:
        {
            auto arg = static_cast<uint16_t>(_u(opcode) - _u(sc::i_ldarg_0));
            return pair<Instruction, vector<argument> >(Instruction::i_ldarg, { arg });
        }

        // Predefined aliases for ldloc [uint16_t num] where num is between 0 and 3.
        case sc::i_ldloc_0:
        case sc::i_ldloc_1:
        case sc::i_ldloc_2:
        case sc::i_ldloc_3:
        {
            auto arg = static_cast<uint16_t>(_u(opcode) - _u(sc::i_ldloc_0));
            return pair<Instruction, vector<argument> >(Instruction::i_ldloc, { arg });
        }

        // Predefined aliases for stloc [uint16_t num] where num is between 0 and 3.
        case sc::i_stloc_0:
        case sc::i_stloc_1:
        case sc::i_stloc_2:
        case sc::i_stloc_3:
        {
            auto arg = static_cast<uint16_t>(_u(opcode) - _u(sc::i_stloc_0));
            return pair<Instruction, vector<argument> >(Instruction::i_stloc, { arg });
        }

        // Predefined aliases for ldc.i4 [int32_t num] where num is between -1 and 8.
        case sc::i_ldc_i4_m1:
        case sc::i_ldc_i4_0:
        case sc::i_ldc_i4_1:
        case sc::i_ldc_i4_2:
        case sc::i_ldc_i4_4:
        case sc::i_ldc_i4_5:
        case sc::i_ldc_i4_6:
        case sc::i_ldc_i4_7:
        case sc::i_ldc_i4_8:
        {
            int32_t arg = _u(opcode) - _u(sc::i_ldc_i4_0);
            return pair<Instruction, vector<argument> >(Instruction::i_ldc_i4, { arg });
        }

        // Load argument (short version)
        // ldarg.s [uint8_t argnum]
        case sc::i_ldarg_s:
        {
            uint16_t arg = *(it++);
            return pair<Instruction, vector<argument> >(Instruction::i_ldarg, { arg });
        }

        // Load argument address (short version)
        // ldarga.s [uint8_t argnum]
        case sc::i_ldarga_s:
        {
            uint16_t arg = *(it++);
            return pair<Instruction, vector<argument> >(Instruction::i_ldarga, { arg });
        }

        // Save argument (short version)
        // starg.s [uint8_t argnum]
        case sc::i_starg_s:
        {
            uint16_t arg = *(it++);
            return pair<Instruction, vector<argument> >(Instruction::i_starg, { arg });
        }

        // Load local variable (short version)
        // ldloc.s [uint8_t varnum]
        case sc::i_ldloc_s:
        {
            uint16_t arg = *(it++);
            return pair<Instruction, vector<argument> >(Instruction::i_ldloc, { arg });
        }

        // Load local variable address (short version)
        // ldloca.s [uint8_t varnum]
        case sc::i_ldloca_s:
        {
            uint16_t arg = *(it++);
            return pair<Instruction, vector<argument> >(Instruction::i_ldloca, { arg });
        }

        // Save local variable (short version)
        // stloc.s [uint8_t varnum]
        case sc::i_stloc_s:
        {
            uint16_t arg = *(it++);
            return pair<Instruction, vector<argument> >(Instruction::i_stloc, { arg });
        }

        // Load integer as int32_t (short version)
        // ldc.i4.s [int8_t smallint]
        case sc::i_ldc_i4_s:
        {
            int32_t arg = static_cast<int8_t>(*(it++));
            return pair<Instruction, vector<argument> >(Instruction::i_ldc_i4, { arg });
        }

        // Load integer as int32_t
        // ldc.i4 [int32_t int]
        case sc::i_ldc_i4:
        {
            auto arg = read_int32(it);
            return pair<Instruction, vector<argument> >(Instruction::i_ldc_i4, { arg });
        }

        // Load integer as int64_t
        // ldc.i8 [int64_t int]
        case sc::i_ldc_i8:
        {
            auto arg = read_int64(it);
            return pair<Instruction, vector<argument> >(Instruction::i_ldc_i8, { arg });
        }

        // Load float as double
        // ldc.r4 [float num]
        case sc::i_ldc_r4:
        {
            auto arg = read_float(it);
            return pair<Instruction, vector<argument> >(Instruction::i_ldc_r4, { arg });
        }

        // Load double
        // ldc.r8 [double num]
        case sc::i_ldc_r8:
        {
            auto arg = read_double(it);
            return pair<Instruction, vector<argument> >(Instruction::i_ldc_r8, { arg });
        }

        // Short representations of branching opcodes
        // xx [int8_t target]
        case sc::i_br_s:
        case sc::i_brfalse_s:
        case sc::i_brtrue_s:
        case sc::i_beq_s:
        case sc::i_bge_s:
        case sc::i_bgt_s:
        case sc::i_ble_s:
        case sc::i_blt_s:
        case sc::i_bne_un_s:
        case sc::i_bge_un_s:
        case sc::i_bgt_un_s:
        case sc::i_ble_un_s:
        case sc::i_blt_un_s:
        {
            auto newcode = _u(sc::i_br) + (_u(opcode) - _u(sc::i_br_s));
            auto target = static_cast<int8_t>(*(it++));
            return pair<Instruction, vector<argument> >(static_cast<Instruction>(newcode), { offset + 2 + target });
        }

        // Leave protected region of code
        // leave.s [int8_t offset]
        case sc::i_leave_s:
        {
            auto target = static_cast<int8_t>(*(it++));
            return pair<Instruction, vector<argument> >(Instruction::i_leave, { offset + 2 + target });
        }

        // Leave normal representation of branching opcodes as is.
        // xx [int32_t target]
        case sc::i_br:
        case sc::i_brfalse:
        case sc::i_brtrue:
        case sc::i_beq:
        case sc::i_bge:
        case sc::i_bgt:
        case sc::i_ble:
        case sc::i_blt:
        case sc::i_bne_un:
        case sc::i_bge_un:
        case sc::i_bgt_un:
        case sc::i_ble_un:
        case sc::i_blt_un:

        // Leave protected region of code
        // leave.s [int32_t offset]
        case sc::i_leave:
        {
            auto target = read_int32(it);
            return pair<Instruction, vector<argument> >(static_cast<Instruction>(opcode), { offset + 5 + target });
        }

        // Jump table instruction
        // switch [uint32_t num] [int32_t offset1, int32_t offset2, ..., int32_t offsetN]
        case sc::i_switch:
        {
            // Switch table size
            auto table_size = read_int32(it);
            auto instr_size = 1 + 4 * (table_size + 1);
            vector<argument> args = { };

            // Jump targets collection
            for (auto i = 0; i < table_size; ++i) {
                auto target = read_int32(it);
                args.push_back(offset + instr_size + target);
            }

            return pair<Instruction, vector<argument> >(Instruction::i_switch, args);
        }

        // Some object model instructions
        // <instruction> [int32_t Token]
        case sc::i_box:
        case sc::i_unbox:
        case sc::i_unbox_any:
        case sc::i_cpobj:
        case sc::i_ldobj:
        case sc::i_stobj:
        case sc::i_isinst:
        case sc::i_ldfld:
        case sc::i_stfld:
        case sc::i_stsfld:
        case sc::i_ldflda:
        case sc::i_ldsfld:
        case sc::i_ldsflda:
        case sc::i_ldstr:
        case sc::i_ldtoken:
        case sc::i_mkrefany:
        case sc::i_newarr:
        case sc::i_newobj:
        case sc::i_refanyval:
        case sc::i_ldelem:
        case sc::i_ldelema:
        case sc::i_stelem:
        case sc::i_castclass:
        case sc::i_jmp:
        case sc::i_call:
        case sc::i_calli:
        case sc::i_callvirt:
        {
            auto token = static_cast<uint32_t>(read_int32(it));
            return pair<Instruction, vector<argument> >(static_cast<Instruction>(opcode), { token });
        }

        // All two-byte instructions and instruction modifiers are marked by 0xFE prefix
        case sc::p_multibyte:
        {
            auto lopcode = tb(static_cast<uint16_t>(_u(sc::p_multibyte) | *(it++) << 8));

            switch (lopcode) {
                // Argumentless instruction
                case tb::i_arglist:
                case tb::i_rethrow:
                case tb::i_ceq:
                case tb::i_cgt:
                case tb::i_clt:
                case tb::i_cgt_un:
                case tb::i_clt_un:
                case tb::i_cpblk:
                case tb::i_initblk:
                case tb::i_localloc:
                case tb::i_endfilter:
                case tb::i_refanytype:
                    return pair<Instruction, vector<argument> >(static_cast<Instruction>(lopcode), {});

                // Local variable and argument operations
                // <instruction> [uint16_t index]
                case tb::i_ldloc:
                case tb::i_ldloca:
                case tb::i_ldarg:
                case tb::i_ldarga:
                case tb::i_starg:
                case tb::i_stloc:
                {
                    auto arg = static_cast<uint16_t>(*(it++) | *(it++) << 8);
                    return pair<Instruction, vector<argument> >(static_cast<Instruction>(lopcode), { arg });
                }

                // Object model instructions
                // <instruction> [int32_t Token]
                case tb::i_sizeof:
                case tb::i_ldftn:
                case tb::i_ldvirtftn:
                case tb::i_initobj:
                {
                    auto token = static_cast<uint32_t>(read_int32(it));
                    return pair<Instruction, vector<argument> >(static_cast<Instruction>(lopcode), { token });
                }

                // Prefixes
                case tb::p_unaligned:
                case tb::p_volatile:
                case tb::p_tail:
                case tb::p_constrained:
                case tb::p_no:
                case tb::p_readonly: {}
            }
        }

        default:
            // Direct conversion of parameterless instructions
            return pair<Instruction, vector<argument> >(static_cast<Instruction>(opcode), {});
    }
}

shared_ptr<InstructionTree> InstructionTree::MakeTree(const vector<uint8_t>& methodData) {
    auto treeObj = new InstructionTree();
    vector<uint8_t> data = methodData;

    for(auto it = data.begin(); it != data.end(); ) {
        auto offset = distance(data.begin(), it);
        auto op = loadOp(offset, it);
        treeObj->tree[offset] = op;

        vector<ptrdiff_t> vtargets;
        if (is_branching(op, vtargets)) {
            treeObj->targets.insert(treeObj->targets.end(), vtargets.begin(), vtargets.end());
        }
    }

    return shared_ptr<InstructionTree>(treeObj);
}

string InstructionTree::str() const {
    using i = Instruction;

    ostringstream s;

    for (const auto &item : tree) {
        auto instr = item.second.first;
        auto args = item.second.second;
        auto offset = item.first;

        s << hex << setw(4) << setfill('0') << offset;

        switch (instr) {
        // Local variable and argument operations
        // <operation> [uint16_t]
        case i::i_ldarg:
        case i::i_ldarga:
        case i::i_ldloc:
        case i::i_ldloca:
        case i::i_starg:
        case i::i_stloc:
        {
            switch (instr) {
            case i::i_ldarg:  s << ": ldarg"; break;
            case i::i_ldarga: s << ": ldarga"; break;
            case i::i_ldloc:  s << ": ldloc"; break;
            case i::i_ldloca: s << ": ldloca"; break;
            case i::i_starg:  s << ": starg"; break;
            case i::i_stloc:  s << ": stloc"; break;
            default:
                throw runtime_error("We shouldn't be here.");
            }

            s << " " << dec << args.begin()->get<uint16_t>();
        }
        break;

        // Load number
        case i::i_ldc_i4: s << ": ldc_i4 " << dec << args.begin()->get<int32_t>(); break;
        case i::i_ldc_i8: s << ": ldc_i8 " << dec << args.begin()->get<int64_t>(); break;
        case i::i_ldc_r4: s << ": ldc_r4 " << dec << args.begin()->get<float>(); break;
        case i::i_ldc_r8: s << ": ldc_r8 " << dec << args.begin()->get<double>(); break;

        // Branching
        case i::i_br:
        case i::i_brfalse:
        case i::i_brtrue:
        case i::i_beq:
        case i::i_bge:
        case i::i_bgt:
        case i::i_ble:
        case i::i_blt:
        case i::i_bne_un:
        case i::i_bge_un:
        case i::i_bgt_un:
        case i::i_ble_un:
        case i::i_blt_un:
        case i::i_leave:
        {
            switch (instr) {
            case i::i_br: s << ": br"; break;
            case i::i_brfalse: s << ": brfalse"; break;
            case i::i_brtrue: s << ": brtrue"; break;
            case i::i_beq: s << ": beq"; break;
            case i::i_bge: s << ": bge"; break;
            case i::i_bgt: s << ": bgt"; break;
            case i::i_ble: s << ": ble"; break;
            case i::i_blt: s << ": blt"; break;
            case i::i_bne_un: s << ": bne_un"; break;
            case i::i_bge_un: s << ": bge_un"; break;
            case i::i_bgt_un: s << ": bgt_un"; break;
            case i::i_ble_un: s << ": ble_un"; break;
            case i::i_blt_un: s << ": blt_un"; break;
            case i::i_leave: s << ": leave"; break;
            default:
                throw runtime_error("We shouldn't be here.");
            }

            s << " " << hex << args.begin()->get<ptrdiff_t>();
        }
        break;

        // Jump table instruction
        // switch [uint32_t num] [int32_t offset1, int32_t offset2, ..., int32_t offsetN]
        case i::i_switch:
        {
            s << ": switch [" << hex << setw(4) << setfill('0');
            for (const auto& elem : args) {
                if (&elem != &args[0]) {
                    s << ", ";
                }

                s << elem.get<ptrdiff_t>();
            }
            s << "]";
        }
        break;

        //// Argumentless instructions
        
        // Arithmetics
        case i::i_add:    s << ": add"; break;
        case i::i_and:    s << ": and"; break;
        case i::i_or:     s << ": or"; break;
        case i::i_xor:    s << ": xor"; break;
        case i::i_neg:    s << ": neg"; break;
        case i::i_not:    s << ": not"; break;
        case i::i_sub:    s << ": sub"; break;
        case i::i_mul:    s << ": mul"; break;
        case i::i_div:    s << ": duv"; break;
        case i::i_div_un: s << ": duv_un"; break;
        case i::i_rem:    s << ": rem"; break;
        case i::i_rem_un: s << ": rem_un"; break;
        case i::i_shr:    s << ": shr"; break;
        case i::i_shr_un: s << ": shr_un"; break;

        // Checked arithmetics
        case i::i_add_ovf:    s << ": add_ovf"; break;
        case i::i_add_ovf_un: s << ": add_ovf_un"; break;
        case i::i_sub_ovf:    s << ": sub_ovf"; break;
        case i::i_sub_ovf_un: s << ": sub_ovf_un"; break;
        case i::i_mul_ovf:    s << ": mul_ovf"; break;
        case i::i_mul_ovf_un: s << ": mul_ovf_un"; break;
        case i::i_ckfinite:  s << ": ckfinite"; break;

        // Indirect load
        case i::i_ldind_i:   s << ": ldind_i"; break;
        case i::i_ldind_i1:  s << ": ldind_i1"; break;
        case i::i_ldind_i2:  s << ": ldind_i2"; break;
        case i::i_ldind_i4:  s << ": ldind_i4"; break;
        case i::i_ldind_i8:  s << ": ldind_i8"; break;
        case i::i_ldind_u1:  s << ": ldind_u1"; break;
        case i::i_ldind_u2:  s << ": ldind_u2"; break;
        case i::i_ldind_u4:  s << ": ldind_u4"; break;
        case i::i_ldind_r4:  s << ": ldind_r4"; break;
        case i::i_ldind_r8:  s << ": ldind_r8"; break;
        case i::i_ldind_ref: s << ": ldind_ref"; break;
        case i::i_stind_ref: s << ": stind_ref"; break;
        case i::i_stind_i:   s << ": stind_i"; break;
        case i::i_stind_i1:  s << ": stind_i1"; break;
        case i::i_stind_i2:  s << ": stind_i2"; break;
        case i::i_stind_i4:  s << ": stind_i4"; break;
        case i::i_stind_i8:  s << ": stind_i8"; break;

        // Working with array elements
        case i::i_ldlen:     s << ": ldlen"; break;
        case i::i_ldelem_i:   s << ": ldelem_i"; break;
        case i::i_ldelem_i1:  s << ": ldelem_i1"; break;
        case i::i_ldelem_i2:  s << ": ldelem_i2"; break;
        case i::i_ldelem_i4:  s << ": ldelem_i4"; break;
        case i::i_ldelem_i8:  s << ": ldelem_i8"; break;
        case i::i_ldelem_u1:  s << ": ldelem_u1"; break;
        case i::i_ldelem_u2:  s << ": ldelem_u2"; break;
        case i::i_ldelem_u4:  s << ": ldelem_u4"; break;
        case i::i_ldelem_r4:  s << ": ldelem_r4"; break;
        case i::i_ldelem_r8:  s << ": ldelem_r8"; break;
        case i::i_ldelem_ref: s << ": ldelem_ref"; break;
        case i::i_stelem_ref: s << ": stelem_ref"; break;
        case i::i_stelem_i:   s << ": stelem_i"; break;
        case i::i_stelem_i1:  s << ": stelem_i1"; break;
        case i::i_stelem_i2:  s << ": stelem_i2"; break;
        case i::i_stelem_i4:  s << ": stelem_i4"; break;
        case i::i_stelem_i8:  s << ": stelem_i8"; break;
        case i::i_stelem_r4:  s << ": stelem_r4"; break;
        case i::i_stelem_r8:  s << ": stelem_r8"; break;

        // Numeric conversion
        case i::i_conv_i:   s << ": conv_i"; break;
        case i::i_conv_i1:  s << ": conv_i1"; break;
        case i::i_conv_i2:  s << ": conv_i2"; break;
        case i::i_conv_i4:  s << ": conv_i4"; break;
        case i::i_conv_i8:  s << ": conv_i8"; break;
        case i::i_conv_u:   s << ": conv_u"; break;
        case i::i_conv_u1:  s << ": conv_u1"; break;
        case i::i_conv_u2:  s << ": conv_u2"; break;
        case i::i_conv_u4:  s << ": conv_u4"; break;
        case i::i_conv_u8:  s << ": conv_u8"; break;
        case i::i_conv_ovf_i:   s << ": conv_ovf_i"; break;
        case i::i_conv_ovf_i1:  s << ": conv_ovf_i1"; break;
        case i::i_conv_ovf_i2:  s << ": conv_ovf_i2"; break;
        case i::i_conv_ovf_i4:  s << ": conv_ovf_i4"; break;
        case i::i_conv_ovf_i8:  s << ": conv_ovf_i8"; break;
        case i::i_conv_ovf_u:   s << ": conv_ovf_u"; break;
        case i::i_conv_ovf_u1:  s << ": conv_ovf_u1"; break;
        case i::i_conv_ovf_u2:  s << ": conv_ovf_u2"; break;
        case i::i_conv_ovf_u4:  s << ": conv_ovf_u4"; break;
        case i::i_conv_ovf_u8:  s << ": conv_ovf_u8"; break;
        case i::i_conv_ovf_i_un:   s << ": conv_ovf_i_un"; break;
        case i::i_conv_ovf_i1_un:  s << ": conv_ovf_i1_un"; break;
        case i::i_conv_ovf_i2_un:  s << ": conv_ovf_i2_un"; break;
        case i::i_conv_ovf_i4_un:  s << ": conv_ovf_i4_un"; break;
        case i::i_conv_ovf_i8_un:  s << ": conv_ovf_i8_un"; break;
        case i::i_conv_r4:  s << ": conv_r4"; break;
        case i::i_conv_r8:  s << ": conv_r8"; break;
        case i::i_conv_r_un:  s << ": conv_r_un"; break;

        // Condition checking operations
        case i::i_ceq: s << ": ceq"; break;
        case i::i_cgt: s << ": cgt"; break;
        case i::i_clt: s << ": clt"; break;
        case i::i_cgt_un: s << ": cgt_un"; break;
        case i::i_clt_un: s << ": clt_un"; break;

        // Special opcodes, which are doing literally nothing
        case i::i_nop:  s << ": nop"; break;
        case i::i_break:  s << ": break"; break;

        // Exception throwing and handling
        case i::i_throw:  s << ": throw"; break;
        case i::i_rethrow: s << ": rethrow"; break;
        case i::i_endfinally: s << ": endfinally"; break;
        case i::i_endfilter: s << ": endfilter"; break;

        // Memory operations
        case i::i_cpblk: s << ": cpblk"; break;
        case i::i_initblk: s << ": initblk"; break;
        case i::i_localloc: s << ": localloc"; break;

        // Misc
        case i::i_dup:    s << ": dup"; break;
        case i::i_pop:    s << ": pop"; break;
        case i::i_ret:    s << ": ret"; break;
        case i::i_arglist: s << ": arglist"; break;
        case i::i_ldnull: s << ": ldnull"; break;
        case i::i_refanytype: s << ": refanytype"; break;

        // Some object model instructions
        // <instruction> [uint32_t Token]
        case i::i_box: 
        case i::i_unbox:
        case i::i_unbox_any:
        case i::i_cpobj:
        case i::i_ldobj:
        case i::i_stobj:
        case i::i_isinst:
        case i::i_ldfld:
        case i::i_stfld:
        case i::i_stsfld:
        case i::i_ldflda:
        case i::i_ldsfld:
        case i::i_ldsflda:
        case i::i_ldstr:
        case i::i_ldtoken:
        case i::i_mkrefany:
        case i::i_newarr:
        case i::i_newobj:
        case i::i_refanyval:
        case i::i_ldelem:
        case i::i_ldelema:
        case i::i_stelem:
        case i::i_castclass:
        case i::i_jmp:
        case i::i_call:
        case i::i_calli:
        case i::i_callvirt:
        case i::i_sizeof:
        case i::i_ldftn:
        case i::i_ldvirtftn:
        case i::i_initobj:
        {
            switch (instr) {
            case i::i_box: s << ": box"; break;
                case i::i_unbox: s << ": unbox"; break;
                case i::i_unbox_any: s << ": unbox_any"; break;
                case i::i_cpobj: s << ": cpobj"; break;
                case i::i_ldobj: s << ": ldobj"; break;
                case i::i_stobj: s << ": stobj"; break;
                case i::i_isinst: s << ": isinst"; break;
                case i::i_ldfld: s << ": ldfld"; break;
                case i::i_stfld: s << ": stfld"; break;
                case i::i_stsfld: s << ": stsfld"; break;
                case i::i_ldflda: s << ": ldflda"; break;
                case i::i_ldsfld: s << ": ldsfld"; break;
                case i::i_ldsflda: s << ": ldsflda"; break;
                case i::i_ldstr: s << ": ldstr"; break;
                case i::i_ldtoken: s << ": ldtoken"; break;
                case i::i_mkrefany: s << ": mkrefany"; break;
                case i::i_newarr: s << ": newarr"; break;
                case i::i_newobj: s << ": newobj"; break;
                case i::i_refanyval: s << ": refanyval"; break;
                case i::i_ldelem: s << ": ldelem"; break;
                case i::i_ldelema: s << ": ldelema"; break;
                case i::i_stelem: s << ": stelem"; break;
                case i::i_castclass: s << ": castclass"; break;
                case i::i_jmp: s << ": jmp"; break;
                case i::i_call: s << ": call"; break;
                case i::i_calli: s << ": calli"; break;
                case i::i_callvirt: s << ": callvirt"; break;
                case i::i_sizeof: s << ": sizeof"; break;
                case i::i_ldftn: s << ": ldftn"; break;
                case i::i_ldvirtftn: s << ": ldvirtftn"; break;
                case i::i_initobj: s << ": initobj"; break;
                default:
                    throw runtime_error("We shouldn't be here.");
            }

            s << " <" << args.begin()->get<uint32_t>() << ">";
        }
        break;

        default: {}
        }

        s << endl;
    }

    return s.str();
}
