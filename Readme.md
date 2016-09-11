# Streams++ #

Streams++ is a WIP C++14 library inspired by Rust-lang Iterator trait and Java 8 Streams API.
A stream represents a sequence of elements that come from some source. The stream itselt 
and most operations upon its elements are lazy, so nothing is evaluated until it has to be done. 
This allows to perform computation in a functional style by chaining operations stream supports.

## Installation ##
Streams++ is a header only library, so just include `Streams.h`. It has no external dependencies, but 
uses `std::optional<T>` type from  `<experimental/optional>`.

If you are using Visual C++ it's almost sure there is no `<experimental/optional>` header. In that 
case the library relies on [akrzemi1/Optional library](https://github.com/akrzemi1/Optional). You'll
need extra steps after pulling this repository:
```
git submodule init
git submodule update
```

## Examples ##
#### Creating a stream from a collection ####
```c++
std::vector<int> vec {/*...*/};
auto stream = streams::from(vec);
```
#### Sum of squares of multiples of `17` ####
```c++
int sum = streams::from(vec) // you can use a 'stream' created before
    .filter([](auto& e) {return e % 17 == 0; })
    .map([](auto& e) { return e*e; })
    .fold(0, std::plus<int>{});
```
#### First 5 roman numbers with more than 10 'digits' ####
```c++
std::string to_roman(size_t i);

streams::generate::counter(1)
    .map(to_roman)
    .enumerate()
    .filter([](auto& e) {return e.v.length() > 10; })
    .take(5)
    .forEach([](auto& e) { std::cout << e.i + 1 << " == " << e.v << std::endl; });
```
If you are curious the result is:
```
388 == CCCLXXXVIII
788 == DCCLXXXVIII
838 == DCCCXXXVIII
878 == DCCCLXXVIII
883 == DCCCLXXXIII
```

## Under the hood ##
Streams are designed to be fast and lightweight proxy objects. Streams:
- doesn't own the underlying collection; 
- doesn't modify the underlying collection; 
- doesn't allocate memory on the heap;
- never throws exceptions unless it's thrown from inside user code;
- are valid to copy, though the state will also be copied.

As a proxy object, stream should never outlive its source. 

Stream is a single-use object. It can't be reset or used again after its source is depleted.
Actually, using a depleted stream is a valid operation, but the stream is always empty, once it had
no element to extract.

### Tests ###
You'll need [googletest](https://github.com/google/googletest/blob/master/googletest/).
Compile and run tests/general.cpp.
