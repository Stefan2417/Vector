#define CATCH_CONFIG_MAIN

#include "catch2/catch_all.hpp"
#include <vector>
#include "../src/Vector.h"
#include <chrono>
#include <random>
#include <iostream>
#include <memory>

std::mt19937 rnd(std::chrono::high_resolution_clock::now().time_since_epoch().count());

template<typename T>
void Check(const Vector<T> &actual, const std::vector<T> &expected) {
    REQUIRE(actual.Size() == expected.size());
    for (size_t i = 0; i < actual.Size(); ++i) {
        REQUIRE(actual[i] == expected[i]);
    }
}

struct Throwing {
    Throwing() {
        throw 42;
    }
};


struct Throwing42 {
    static size_t count;

    Throwing42() {
        count++;
        if (count > 42) {
            throw std::runtime_error("More than 42 instances created!");
        }
    }
};

size_t Throwing42::count = 0;

struct Int {
    int value_;

    Int() = delete;

    Int(int value) : value_(value) {
    }

    Int(const Int &other) : value_(other.value_) {
    }
};

TEST_CASE("Construct with deleted constructor") {
    Vector<Int> v(32, 4);
    Vector<Int> v2;
    REQUIRE(v.Size() == 32);
    REQUIRE(v2.Empty());
}


TEST_CASE("Default Constructor") {
    Vector<int> vec;
    REQUIRE(vec.Size() == 0);
    REQUIRE(vec.Capacity() == 0);
    REQUIRE(vec.Data() == nullptr);
}

TEST_CASE("Initializer List Constructor") {
    Vector<int> vec{1, 2, 3, 4, 5};
    REQUIRE(vec.Size() == 5);
    REQUIRE(vec.Capacity() >= 5);

    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
    REQUIRE(vec[3] == 4);
    REQUIRE(vec[4] == 5);
}

TEST_CASE("Count and Value Constructor") {
    Vector<int> vec(4, 10);
    REQUIRE(vec.Size() == 4);
    REQUIRE(vec.Capacity() >= 4);

    for (size_t i = 0; i < vec.Size(); ++i) {
        REQUIRE(vec[i] == 10);
    }
}

TEST_CASE("Copy Constructor") {
    Vector<int> original{1, 2, 3, 4};
    Vector<int> copy = original;
    REQUIRE(copy.Size() == original.Size());
    REQUIRE(copy.Capacity() >= original.Size());

    for (size_t i = 0; i < original.Size(); ++i) {
        REQUIRE(copy[i] == original[i]);
    }
}

TEST_CASE("Move Constructor") {
    Vector<int> original{1, 2, 3, 4};
    size_t original_capacity = original.Capacity();
    Vector<int> moved = std::move(original);

    REQUIRE(moved.Size() == 4);
    REQUIRE(moved.Capacity() == original_capacity);

    REQUIRE(moved[0] == 1);
    REQUIRE(moved[1] == 2);
    REQUIRE(moved[2] == 3);
    REQUIRE(moved[3] == 4);

    REQUIRE(original.Size() == 0);
    REQUIRE(original.Capacity() == 0);
    REQUIRE(original.Data() == nullptr);
}

TEST_CASE("Memory layout") {
    Vector<int> a(129);
    for (int i = 0; i < 127; ++i) {
        REQUIRE(&a[i] + 1 == &a[i + 1]);
    }
}

TEST_CASE("Copy correctness") {
    Vector<int> a;
    Vector<int> b(a);
    b.PushBack(1);
    Check(a, std::vector<int>());
    Check(b, std::vector<int>{1});

    b = b;  // NOLINT
    Check(b, std::vector<int>{1});
    a = b;
    Check(a, std::vector<int>{1});

    b = std::move(a);
    Check(b, std::vector<int>{1});
    Vector<int> c(std::move(b));
    Check(c, std::vector<int>{1});

    Vector<int> d{3, 4, 5};
    Vector<int> e(d);
    Check(e, std::vector<int>{3, 4, 5});
    d.Swap(c);
    Check(e, std::vector<int>{3, 4, 5});
    Check(d, std::vector<int>{1});
    Check(c, std::vector<int>{3, 4, 5});
}


TEST_CASE("std::vector basic operations") {
    Vector<int> vec;

    SECTION("Initial Size and capacity") {
        REQUIRE(vec.Size() == 0);
        REQUIRE(vec.Capacity() == 0);
    }

    SECTION("Push back elements and check Size") {
        vec.PushBack(1);
        vec.PushBack(2);
        vec.PushBack(3);
        REQUIRE(vec.Size() == 3);
        REQUIRE(vec[0] == 1);
        REQUIRE(vec[1] == 2);
        REQUIRE(vec[2] == 3);
    }

    SECTION("Pop back elements and check Size") {
        vec.PushBack(1);
        vec.PushBack(2);
        vec.PushBack(3);
        vec.PopBack();
        REQUIRE(vec.Size() == 2);
        REQUIRE(vec.Back() == 2);
    }


    SECTION("Clear the vector") {
        vec.PushBack(1);
        vec.PushBack(2);
        vec.PushBack(3);
        vec.Clear();
        REQUIRE(vec.Size() == 0);
        REQUIRE(vec.Empty());
    }
}

TEST_CASE("capacity and resizing") {
    Vector<int> vec(5, 10);

    SECTION("Resize to a smaller size") {
        vec.Resize(3);
        REQUIRE(vec.Size() == 3);
        REQUIRE(vec.Capacity() >= 3);
    }

    SECTION("Reserve capacity") {
        vec.Reserve(10);
        REQUIRE(vec.Capacity() >= 10);
    }
}

TEST_CASE("exceptions and bounds checking") {
    Vector<int> vec = {1, 2, 3};

    SECTION("Access out of bounds with at()") {
        REQUIRE_THROWS_AS(vec.At(5), std::out_of_range);
    }

    SECTION("Access in bounds with at()") {
        REQUIRE(vec.At(1) == 2);
    }
}

TEST_CASE("vector iterator functionality") {
    Vector<int> vec = {1, 2, 3};

    SECTION("Forward iteration with iterators") {
        auto it = vec.begin(); // NOLINT
        REQUIRE(*it == 1);
        ++it;
        REQUIRE(*it == 2);
    }

}

TEST_CASE("StressTest") {
    const size_t large_size = 1'000'000;

    SECTION("PushBack Stress Test") {
        std::vector<int> std_vec;
        Vector<int> my_vec;

        for (size_t i = 0; i < large_size; ++i) {
            std_vec.push_back(static_cast<int>(i));
            my_vec.PushBack(static_cast<int>(i));
        }

        REQUIRE(std_vec.size() == large_size);
        REQUIRE(my_vec.Size() == large_size);

        for (size_t i = 0; i < large_size; ++i) {
            REQUIRE(std_vec[i] == my_vec[i]);
        }
    }

    SECTION("Copy Stress Test") {
        std::vector<int> std_vec(large_size, 42);
        Vector<int> my_vec(large_size, 42);

        std::vector<int> std_vec_copy = std_vec;
        Vector<int> my_vec_copy = my_vec;

        REQUIRE(std_vec_copy.size() == large_size);
        REQUIRE(my_vec_copy.Size() == large_size);

        for (size_t i = 0; i < large_size; ++i) {
            REQUIRE(std_vec_copy[i] == 42);
            REQUIRE(my_vec_copy[i] == 42);
        }
    }

    SECTION("Random Access Stress Test") {
        std::vector<int> std_vec(large_size, 42);
        Vector<int> my_vec(large_size, 42);

        std::uniform_int_distribution<int> azino(0, large_size - 1);
        std::uniform_int_distribution<int> azino2(INT32_MIN, INT32_MAX);

        for (size_t i = 0; i < large_size; ++i) {
            REQUIRE(std_vec.at(i) == 42);
            REQUIRE(my_vec[i] == 42);
        }

        for (size_t i = 0; i < large_size; i++) {
            int p = azino(rnd);
            int x = azino(rnd);
            std_vec[p] = x;
            my_vec[p] = x;
        }

        for (size_t i = 0; i < large_size; i++) {
            int p = azino(rnd);
            REQUIRE(std_vec.at(p) == my_vec.At(p));
        }
    }

    SECTION("Random Operations") {
        std::vector<int> std_vec;
        Vector<int> my_vec;

        std::uniform_int_distribution<int> azino2(INT32_MIN, INT32_MAX);
        std::uniform_int_distribution<int> azino3(1, 7);
        std::uniform_int_distribution<int> azino4(0, large_size / 100);


        const size_t operations = 50'000;

        for (size_t op = 0; op < operations; ++op) {
            std::uniform_int_distribution<int> azino(0, std::max<int>(1, std_vec.size()) - 1);
            int t = azino3(rnd);
            REQUIRE(my_vec.Size() == std_vec.size());
            if (t == 1) {
                int x = azino2(rnd);
                std_vec.push_back(x);
                my_vec.PushBack(x);
            } else if (t == 2 && my_vec.Size() > 0) {
                int p = azino(rnd);
                REQUIRE(std_vec[p] == my_vec[p]);
            } else if (t == 3 && my_vec.Size() > 0) {
                std_vec.pop_back();
                my_vec.PopBack();
                REQUIRE(my_vec.Size() == std_vec.size());
            } else if (t == 4) {
                int sz = azino4(rnd);
                int x = azino2(rnd);
                my_vec.Resize(sz, x);
                std_vec.resize(sz, x);
                REQUIRE(my_vec.Size() == std_vec.size());
            } else if (t == 5) {
                my_vec.Clear();
                std_vec.clear();
                REQUIRE(my_vec.Size() == std_vec.size());
            } else if (t == 6 && my_vec.Size() > 0) {
                REQUIRE(my_vec.Back() == std_vec.back());
            } else if (t == 7 && my_vec.Size() > 0) {
                REQUIRE(my_vec.Front() == std_vec.front());
            }
        }
    }

}


TEST_CASE("Faulty constructors") {

    SECTION("Throw") {
        REQUIRE_THROWS(Vector<Throwing>(100));
    }

    SECTION("ThrowEq42") {
        try {
            auto sp = Vector<Throwing42>();
            for (size_t i = 0; i <= 42; i++) {
                sp.PushBack(Throwing42());
            }
            REQUIRE(false);
        } catch (...) {
        }
    }

    SECTION("ThrowMore42") {
        try {
            auto sp = Vector<Throwing42>();
            for (size_t i = 0; i <= 43; i++) {
                sp.PushBack(Throwing42());
            }
            REQUIRE(false);
        } catch (...) {
        }
    }

}
