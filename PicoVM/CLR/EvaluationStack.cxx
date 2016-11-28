#include "EvaluationStack.hxx"
#include "CLIElementTypes.hxx"
#include "EnumCasting.hxx"
#include "NumCasting.hxx"

#if INTPTR_MAX == INT32_MAX
    #define THIS_IS_32_BIT
#elif INTPTR_MAX == INT64_MAX
    #define THIS_IS_64_BIT
#else
    #error "Unsupported machine"
#endif

using namespace std;

EvaluationStack::EvaluationStack(uint32_t nStackSize) {
    data.reserve(nStackSize / sizeof(size_t));
}

void EvaluationStack::push_int8(int8_t value) {
    data.push_back(value);
    data.push_back(_u(CLIElementType::ELEMENT_TYPE_I1));
}

void EvaluationStack::push_int16(int16_t value) {
    data.push_back(value);
    data.push_back(_u(CLIElementType::ELEMENT_TYPE_I2));
}

void EvaluationStack::push_int32(int32_t value) {
    data.push_back(value);
    data.push_back(_u(CLIElementType::ELEMENT_TYPE_I4));
}

void EvaluationStack::push_int64(int64_t value) {
#ifdef THIS_IS_32_BIT
    data.push_back(value);
    data.push_back(value >> 32);
#else
    data.push_back(value);
#endif

    data.push_back(_u(CLIElementType::ELEMENT_TYPE_I8));
}

void EvaluationStack::push_float32(float value) {
    data.push_back(floatToInt(value));
    data.push_back(_u(CLIElementType::ELEMENT_TYPE_R4));
}

void EvaluationStack::push_float64(double value) {
#ifdef THIS_IS_32_BIT
    int64_t v = doubleToLong(value);
    data.push_back(v);
    data.push_back(v >> 32);
#else
    int64_t v = doubleToLong(value);
    data.push_back(v);
#endif

    data.push_back(_u(CLIElementType::ELEMENT_TYPE_R8));
}

void EvaluationStack::push_nint(ptrdiff_t value) {
    data.push_back(value);
    data.push_back(_u(CLIElementType::ELEMENT_TYPE_I));
}

void EvaluationStack::push_ref(size_t value) {
    data.push_back(value);
    data.push_back(_u(CLIElementType::ELEMENT_TYPE_U));
}
