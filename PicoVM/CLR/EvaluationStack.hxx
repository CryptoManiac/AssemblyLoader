#ifndef __EVALUATION_STACK_ITEM_HXX__
#define __EVALUATION_STACK_ITEM_HXX__

#include <vector>
#include <cstdint>
#include <cstddef>

struct EvaluationStack {
    std::vector<size_t> data;

    EvaluationStack(uint32_t nStackSize=1048576);

    void push_int8(int8_t value);
    void push_int16(int16_t value);
    void push_int32(int32_t value);
    void push_int64(int64_t value);
    void push_nint(ptrdiff_t value);
    void push_ref(size_t value);

    void push_float32(float value);
    void push_float64(double value);

    int8_t pop_int8();
    int16_t pop_int16();
    int32_t pop_int32();
    int64_t pop_int64();
    ptrdiff_t pop_nint();
    size_t pop_ref();

    float pop_float32();
    double pop_float64();

    void pop();
    void dup();
};

#endif
