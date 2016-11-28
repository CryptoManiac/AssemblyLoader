#ifndef __EVALUATION_STACK_ITEM_HXX__
#define __EVALUATION_STACK_ITEM_HXX__

#include <vector>
#include <cstdint>

struct EvaluationStack {
    std::vector<std::size_t> data;

    EvaluationStack(uint32_t nStackSize=1048576);

    void push_int8(int8_t value);
    void push_int16(int16_t value);
    void push_int32(int32_t value);
    void push_int64(int64_t value);
    void push_float32(float value);
    void push_float64(double value);

    void push_nint(std::ptrdiff_t value);
    void push_nfloat(double value);
    void push_ref(std::size_t value);
};

#endif
