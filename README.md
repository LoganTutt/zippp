# zippp
Type-safe Python style zip function in C++17, with added support for random access and bi-directional iterators.

## Features
* Type-safe creation of merged lists from an arbitrary number of input lists
* Support for bidirectional motion (if all zipped collections support it)
* Support for random access (if all zipped collections support it)
* Returned iteraror provides a tuple-like for use with structured bindings
* Support for all iterables that work with `std::begin()` and `std::end()`, including `std::array` and c-arrays

## Use

Simply pass the desired collections into the `zippp::zip()` function to create a wrapper collection. 
This can then be iterated over like any other collection. All collections must be the same length. This is not checked.
```cpp

std::vector<int> v1{1,2,3};
std::list<double> v2{2,4,6};
std::array<std:string, 3> v3{"3","6","9"};

for(const auto& [ i1, i2, i3] : zippp::zip(v1,v2,v3))
{
    std::cout << i1 << "+" << i2 << "=" << i3 << std::endl;
}
```

The returned collection from `zippp:zip()` is a wrapper, and does not duplicate any data contained in the passed
collections. It maintains a reference to the input containers and expects them to outlive itself.

Dereferencing the `zip_iterator` for the collection returns a `std::tuple` containing coppies of the items in the 
collection at that point.

The collection can also be used directly like a normal collection.
```cpp
std::vector<int> v1{1,2,3};
std::array<std:string, 3> v2{"3","6","9"};

auto zipped = zippp::zip(v1, v2);
auto itr = zipped.begin();
auto [l1, l2] = *itr;
```

**NOTE**: The tuple returned by dereferencing an iterator contains copies to the interal items in the collections.
This means that even when using reference structured bindings (like `auto& [...]`) the created bindings are copies, 
not references. This is something that will be improved upon in the future.



### Iterator lifetimes
The iterators of the `zip_collection` have a lifetime that is the same as all the iterators contained inside it. 
This means resizing or reallocating the collections can be done if it would not invalidate the existing iterators at
the current `zip_iterator` position. Be very careful when doing this, as if the collections are not all the same length
when an iterator is incrementd then undefined behavoiur is likely to be invoked. 
