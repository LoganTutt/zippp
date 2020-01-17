#ifndef ZIPPP
#define ZIPPP
#include <type_traits>
#include <tuple>

namespace zippp
{
namespace detail
{
template<typename Tuple, typename Func, std::size_t ... Ind>
constexpr void foreach_tuple_impl(Tuple&& tup, Func&& func, std::integer_sequence<std::size_t, Ind...>)
{
    using ignore = int[];
    static_cast<void>(ignore{1, (func(std::get<Ind>(std::forward<Tuple>(tup))), void(), int{})...});
}

template<typename Tuple, typename Func>
constexpr void foreach_tuple(Tuple&& tup, Func&& func)
{
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    foreach_tuple_impl(std::forward<Tuple>(tup), std::forward<Func>(func), std::make_index_sequence<N>{});
}

template<typename ... Iters>
class zip_iterator
{
public:
    zip_iterator(Iters&&... iters) : iter_tup(std::forward<Iters>(iters)...){}

    auto operator++()
    {
        foreach_tuple(iter_tup, [](auto&& iter){++iter;});
        return *this;
    }
    auto operator*()
    {
        return std::apply([](auto&& ... iters){return std::make_tuple(*iters...);}, iter_tup);
    }
    bool operator!=(zip_iterator<Iters...> in) const
    {
        return iter_tup != in.iter_tup;
    }

private:
    std::tuple<Iters...> iter_tup;
};


template<typename ... Collections>
class zip_collection
{
public:
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