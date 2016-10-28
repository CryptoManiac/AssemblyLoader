#ifndef __EVALUATION_STACK_ITEM_HXX__
#define __EVALUATION_STACK_ITEM_HXX__
#include <mapbox/variant.hpp>
#include <string>

typedef mapbox::util::variant<int32_t, int64_t, size_t, double, std::u16string > EvaluationStackItem;

#endif
