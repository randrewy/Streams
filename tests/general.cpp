#include<vector>
#include<string>
#include<algorithm>
#include<utility>
#include<list>
#include<iostream>
#include "..\Streams.h"
#include "gtest/gtest.h"

class GeneralTests : public ::testing::Test {
	const int size = 100;

protected:
	std::vector<int> vector;
	void SetUp() {
		vector.reserve(size);
		for (int i = 0; i < size; ++i) {
			vector.push_back(i);
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
	for (size_t i = 0; i < vector.size(); i++) {
		if (vector[i] >= 10) {
			break;
		}
		check.push_back(vector[i]);
	}

	ASSERT_EQ(check, vec);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}