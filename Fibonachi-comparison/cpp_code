#include <iostream>
#include <vector>


using std::cin;
using std::cout;
using std::endl;
using std::vector;



class NaiveSolution {
public:
    explicit NaiveSolution (const size_t &n): n(n) {}

    /// functor is a special class that has overloaded operator ()
    /// that is doing some specified logic
    size_t operator() () {
        return solve(n);
    }

private:
    size_t solve (const size_t &n) const {
        if (n <= 1)
            return 1;

        return solve(n - 1) + solve(n - 2);
    }

    size_t n;
};


class MemoizedRecursionSolution {
public:
    /// vector<size_t>(n + 1, 0) its a temporary object of type vector from size_t
    explicit MemoizedRecursionSolution (const size_t &n): n(n), memo(n + 1, 0) {}

    size_t operator() () {
        return solve(n);
    }

private:
    size_t solve (const size_t &n) {
        /// if we have already counted the nth fib num, then we will return the memoized number
        if (memo[n] != 0)
            return memo[n];

        if (n <= 1)
            return memo[n] = 1; /// here we memoize the answer

        return memo[n] = solve(n - 1) + solve(n - 2); /// here we memoize the answer
    }

    vector<size_t> memo;
    size_t n;
};


class DpSolution {
public:
    /// vector<size_t>(n + 1, 0) its a temporary object of type vector from size_t
    explicit DpSolution (const size_t &n): n(n), memo(n + 1, 0) {}

    size_t operator() () {
        memo[0] = memo[1] = 1;

        for (size_t i = 2; i <= n; i ++)
            memo[i] = memo[i - 1] + memo[i - 2];

        return memo[n];
    }

private:
    vector<size_t> memo;
    size_t n;
};


//////////////////////
/// Performance tester
/// This is the class that test performance
/// of every algorithm.

template <class Solution>
class PerformanceTester {
private:
    const size_t step;
    const size_t upper_bound;

public:
    /// We run tests inside the constructor itself:
    explicit PerformanceTester (size_t upper_bound, size_t step):
        upper_bound(upper_bound),
        step(step)
    {}

    vector<size_t> test () {
        vector<size_t> durations;
        durations.reserve(upper_bound);

        for (size_t i = 0; i < upper_bound; i += step) {
            auto start_time = std::chrono::high_resolution_clock::now();
            /// Need to name the object in order to prevent optimization from compiler.
            auto res = Solution(i)();
            auto end_time = std::chrono::high_resolution_clock::now();

            durations.emplace_back(
                std::chrono::duration_cast<std::chrono::microseconds>(
                    end_time - start_time
                ).count()
            );
        }

        return durations;
    }
};


template <typename T>
void print_vec (const vector<T> &vec) {
    cout << "[";

    for (size_t i = 0; i < vec.size(); i ++)
        cout << vec[i] << std::string(i == vec.size() - 1 ? "" : ", ");

    cout << "]";
}

int main () {
    /// I've chosen this upper_bound because it's the largest
    /// number that can be stored inside the size_t variable (it's unsigned long variable).
    PerformanceTester<NaiveSolution> naive(47, 1);
    PerformanceTester<MemoizedRecursionSolution> memoizedRecursion(100000, 100);
    PerformanceTester<DpSolution> dpSolution(100000, 100);

    cout << "naive = ";
    print_vec(naive.test());
    cout << endl;

    cout << "memoized = ";
    print_vec(memoizedRecursion.test());
    cout << endl;

    cout << "dp = ";
    print_vec(dpSolution.test());
    cout << endl;

    return 0;
}

