#ifndef __ENUMCASTING_HXX__
#define __ENUMCASTING_HXX__

#include <type_traits> //for std::underlying_type

template<typename E>
constexpr auto _u(E e) -> typename std::underlying_type<E>::type
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

#endif // __ENUMCASTING_HXX__
