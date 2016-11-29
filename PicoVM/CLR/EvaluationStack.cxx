#include "EvaluationStack.hxx"
#include "CLIElementTypes.hxx"
#include "EnumCasting.hxx"
#include "NumCasting.hxx"

#include <cassert>

#if INTPTR_MAX == INT32_MAX
    #define THIS_IS_32_BIT
#elif INTPTR_MAX == INT64_MAX
    #define THIS_IS_64_BIT
#else
    #error "Unsupported machine"
#endif

using namespace std;

inline static size_t _pop_sz(vector<size_t>& data) {
    size_t value = data.back();
    data.pop_back();
    return value;
}

#define push_sz(v) data.push_back(v)
#define pop_sz() _pop_sz(data)

EvaluationStack::EvaluationStack(uint32_t nStackSize) {
    data.reserve(nStackSize / sizeof(size_t));
}

void EvaluationStack::push_int8(int8_t value) {
    push_sz(value);
    push_sz(_u(CLIElementType::ELEMENT_TYPE_I1));
}

void EvaluationStack::push_int16(int16_t value) {
    push_sz(value);
    push_sz(_u(CLIElementType::ELEMENT_TYPE_I2));
}

void EvaluationStack::push_int32(int32_t value) {
    push_sz(value);
    push_sz(_u(CLIElementType::ELEMENT_TYPE_I4));
}

void EvaluationStack::push_int64(int64_t value) {
#ifdef THIS_IS_32_BIT
    push_sz(value & 0xffffffff);
    push_sz(value >> 32);
#else
    push_sz(value);
#endif

    push_sz(_u(CLIElementType::ELEMENT_TYPE_I8));
}

void EvaluationStack::push_float32(float value) {
    push_sz(floatToUInt(value));
    push_sz(_u(CLIElementType::ELEMENT_TYPE_R4));
}

void EvaluationStack::push_float64(double value) {
    uint64_t v = doubleToULong(value);
#ifdef THIS_IS_32_BIT
    push_sz(v & 0xffffffff);
    push_sz(v >> 32);
#else
    push_sz(v);
#endif
    push_sz(_u(CLIElementType::ELEMENT_TYPE_R8));
}

void EvaluationStack::push_nint(ptrdiff_t value) {
    push_sz(value);
    push_sz(_u(CLIElementType::ELEMENT_TYPE_I));
}

void EvaluationStack::push_ref(size_t value) {
    push_sz(value);
    push_sz(_u(CLIElementType::ELEMENT_TYPE_U));
}

int8_t EvaluationStack::pop_int8() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_I1));
    return static_cast<int8_t>(pop_sz());
}

int16_t EvaluationStack::pop_int16() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_I2));
    return static_cast<int16_t>(pop_sz());
}

int32_t EvaluationStack::pop_int32() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_I4));
    return static_cast<int32_t>(pop_sz());
}

int64_t EvaluationStack::pop_int64() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_I8));

    uint64_t value;
#ifdef THIS_IS_32_BIT
    value = static_cast<uint64_t>(pop_sz()) << 32;
    value |= static_cast<uint64_t>(pop_sz());
#else
    value = static_cast<int64_t>(pop_sz());
#endif
    return value;
}

ptrdiff_t EvaluationStack::pop_nint() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_I));
    return pop_sz();
}

size_t EvaluationStack::pop_ref() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_U));
    return pop_sz();
}

float EvaluationStack::pop_float32() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_R4));
    auto iv = static_cast<uint32_t>(pop_sz());
    return uintToFloat(iv);
}

double EvaluationStack::pop_float64() {
    assert(pop_sz() == _u(CLIElementType::ELEMENT_TYPE_R8));
    uint64_t lv = 0;
#ifdef THIS_IS_32_BIT
    lv  = static_cast<uint64_t>(pop_sz()) << 32;
    lv |= static_cast<uint64_t>(pop_sz()); 
#else
    lv = pop_sz();
#endif
    return ulongToDouble(lv);
}

void EvaluationStack::dup() {
    using elt = CLIElementType;
    auto type = static_cast<elt>(data.back());

    switch (type) {
        case elt::ELEMENT_TYPE_I1:
        {
            auto v = pop_int8();
            push_int8(v);
            push_int8(v);
        }
        break;
        case elt::ELEMENT_TYPE_I2:
        {
            auto v = pop_int16();
            push_int16(v);
            push_int16(v);
        }
        break;
        case elt::ELEMENT_TYPE_I4:
        {
            auto v = pop_int32();
            push_int32(v);
            push_int32(v);
        }
        break;
        case elt::ELEMENT_TYPE_I8:
        {
            auto v = pop_int64();
            push_int64(v);
            push_int64(v);
        }
        break;

        case elt::ELEMENT_TYPE_I:
        {
            auto v = pop_nint();
            push_nint(v);
            push_nint(v);
        }
        break;
        case elt::ELEMENT_TYPE_U:
        {
            auto v = pop_ref();
            push_ref(v);
            push_ref(v);
        }
        break;

        case elt::ELEMENT_TYPE_R4:
        {
            auto v = pop_float32();
            push_float32(v);
            push_float32(v);
        }
        break;
        case elt::ELEMENT_TYPE_R8:
        {
            auto v = pop_float64();
            push_float64(v);
            push_float64(v);
        }
        break;

        default:
            throw runtime_error("Not yet implemented");
    }
}

void EvaluationStack::pop() {
using elt = CLIElementType;
    auto type = static_cast<elt>(data.back());

    switch (type) {
        case elt::ELEMENT_TYPE_I1:
            pop_int8();
        break;
        case elt::ELEMENT_TYPE_I2:
            pop_int16();
        break;
        case elt::ELEMENT_TYPE_I4:
            pop_int32();
        break;
        case elt::ELEMENT_TYPE_I8:
            pop_int64();
        break;
        case elt::ELEMENT_TYPE_I:
            pop_nint();
        break;
        case elt::ELEMENT_TYPE_U:
            pop_ref();
        break;
        case elt::ELEMENT_TYPE_R4:
            pop_float32();
        break;
        case elt::ELEMENT_TYPE_R8:
            pop_float64();
        break;

        default:
            throw runtime_error("Not yet implemented");
    }}
