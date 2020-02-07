#ifndef ZIPPP
#define ZIPPP
#include <type_traits>
#include <tuple>

namespace zippp
{
namespace detail
{
template<typename Tuple, typename Func, std::size_t ... Ind>
constexpr inline void foreach_tuple_impl(Tuple&& tup, Func&& func, std::integer_sequence<std::size_t, Ind...>)
{
    using ignore = int[];
    static_cast<void>(ignore{1, (func(std::get<Ind>(std::forward<Tuple>(tup))), void(), int{})...});
}

template<typename Tuple, typename Func>
constexpr inline void foreach_tuple(Tuple&& tup, Func&& func)
{
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    foreach_tuple_impl(std::forward<Tuple>(tup), std::forward<Func>(func), std::make_index_sequence<N>{});
}

template<typename Base, typename ... Types>
constexpr inline bool all_are_type()
{
    return std::conjunction<std::is_base_of<Base, Types>...>::value;
}

template<typename Target, typename ... Iters>
constexpr inline bool all_iter_are_type()
{
    return all_are_type<Target, typename std::iterator_traits<Iters>::iterator_category...>();
}

template<typename ... Iters>
using iterator_tag_type = std::conditional_t<all_iter_are_type<std::random_access_iterator_tag, Iters...>(),
    std::random_access_iterator_tag, 
    std::conditional_t<all_iter_are_type<std::bidirectional_iterator_tag, Iters...>(),
        std::bidirectional_iterator_tag,
        std::conditional_t<all_iter_are_type<std::forward_iterator_tag, Iters...>(),
            std::forward_iterator_tag,
            std::input_iterator_tag>>>;

template<typename ... Iters>
class zip_iterator //: public std::iterator<iterator_tag_type,T,ptrdiff_t,const T*,const T&>
{
public:
    using iterator_category = iterator_tag_type<Iters...>;
    using difference_type = std::ptrdiff_t;
private:
    template<typename Tag>
    static constexpr inline bool is_tag = std::is_base_of_v<Tag, iterator_category>;

    // This was originally just inline with X = part of the SFINAE for the member functions, 
    // but a compiler bug in GCC 9.2 caused a segfault.
    // Moving it to this using directive fixes the bug, and it compiles on MSVC, GCC 9.2, anmd GCC 7.
    template<typename T>
    using IterEnabler = std::enable_if_t<is_tag<T>>;

public:
    zip_iterator(Iters&&... iters) : iter_tup(std::forward<Iters>(iters)...){}

    decltype(auto) operator++()
    {
        foreach_tuple(iter_tup, [](auto&& iter){++iter;});
        return *this;
    }
    auto operator++(int)
    {
        auto temp = *this;
        foreach_tuple(iter_tup, [](auto&& iter){++iter;});
        return temp;
    }

    auto operator*()
    {
        return std::apply([](auto&& ... iters){return std::make_tuple<decltype(*iters)...>(*iters...);}, iter_tup);
    }
    bool operator!=(zip_iterator<Iters...> in) const
    {
        return iter_tup != in.iter_tup;
    }
    bool operator==(zip_iterator<Iters...> in) const
    {
        return iter_tup == in.iter_tup;
    }

    // Bidirectional operations
    template<typename T = std::bidirectional_iterator_tag, typename X = IterEnabler<T>>
    decltype(auto) operator--()
    {
        foreach_tuple(iter_tup, [](auto&& iter){--iter;});
        return *this;
    }
    template<typename T = std::bidirectional_iterator_tag, typename X = IterEnabler<T>>
    auto operator--(int)
    {
        auto temp = *this;
        foreach_tuple(iter_tup, [](auto&& iter){--iter;});
        return temp;
    }

    //random access operations
    template<typename T = std::random_access_iterator_tag, typename X = IterEnabler<T>>
    decltype(auto) operator+=(ptrdiff_t i)
    {
        foreach_tuple(iter_tup, [i](auto&& iter){iter+=i;});
        return *this;
    }
    template<typename T = std::random_access_iterator_tag, typename X = IterEnabler<T>>
    auto operator+(ptrdiff_t i) const
    {
        auto temp = *this;
        temp += i;
        return temp;
    }
    template<typename T = std::random_access_iterator_tag, typename X = IterEnabler<T>>
    decltype(auto) operator-=(ptrdiff_t i)
    {
        foreach_tuple(iter_tup, [i](auto&& iter){iter-=i;});
        return *this;
    }
    template<typename T = std::random_access_iterator_tag, typename X = IterEnabler<T>>
    auto operator-(ptrdiff_t i) const
    {
        auto temp = *this;
        temp -= i;
        return temp;
    }

private:
    std::tuple<Iters...> iter_tup;
};


template<typename ... Collections>
class zip_collection
{
public:
    using iterator = zip_iterator<decltype(std::begin(std::declval<Collections>()))...>;

    zip_collection(Collections&& ... in_cols) : col_tup(std::forward<Collections>(in_cols)...){}

    decltype(auto) begin()
    {
        return std::apply([](auto&&... cols){return zip_iterator(std::begin(cols)...);}, col_tup);
    }

    decltype(auto) end()
    {
        return std::apply([](auto&&... cols){return zip_iterator(std::end(cols)...);}, col_tup);
    }

private:
    std::tuple<Collections&...> col_tup;
};
}

template<typename ... Collections>
auto zip(Collections&& ... cols)
{
    return detail::zip_collection<Collections...>(std::forward<Collections>(cols)...);
}
}
#endif