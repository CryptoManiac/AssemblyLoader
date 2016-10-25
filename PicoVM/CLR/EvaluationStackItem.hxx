#ifndef __EVALUATION_STACK_ITEM_HXX__
#define __EVALUATION_STACK_ITEM_HXX__
#include <deque>
#include <mapbox/variant.hpp>

typedef mapbox::util::variant<int32_t, int64_t, size_t, double, std::vector<uint16_t> > EvaluationStackItem;

#endif
