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
class zip_iter_value_view {
private:
    using tuple_type = std::tuple<Iters...>;
    const tuple_type * iters;
public:
    zip_iter_value_view(const tuple_type& iters_) : iters(&iters_) {}

    // Return a reference is the object is an lvalue
    // This is true whenever the binding is any type of reference
    template<std::size_t I>
    auto constexpr get() const &  -> const decltype((*std::get<I>(*iters))) {
        return *std::get<I>(*iters);
    }
    template<std::size_t I>
    auto constexpr get() & -> decltype((*std::get<I>(*iters))) {
        return *std::get<I>(*iters);
    }

    // Copy only if the object is an rvalue.
    // This happens when the binding is not any form of reference.
    template<std::size_t I>
    auto constexpr get() const && -> typename std::decay<decltype(*std::get<I>(*iters))>::type {
        return *std::get<I>(*iters);
    }
};

template<typename ... Iters>
class zip_iterator 
    //TODO: : public std::iterator<iterator_tag_type,T,ptrdiff_t,const T*,const T&>
{
public:
    using iterator_category = iterator_tag_type<Iters...>;
    using difference_type = std::ptrdiff_t;
    //TODO: add other itterator traits
private:
    template<typename Tag>
    static constexpr inline bool is_tag = std::is_base_of_v<Tag, iterator_category>;

    // This was originally just inline with X = part of the SFINAE for the member functions, 
    // but a compiler bug in GCC 9.2 caused a segfault.
    // Moving it to this using directive fixes the bug, and it compiles on MSVC, GCC 9.2, anmd GCC 7.
    template<typename T>
    using IterEnabler = std::enable_if_t<is_tag<T>>;

public:
    zip_iterator(Iters&&... iters) : iter_tup(std::forward<Iters>(iters)...), iter_view(iter_tup) {}
    ~zip_iterator() = default;

    // Copy and move operators
    zip_iterator(const zip_iterator& in) : iter_tup(in.iter_tup), iter_view(iter_tup) {}
    zip_iterator(zip_iterator&& in) : iter_tup(std::move(in.iter_tup)), iter_view(iter_tup) {}
    zip_iterator& operator=(zip_iterator in) {
        std::swap(in.iter_tup, iter_tup);
        iter_view = zip_iter_value_view(iter_tup);
    }

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

    auto& operator*()
    {
        return iter_view;
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
    // Keep the value view here so that we can return lvalue references to it
    zip_iter_value_view<Iters...> iter_view;

};

template<typename ... Collections>
class zip_collection
{
public:
    using iterator = zip_iterator<decltype(std::begin(std::declval<Collections>()))...>;
    // If a list is an rvalue we will move it into our tuple and keep an actual list stored
    // Otherwise we keep a reference
    using tuple_type = std::tuple<std::conditional_t<std::is_rvalue_reference<Collections>::value,
                                  std::decay_t<Collections>, Collections>...>;

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
    tuple_type col_tup;
};
}

/**
 * @brief Creates a wrapper collection of the provided collections that will iterate through all at once
 * 
 * 
 * @param collections Collections that should be zipped together
 * 
 * All collections must be of the same length. They must also remain valid for as long as the returned object is valid
 * @return A zip_collection containing copies to each of the provided collections
 */
template<typename ... Collections>
auto zip(Collections&& ... collections)
{
    return detail::zip_collection<Collections...>(std::forward<Collections>(collections)...);
}
}

// Template specializations for zip_iter_value_view to let it be bound by structured bindings
namespace std {
    template<typename ... Iters>
    struct tuple_size<zippp::detail::zip_iter_value_view<Iters...>> : std::integral_constant<size_t, sizeof...(Iters)> { };

    template<std::size_t I, typename ... Iters>
    struct tuple_element<I, zippp::detail::zip_iter_value_view<Iters...>> {
        using type = typename std::decay<decltype(std::declval<zippp::detail::zip_iter_value_view<Iters...>>().template get<I>())>::type;
    };
}
#endif