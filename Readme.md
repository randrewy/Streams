[![Build Status](https://travis-ci.org/randrewy/Streams.svg?branch=master)](https://travis-ci.org/randrewy/Streams)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/randrewy/Streams/master/License.md)
[![Coverage Status](https://coveralls.io/repos/github/randrewy/Streams/badge.svg?branch=master)](https://coveralls.io/github/randrewy/Streams?branch=master)
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
You can use cmake to build tests and examples alltogeather.

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
    .fold(0, std::plus<>{});
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
Streams are designed to be fast and lightweight proxy objects. Every stream is a different 
class with statically dispatched methods. More than that, a stream
- doesn't own the underlying collection; 
- doesn't modify the underlying collection; 
- doesn't allocate memory on the heap;
- never throws exceptions unless it's thrown from inside user code;
- is valid to copy, though the state will also be copied.

As a proxy object, stream should never outlive its source. 

Stream is a single-use object. It can't be reset or used again after its source is depleted.
Actually, using an exhausted stream is a valid operation, but the stream will remain empty forever.

### Tests ###
You'll need [googletest](https://github.com/google/googletest/blob/master/googletest/).
Simply run
```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ctest -VV
```
or use generated project file on Windows.
