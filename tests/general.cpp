#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <list>
#include <iostream>
#include "../Streams.h"
#include "gtest/gtest.h"

class GeneralTests : public ::testing::Test {
    const size_t size = 100;

protected:
    std::vector<int> vector = {};
    void SetUp() {
        vector.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            vector.push_back(static_cast<int>(i));
        }
    }

    auto getStream() {
        return streams::from(vector);
    }
};


TEST_F(GeneralTests, ForEach) {
    std::vector<int> vec;
    getStream().forEach([&vec](auto& v) {vec.push_back(v); });

    ASSERT_EQ(vector, vec);
}

TEST_F(GeneralTests, ForEachOnEmpty) {
    vector.clear();
    std::vector<int> vec;
    getStream().forEach([&vec](auto& v) {vec.push_back(v); });

    ASSERT_EQ(std::vector<int>{}, vec);
}


TEST_F(GeneralTests, Collect) {
    auto vec = getStream().collect<std::vector>();
    ASSERT_EQ(vector, vec);

    auto vec2 = getStream().collect();
    ASSERT_EQ(vector, vec2);
}


TEST_F(GeneralTests, CollectOnEmpty) {
    vector.clear();
    auto vec = getStream().collect();
    ASSERT_EQ(vector, vec);
}

TEST_F(GeneralTests, CollectList) {
    auto vec = getStream().collect<std::list>();

    std::list<int> lst;
    std::copy(vector.begin(), vector.end(), std::back_inserter(lst));
    ASSERT_EQ(lst, vec);
}

TEST_F(GeneralTests, CollectAsOther) {
    auto vec = getStream().collect<std::list, double>();

    std::list<double> lst;
    std::copy(vector.begin(), vector.end(), std::back_inserter(lst));
    ASSERT_EQ(lst, vec);
}


TEST_F(GeneralTests, MapSameType) {
    auto vec = getStream()
        .map([](auto& v) { return v*v; })
        .collect();

    std::transform(vector.begin(), vector.end(), vector.begin(), [](auto& v) {return v*v; });

    ASSERT_EQ(vector, vec);
}

TEST_F(GeneralTests, MapChangeType) {
    auto vec = getStream()
        .map([](auto& v) { return std::to_string(v*v); })
        .collect();

    std::vector<std::string> check;
    std::transform(vector.begin(), vector.end(), std::back_inserter(check), [](auto& v) {return std::to_string(v*v); });

    ASSERT_EQ(check, vec);
}


TEST_F(GeneralTests, FilterSome) {
    auto vec = getStream()
        .filter([](auto& v) { return v != 3 && v != 45 && v != 98; })
        .collect();

    ASSERT_TRUE(std::find(vec.begin(), vec.end(), 3) == vec.end());
    ASSERT_TRUE(std::find(vec.begin(), vec.end(), 45) == vec.end());
    ASSERT_TRUE(std::find(vec.begin(), vec.end(), 98) == vec.end());
}

TEST_F(GeneralTests, FilterAll) {
    auto vec = getStream()
        .filter([](auto&) { return false; })
        .collect();

    ASSERT_EQ(std::vector<int>{}, vec);
}

TEST_F(GeneralTests, FilterNone) {
    auto vec = getStream()
        .filter([](auto&) { return true; })
        .collect();

    ASSERT_EQ(vector, vec);
}


TEST_F(GeneralTests, FilterMap) {
    auto vec = getStream()
        .filterMap([](auto&& e) { return e % 25 == 0 ? streams::Optional<int>(e) : streams::nullopt; })
        .collect();

    std::vector<int> check;
    for (int i : vector) {
        if (i % 25 == 0) {
            check.push_back(i);
        }
    }

    ASSERT_EQ(check, vec);
}


TEST_F(GeneralTests, SkipAll) {
    auto vec = getStream()
        .skip(100)
        .collect();

    ASSERT_EQ(std::vector<int>{}, vec);
}

TEST_F(GeneralTests, SkipNone) {
    auto vec = getStream()
        .skip(0)
        .collect();

    ASSERT_EQ(vector, vec);
}

TEST_F(GeneralTests, SkipSome) {
    auto vec = getStream()
        .skip(3)
        .collect();

    std::vector<int> check;
    auto from = vector.begin();
    std::advance(from, 3);
    std::copy(from, vector.end(), std::back_inserter(check));
    ASSERT_EQ(check, vec);
}


TEST_F(GeneralTests, SkipWhileAll) {
    auto vec = getStream()
        .skipWhile([](auto&) {return true; })
        .collect();

    ASSERT_EQ(std::vector<int>{}, vec);
}

TEST_F(GeneralTests, SkipWhileNone) {
    auto vec = getStream()
        .skipWhile([](auto&) {return false; })
        .collect();

    ASSERT_EQ(vector, vec);
}

TEST_F(GeneralTests, SkipWhileSome) {
    auto vec = getStream()
        .skipWhile([](auto& e) {return e < 7; })
        .collect();

    std::vector<int> check;
    auto it = vector.begin();
    while (it != vector.end() && *it < 7) {
        ++it;
    }
    std::copy(it, vector.end(), std::back_inserter(check));

    ASSERT_EQ(check, vec);
}


TEST_F(GeneralTests, TakeAll) {
    auto vec = getStream()
        .take(vector.size())
        .collect();

    ASSERT_EQ(vector, vec);
}

TEST_F(GeneralTests, TakeNone) {
    auto vec = getStream()
        .take(0)
        .collect();

    ASSERT_EQ(std::vector<int>{}, vec);
}

TEST_F(GeneralTests, TakeSome) {
    size_t n = 5;

    auto vec = getStream()
        .take(n)
        .collect();

    std::vector<int> check;
    for (size_t i = 0; i < n; i++) {
        check.push_back(vector[i]);
    }

    ASSERT_EQ(check, vec);
}


TEST_F(GeneralTests, TakeWhileAll) {
    auto vec = getStream()
        .takeWhile([](auto&) {return true; })
        .collect();

    ASSERT_EQ(vector, vec);
}

TEST_F(GeneralTests, TakeWhileNone) {
    auto vec = getStream()
        .takeWhile([](auto&) {return false; })
        .collect();

    ASSERT_EQ(std::vector<int>{}, vec);
}

TEST_F(GeneralTests, TakeWhileSome) {
    auto vec = getStream()
        .takeWhile([](auto& e) {return e < 10; })
        .collect();

    std::vector<int> check;
    for (int i : vector) {
        if (i >= 10) {
            break;
        }
        check.push_back(i);
    }

    ASSERT_EQ(check, vec);
}


TEST_F(GeneralTests, Next) {
    auto stream = getStream();

    for (int i : vector) {
        auto e = stream.next();
        ASSERT_EQ(true, static_cast<bool>(e));
        ASSERT_EQ(i, *e);
    }
    auto e = stream.next();
    ASSERT_EQ(false, static_cast<bool>(e));
    ASSERT_EQ(streams::nullopt, e);
}


TEST_F(GeneralTests, NthConsumes) {
    auto stream = getStream();
    auto e = stream.nth(0);
    ASSERT_EQ(true, static_cast<bool>(e));
    ASSERT_EQ(vector[0], *e);

    auto e2 = stream.nth(0);
    ASSERT_EQ(true, static_cast<bool>(e2));
    ASSERT_EQ(vector[1], *e2);
}

TEST_F(GeneralTests, NthState) {
    auto stream = getStream();
    auto e = stream.nth(12);
    ASSERT_EQ(true, static_cast<bool>(e));
    ASSERT_EQ(vector[12], *e);

    auto e2 = stream.nth(20);
    ASSERT_EQ(true, static_cast<bool>(e2));
    ASSERT_EQ(vector[33], *e2); // 33! coz 32-th is comsumed
}

TEST_F(GeneralTests, NthNotPresent) {
    auto e2 = getStream().nth(100000);
    ASSERT_EQ(false, static_cast<bool>(e2));
    ASSERT_EQ(streams::nullopt, e2);
}


TEST_F(GeneralTests, Count) {
    ASSERT_EQ(vector.size(), getStream().count());

    std::vector<int> v;
    ASSERT_EQ(0, streams::from(v).count());
}


TEST_F(GeneralTests, AnyResult) {
    ASSERT_EQ(true, getStream().any([](auto& e) { return e > 50; }));
    ASSERT_EQ(false, getStream().any([](auto& e) { return e < 0; }));
}

TEST_F(GeneralTests, AnyState) {
    auto s = getStream();
    ASSERT_EQ(true, s.any([](auto& e) { return e > 50; }));
    ASSERT_EQ(false, s.any([](auto& e) { return e < 50; }));
    ASSERT_EQ(false, s.any([](auto&) { return true; })); // Yes! false coz stream is depleted
}


TEST_F(GeneralTests, AllResult) {
    ASSERT_EQ(true, getStream().all([](auto& e) { return e >= 0; }));
    ASSERT_EQ(false, getStream().all([](auto& e) { return e < 99; }));
}

TEST_F(GeneralTests, AllState) {
    auto s = getStream();
    auto check = [](auto& e) { return e >= 0; };
    ASSERT_EQ(true, s.all(check));
    ASSERT_EQ(true, s.all(check));  // this should be well documented or changed
}


TEST_F(GeneralTests, Fold) {
    int result = std::accumulate(vector.begin(), vector.end(), 0);
    ASSERT_EQ(result, getStream().fold(0, std::plus<int>{}));
}

TEST_F(GeneralTests, FoldNone) {
    std::vector<int> v{};
    ASSERT_EQ(0, streams::from(v).fold(0, std::plus<int>{}));
}


TEST_F(GeneralTests, Inspect) {
    std::vector<int> vec;
    auto s = getStream().inspect([&vec](auto& v) {vec.push_back(v); }); // pretty stupid way to use inspect

    ASSERT_EQ(std::vector<int>{}, vec); // laziness
    s.collect();
    ASSERT_EQ(vector, vec);
}


TEST_F(GeneralTests, InspectNth) {
    std::vector<int> vec;
    auto s = getStream()
        .inspect([&vec](auto& v) {vec.push_back(v); })
        .nth(10);

    ASSERT_EQ(std::vector<int>(vector.begin(), vector.begin() + 11), vec);
    ASSERT_EQ(true, static_cast<bool>(s));
    ASSERT_EQ(10, *s);
}


TEST_F(GeneralTests, Spy) {
    std::vector<int> vec;
    auto s = getStream().spy([&vec](auto& v) {vec.push_back(v); });

    ASSERT_EQ(std::vector<int>{}, vec); // laziness
    s.collect();
    ASSERT_EQ(vector, vec);
}


TEST_F(GeneralTests, SpyNth) {
    std::vector<int> vec;
    auto s = getStream()
        .spy([&vec](auto& v) {vec.push_back(v); })
        .nth(10);

    std::vector<int> result = { (*vector.begin() + 10) };
    ASSERT_EQ(result, vec);
    ASSERT_EQ(true, static_cast<bool>(s));
    ASSERT_EQ(10, *s);
}


TEST_F(GeneralTests, LastSome) {
    auto last = getStream().last();
    ASSERT_EQ(true, static_cast<bool>(last));
    ASSERT_EQ(*(vector.end()-1), *last);
}

TEST_F(GeneralTests, LastNone) {
    std::vector<int> v{};
    auto last = streams::from(v).last();
    ASSERT_EQ(false, static_cast<bool>(last));
}


TEST_F(GeneralTests, Enumerate) {
    auto s = getStream()
        .enumerate()
        .collect();

    std::vector<streams::Enumerated<int>> check{};
    size_t counter = 0;
    std::transform(vector.begin(), vector.end(), std::back_inserter(check), [&counter](auto& v) {
        return streams::Enumerated<int> {counter++, v};
    });
    ASSERT_EQ(check, s);
}


TEST_F(GeneralTests, EnumerateTup) {
    auto s = getStream()
        .enumerateTup()
        .collect();

    std::vector<std::tuple<size_t, int>> check{};
    size_t counter = 0;
    std::transform(vector.begin(), vector.end(), std::back_inserter(check), [&counter](auto& v) {
        return std::tuple<size_t, int> {counter++, v};
    });
    ASSERT_EQ(check, s);
}


TEST_F(GeneralTests, ChainAll) {
    auto s1 = getStream();
    auto s2 = getStream().chain(s1);


    std::vector<int> check{};
    std::copy(vector.begin(), vector.end(), std::back_inserter(check));
    std::copy(vector.begin(), vector.end(), std::back_inserter(check));

    ASSERT_EQ(check, s2.collect());
}

TEST_F(GeneralTests, ChainWithEmpty) {
    std::vector<int> emptyVec {};

    auto s1 = getStream();
    auto s2 = getStream();
    auto empty = streams::from(emptyVec);

    ASSERT_EQ(vector, s1.chain(empty).collect());
    ASSERT_EQ(vector, empty.chain(s2).collect());
}

TEST_F(GeneralTests, ChainRepeated) {
    auto s1 = getStream();
    auto s2 = getStream();
    auto s3 = getStream();

    std::vector<int> check{};
    std::copy(vector.begin(), vector.end(), std::back_inserter(check));
    std::copy(vector.begin(), vector.end(), std::back_inserter(check));
    std::copy(vector.begin(), vector.end(), std::back_inserter(check));

    ASSERT_EQ(check, s1.chain(s2).chain(s3).collect());
}


TEST_F(GeneralTests, Zip) {
    auto s1 = getStream();
    auto s2 = getStream();

    std::vector<std::tuple<int, int>> check;
    for (int i : vector) {
        check.push_back(std::make_tuple(i, i));
    }

    ASSERT_EQ(check, s1.zip(s2).collect());
}

TEST_F(GeneralTests, ZipWithShort) {
    std::vector<int> vec{ 3, 4, 5 };

    auto s1 = getStream();
    auto s2 = getStream();
    auto short1 = streams::from(vec);
    auto short2 = streams::from(vec);

    std::vector<std::tuple<int, int>> check1;
    std::vector<std::tuple<int, int>> check2;
    for (size_t i = 0; i < vec.size(); ++i) {
        check1.push_back(std::make_tuple(vector[i], vec[i]));
        check2.push_back(std::make_tuple(vec[i], vector[i]));
    }

    ASSERT_EQ(check1, s1.zip(short1).collect());
    ASSERT_EQ(check2, short2.zip(s2).collect());
}


TEST_F(GeneralTests, Purify) {
    using streams::nullopt;
    std::vector<streams::Optional<int>> vec1{ 1, nullopt, 3, nullopt, 5, 6, 7, nullopt, nullopt };
    std::vector<streams::Optional<int>> vec2{ nullopt, 1, nullopt, 2, nullopt, 3 };

    auto v1 = streams::from(vec1).purify().collect();
    auto v2 = streams::from(vec2).purify().collect();

    std::vector<int> check1{ 1, 3, 5, 6, 7 };
    std::vector<int> check2{ 1, 2, 3};
    ASSERT_EQ(check1, v1);
    ASSERT_EQ(check2, v2);
}


TEST_F(GeneralTests, FlatMap) {
    std::vector<std::string> vec{ "Banana", "Grapefruit", "Strawberry" };
    auto s = streams::from(vec);

    std::vector<char> check = { 'B', 'a', 'n', 'a', 'n', 'a',
                                'G', 'r', 'a', 'p', 'e', 'f', 'r', 'u', 'i', 't',
                                'S', 't', 'r', 'a', 'w', 'b', 'e', 'r', 'r', 'y' };
    auto res = s.flatMap([](auto&& e) { return e; }).collect();
    ASSERT_EQ(check, res);
}


TEST_F(GeneralTests, FlatMapWithEmpty) {
    std::vector<std::list<std::string>> vec{ {"abc", ""}, {"", "d"}, {}, {"", ""}, {"e"} };
    auto s = streams::from(vec);

    std::vector<char> check = { 'a', 'b', 'c', 'd', 'e',};
    auto res = s.flatten().flatten().collect();
    ASSERT_EQ(check, res);
}


TEST_F(GeneralTests, Flatten) {
    std::vector<std::string> vec{ "Foo", "Bar" };
    std::vector<char> check = { 'F', 'o', 'o', 'B', 'a', 'r' };
    
    auto res = streams::from(vec).flatten().collect();
    ASSERT_EQ(check, res);
}


TEST_F(GeneralTests, Min) {
    auto m = getStream().min();

    ASSERT_EQ(true, static_cast<bool>(m));
    ASSERT_EQ(*std::min_element(vector.begin(), vector.end()), *m);
}

TEST_F(GeneralTests, MinNone) {
    std::vector<int> v{};
    auto m = streams::from(v).min();

    ASSERT_EQ(false, static_cast<bool>(m));
}

TEST_F(GeneralTests, MinCustom) {
    std::vector<std::string> v {"Hurricane", "Oblivion", "Conquistador", "Stay"};
    auto m = streams::from(v).min([](auto&& lhs, auto&& rhs) { return lhs.size() < rhs.size(); });

    ASSERT_EQ(true, static_cast<bool>(m));
    ASSERT_EQ("Stay", *m);
}


TEST_F(GeneralTests, Max) {
    auto m = getStream().max();

    ASSERT_EQ(true, static_cast<bool>(m));
    ASSERT_EQ(*std::max_element(vector.begin(), vector.end()), *m);
}


TEST_F(GeneralTests, FindSome) {
    auto m = getStream().find([](auto&& e) {return e*e == 99 * 99; });

    ASSERT_EQ(true, static_cast<bool>(m));
    ASSERT_EQ(99, *m);
}

TEST_F(GeneralTests, FindNone) {
    auto m = getStream().find([](auto&&) {return false; });

    ASSERT_EQ(false, static_cast<bool>(m));
}


TEST_F(GeneralTests, PositionSome) {
    auto m = getStream().position([](auto&& e) {return e*e == 99 * 99; });

    ASSERT_EQ(true, static_cast<bool>(m));
    ASSERT_EQ(100, *m);
}

TEST_F(GeneralTests, PositionNone) {
    auto m = getStream().position([](auto&& e) {return e < 0; });

    ASSERT_EQ(false, static_cast<bool>(m));
}


TEST_F(GeneralTests, Partition) {
    auto decider = [](auto&& e) {return e % 2; };
    auto pair = getStream().partition(decider);

    std::vector<int> check1;
    std::vector<int> check2;
    for (int i : vector) {
        if (decider(i)) {
            check1.push_back(i);
        } else {
            check2.push_back(i);
        }
    }

    ASSERT_EQ(check1, pair.first);
    ASSERT_EQ(check2, pair.second);
}


TEST_F(GeneralTests, GeneratorCounter) {
    auto c = streams::generate::counter(123)
        .take(1000)
        .count();

    ASSERT_EQ(1000, c);

    auto v = streams::generate::counter(77)
        .take(4)
        .collect();

    std::vector<size_t> check{77, 78, 79, 80};

    ASSERT_EQ(check, v);

    auto v2 = streams::generate::counter()
        .take(5)
        .collect();

    std::vector<size_t> check2{ 0, 1, 2, 3, 4 };

    ASSERT_EQ(check2, v2);
}



namespace streams {
    template<typename T>
    std::ostream& operator << (std::ostream& os, const Enumerated<T>& e) {
        return os << "(" << e.i << ", " << e.v << ")";
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
