#ifndef ZIPPP
#define ZIPPP
#include <type_traits>
#include <tuple>

namespace zippp
{
namespace detail
{

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

template<typename index_seq, typename ... Iters>
class zip_iterator;

/**
 * @brief Small class contianing the iterators in the collections
 * 
 * This class handles accessing the actual vaules when bound to a structured binding.
 * Copies or references of the values is handled based on the value category of this object when it is bound.
 */
template<typename ... Iters>
class zip_iter_value {
private:
    using tuple_type = std::tuple<Iters...>;
    /// Tupe containing the iterators pointing to each collection
    tuple_type iters;

    template<typename seq, typename ... T>
    friend class zip_iterator;

public:
    template<std::size_t I>
    using element_type = decltype((*std::declval<typename std::tuple_element<I, tuple_type>::type>()));

    zip_iter_value(Iters&&... iters_) : iters(std::forward<Iters>(iters_)...) {}

    /// Return a reference to the underlying value if this object is an lvalue.
    /// This is true whenever the binding is any type of reference.
    template<std::size_t I>
    auto constexpr get() const &  -> element_type<I>
    { 
        return *std::get<I>(iters);
    }

    /// Copy the value only if this object is an rvalue.
    /// This happens when the binding is not any form of reference.
    template<std::size_t I>
    auto constexpr get() const && -> typename std::decay<element_type<I>>::type
    {
        return *std::get<I>(iters);
    }
};

/**
 * @brief The iterator for a zipped set of collections
 * 
 * Bidirectional (--) and random access (+=, -=, etc) operators are enabled when all internal iterators have the 
 * correct operations as well.
 */
template<std::size_t ... Ind, typename ... Iters>
class zip_iterator<std::index_sequence<Ind...>, Iters...>: 
    public std::iterator<iterator_tag_type<Iters...>, zip_iter_value<Iters...>>
{
public:
    // Iterator member types forwarded for convenience
    using Base = std::iterator<iterator_tag_type<Iters...>, zip_iter_value<Iters...>>;
    using iterator_category = typename Base::iterator_category;
    using value_type = typename Base::value_type;
    using difference_type = typename Base::difference_type;
    using pointer = typename Base::pointer;
    using reference = typename Base::reference;

private:
    template<typename Tag>
    static constexpr inline bool is_tag = std::is_base_of_v<Tag, iterator_category>;

    // This was originally just inline with X = part of the SFINAE for the member functions, 
    // but a compiler bug in GCC 9.2 caused a segfault.
    // Moving it to this using directive fixes the bug, and it compiles on MSVC, GCC 9.2, anmd GCC 7.
    template<typename T>
    using IterEnabler = std::enable_if_t<is_tag<T>>;

public:
    zip_iterator(Iters&&... iters) : iter_values(std::forward<Iters>(iters)...) {}
    ~zip_iterator() = default;

    // Increment operators
    decltype(auto) operator++()
    {
        ((void)++std::get<Ind>(iter_values.iters), ...);
        return *this;
    }
    auto operator++(int)
    {
        auto temp = *this;
        ++*this;
        return temp;
    }

    // Dereference operators
    reference operator*()
    {
        return iter_values;
    }

    // Comparison operators
    bool operator==(zip_iterator<std::index_sequence<Ind...>, Iters...> in) const
    {
        return std::get<0>(iter_values.iters) == std::get<0>(in.iter_values.iters);
    }
    bool operator!=(zip_iterator<std::index_sequence<Ind...>, Iters...> in) const
    {
        return !(*this == in);
    }

    // Bidirectional operations
    template<typename T = std::bidirectional_iterator_tag, typename X = IterEnabler<T>>
    decltype(auto) operator--()
    {
        ((void)--std::get<Ind>(iter_values.iters), ...);
        return *this;
    }
    template<typename T = std::bidirectional_iterator_tag, typename X = IterEnabler<T>>
    auto operator--(int)
    {
        auto temp = *this;
        --*this;
        return temp;
    }

    // Random Access operations
    template<typename T = std::random_access_iterator_tag, typename X = IterEnabler<T>>
    decltype(auto) operator+=(ptrdiff_t i)
    {
        (((void)(std::get<Ind>(iter_values.iters) += i)), ...);
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
        (((void)(std::get<Ind>(iter_values.iters) -= i)), ...);
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
    // The actual iterators are stored inside this object
    // Keep the values here so that we can return lvalue references to it
    zip_iter_value<Iters...> iter_values;

};

template<typename ... Collections>
class zip_collection
{
public:
    using iterator = zip_iterator<std::make_index_sequence<sizeof...(Collections)>, 
                                  decltype(std::begin(std::declval<Collections>()))...>;
    // If a list is an rvalue we will move it into our tuple and keep an actual list stored
    // Otherwise we keep a reference
    using tuple_type = std::tuple<std::conditional_t<std::is_rvalue_reference<Collections>::value,
                                  std::decay_t<Collections>, Collections>...>;

    zip_collection(Collections&& ... in_cols) : col_tup(std::forward<Collections>(in_cols)...){}

    decltype(auto) begin()
    {
        return std::apply([](auto&&... cols){return iterator(std::begin(cols)...);}, col_tup);
    }

    decltype(auto) end()
    {
        return std::apply([](auto&&... cols){return iterator(std::end(cols)...);}, col_tup);
    }

private:
    tuple_type col_tup;
};
} // namespace detail

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
} // namespace zippp

// Template specializations for zip_iter_value to let it be bound by structured bindings
namespace std {
    template<typename ... Iters>
    struct tuple_size<zippp::detail::zip_iter_value<Iters...>> : std::integral_constant<size_t, sizeof...(Iters)> {};

    template<std::size_t I, typename ... Iters>
    struct tuple_element<I, zippp::detail::zip_iter_value<Iters...>>
    {
        using type = typename std::decay_t<typename zippp::detail::zip_iter_value<Iters...>::template element_type<I>>;
    };
} // namespace std
#endif