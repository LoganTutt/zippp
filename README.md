# zippp
Type-safe Python style zip function in C++17, with added support for random access and bi-directional iterators.

![ci badge gcc](https://github.com/LoganTutt/zippp/workflows/linux-gcc/badge.svg)
![ci badge clang](https://github.com/LoganTutt/zippp/workflows/linux-clang/badge.svg)
![ci badge windows](https://github.com/LoganTutt/zippp/workflows/windows/badge.svg)

## Features
* Type-safe creation of merged lists from an arbitrary number of input lists
* Support for bidirectional motion (if all zipped collections support it)
* Support for random access (if all zipped collections support it)
* Returned iterator can be easily mapped into structured bindings
* Expected behaviour when used with bindings of references, copies, and const/non-const
* Support for temporary (rvalue) collections
* Support for all iterables that work with `std::begin()` and `std::end()`, including `std::array`, `std::vector<bool>`, and C-arrays

## Use

Simply pass the desired collections into the `zippp::zip()` function to create a wrapper collection. 
This can then be iterated over like any other collection. All collections must be the same length. This is not checked.
```cpp

std::vector<int> v1{1,2,3};
std::list<double> v2{2,4,6};
std::array<std:string, 3> v3{"3","6","9"};

for(const auto& [ i1, i2, i3] : zippp::zip(v1, v2, v3))
{
    std::cout << i1 << "+" << i2 << "=" << i3 << std::endl;
}
```

The returned collection from `zippp:zip()` is a wrapper, and does not duplicate any data contained in the passed
collections. It maintains a reference to the input containers and expects them to outlive itself. If any of the collections
are `rvalues` then they will be moved into the returned collections and will be maintained.

The collection can also be used directly like a normal collection.
```cpp
std::vector<int> v1{1, 2, 3};
std::array<std:string, 3> v2{"3", "6", "9"};

auto zipped = zippp::zip(v1, v2);
auto itr = zipped.begin();
auto [l1, l2] = *itr;
```

Dereferencing the `zip_iterator` for the collection returns a custom tuple-like wrapper that can be accessed using a structed
binding. It is not suggested to explicitly store this wrapper except directly into a structured binding as the behaviour is dependent on
its value category. The wrapper will also be invalidated when the iterator is incremented (but any variables/references created via a
structured binding will not be).

### References and Copies

Both copy and reference structured bindings are supported, in both const and non-const forms. They behave as one
would expect on first glance, the const and reference qualifers applied to the bound object are propogated to the binding
labels themselves. **NOTE** If any of the zipped collections are `const` then all will be considered const when bound and mutable
references will not be allowed.

```cpp
std::vector<int> v1{1, 2, 3};
std::array<std:string, 3> v2{"3", "6", "9"};
auto zipped = zippp::zip(v1, v2);
auto itr = zipped.begin();

auto [i1, s1] = *itr;        // i1 and s1 are copies of the first elements of v1 and v2
auto& [i2, s2] = *itr;       // i2 and s2 are mutable references to the first elements of v1 and v2
auto&& [i3, s3] = *itr;      // i3 and s3 are mutable references to the first elements of v1 and v2
const auto [i4, s4] = *itr;  // i4 and s4 are const copies to the first elements of v1 and v2
const auto& [i5, s5] = *itr; // i5 and s5 are const references to the first elements of v1 and v2
```

### Iterator Lifetimes
The iterators of the `zip_collection` have a lifetime that is the same as all the iterators contained inside it. 
This means resizing or reallocating the collections can be done if it would not invalidate the existing iterators at
the current `zip_iterator` position. Be very careful when doing this, as if the collections are not all the same length
when an iterator is incrementd then undefined behavoiur is likely to be invoked. 

Similarly, all structured bindings that are references will be invalidated if the data they point to is no longer valid.
Any bindings created as value copies will still be valid.
