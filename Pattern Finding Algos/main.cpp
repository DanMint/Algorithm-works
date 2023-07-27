

Skip to content
Using Gmail with screen readers
algo.hw@gmail.com 
Meet
Hangouts

1 of 3
HW 3

Zavio <dannymints@gmail.com>
Attachments
Wed, May 19, 5:39 AM (7 days ago)
to algo.hw

Hello sir im sending you my hw3 incase i did something wrong with the git.
2 Attachments
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>


/// Just a simple util for printing vectors
template <typename vt>
std::string vector_to_string (const std::string& name, const std::vector<vt>& vec) {
    std::string str;
    str += name;
    str += " = [";

    for (size_t i = 0; i < vec.size(); i ++) {
        str += (i != 0 ? ", " : "");
        str += std::to_string(vec[i]);
    }

    str += "]";
    return str;
}


/// ------ SOLUTIONS ----- /////

struct solution {
    typedef std::vector<size_t> output_t;
    typedef std::pair<std::string, std::string> input_t;
    virtual output_t substr (const input_t& input) const = 0;
};

struct naive_solution : solution {
    std::vector<size_t> substr (const input_t& input) const override {
        const std::string& str = input.first;
        const std::string& pattern = input.second;

        if (str.size() < pattern.size())
            return std::vector<size_t>(0);

        std::vector<size_t> indices;

        for (size_t i = 0; i < str.size() - pattern.size() + 1; i ++) {
            bool matched = true;

            for (size_t j = 0; j < pattern.size(); j ++) {
                if (str[i + j] != pattern[j]) {
                    matched = false;
                    break;
                }
            }

            if (matched)
                indices.push_back(i);
        }

        return indices;
    }
};

struct rabin_karp_solution : solution {
    std::vector<size_t> substr (const input_t& input) const override {
        const std::string& str = input.first;
        const std::string& pattern = input.second;

        if (pattern.size() > str.size())
            return std::vector<size_t>(0);

        const long long x = 31;
        const size_t m = pattern.size();

        std::vector<long long> x_powers(str.size());
        x_powers[0] = 1;

        for (size_t i = 1; i < x_powers.size(); i ++)
            x_powers[i] = x_powers[i - 1] * x;

        /// Pattern's and str's hash:
        long long pattern_hash = 0;
        long long str_hash = 0;

        for (size_t i = 0; i < m; i ++) {
            pattern_hash += static_cast<long long>(pattern[i]) * x_powers[m - i - 1];
            str_hash += static_cast<long long>(str[i]) * x_powers[m - i - 1];
        }

        /// Algorithm itself
        std::vector<size_t> indices;

        for (int i = 0; i < str.size() - m + 1; i ++) {
            if (str_hash == pattern_hash && pattern == str.substr(i, m))
                indices.push_back(i);

            if (i != str.size() - m) {
                str_hash -= static_cast<long long>(str[i]) * x_powers[m - 1];
                str_hash *= x;
                str_hash += static_cast<long long>(str[i + m]);
            }
        }

        return indices;
    }
};


/// ------ GENERATOR & TESTER ----- /////

struct generator {
    typedef std::pair<size_t, size_t> params_t;

    static solution::input_t generate (const params_t& params) {
        const size_t str_size = params.first;
        const size_t pattern_size = params.second;

        std::string str;

        std::random_device random_device;
        std::mt19937 random_generator(random_device());
        std::uniform_int_distribution<char> alphabet('a', 'z');

        for (size_t i = 0; i < str_size; i ++)
            str += alphabet(random_generator);

        std::string pattern = str.substr((str_size - pattern_size) / 2, pattern_size);

        return std::pair(str, pattern);
    }
};

struct tester {
    typedef std::pair<solution*, std::string> solution_and_name_t;

    struct test_failed : std::exception {
        explicit test_failed (const std::string& description):
                _description(std::string("TEST FAILED: ").append(description)) {}

        const char* what () const noexcept override { return _description.c_str(); }

    private:
        std::string _description;
    };

    explicit tester (const std::vector<solution_and_name_t>& solutions_and_names) {
        _solutions.reserve(solutions_and_names.size());
        _names.reserve(solutions_and_names.size());

        for (const solution_and_name_t& solution_and_name : solutions_and_names) {
            _solutions.emplace_back(solution_and_name.first);
            _names.emplace_back(solution_and_name.second);
        }
    }

    std::vector<size_t> test_and_benchmark (const solution::input_t& input) {
        std::vector<solution::output_t> outputs;
        std::vector<size_t> times;

        outputs.reserve(_solutions.size());
        times.reserve(_solutions.size());

        for (const solution* const sol : _solutions) {
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            outputs.emplace_back(sol->substr(input));

            times.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - start
            ).count());
        }

        /// Comparing the outputs:

        for (size_t algo_num = 0; algo_num < _solutions.size(); algo_num ++) {
            std::sort(outputs[algo_num].begin(), outputs[algo_num].end());

            if (algo_num > 0 && outputs[algo_num] != outputs[algo_num - 1])
                throw test_failed(
                        "Different outputs on algorithms " +
                        _names[algo_num - 1] + " and " + _names[algo_num] + "\n" +
                        vector_to_string(_names[algo_num - 1], outputs[algo_num - 1]) + "\n" +
                        vector_to_string(_names[algo_num], outputs[algo_num]) + "\n" +
                        "string: " + input.first + "\n" +
                        "pattern: " + input.second + "\n"
                );
        }

        return times;
    }

    size_t solutions_count () { return _solutions.size(); }

    const std::vector<std::string>& get_names () { return _names; }

private:
    std::vector<solution*> _solutions;
    std::vector<std::string> _names;
};


int main () {
    naive_solution n;
    rabin_karp_solution rk;

    tester t(std::vector<tester::solution_and_name_t>{
            { &n, "naive" },
            { &rk, "rabin karp" },
    });

    std::vector<generator::params_t> tests({
                                                   /// Ascending str_size
                                                   { 1000, 10 },
                                                   { 2000, 10 },
                                                   { 3000, 10 },
                                                   { 4000, 10 },
                                                   { 5000, 10 },
                                                   { 6000, 10 },
                                                   { 7000, 10 },
                                                   { 8000, 10 },
                                                   { 9000, 10 },
                                                   { 10000, 10 },

                                                   /// Ascending pattern_size
                                                   { 20000, 1000 },
                                                   { 20000, 2000 },
                                                   { 20000, 3000 },
                                                   { 20000, 4000 },
                                                   { 20000, 5000 },
                                                   { 20000, 6000 },
                                                   { 20000, 7000 },
                                                   { 20000, 8000 },
                                                   { 20000, 9000 },
                                                   { 20000, 10000 },
                                           });

    std::vector<std::vector<size_t>> test_solution_time;
    test_solution_time.reserve(tests.size());

    for (size_t test_num = 0; test_num < tests.size(); test_num ++) {
        const std::string delimiter = "-------------------------------";

        std::cout <<
                  delimiter << std::endl <<
                  "TEST " << (test_num + 1) << " STARTED, params: { " <<
                  "string size: " << tests[test_num].first << ", " <<
                  "pattern size: " << tests[test_num].second << " }" << std::endl;

        try {
            test_solution_time.emplace_back(
                    t.test_and_benchmark(
                            generator::generate(tests[test_num])
                    )
            );
        } catch (const tester::test_failed& exception) {
            std::cout << exception.what() << std::endl;
            std::cout << delimiter << std::endl;
            return 0;
        }

        std::cout << "TEST PASSED" << std::endl;

        if (test_num == tests.size() - 1)
            std::cout << delimiter << std::endl << std::endl;
    }

    for (size_t algo_num = 0; algo_num < t.solutions_count(); algo_num ++) {
        std::vector<size_t> str_sizes, pattern_sizes, times;

        for (size_t test_num = 0; test_num < tests.size(); test_num ++) {
            str_sizes.emplace_back(tests[test_num].first);
            pattern_sizes.emplace_back(tests[test_num].second);
            times.emplace_back(test_solution_time[test_num][algo_num]);
        }

        std::cout << vector_to_string("str_sizes", str_sizes) << std::endl;
        std::cout << vector_to_string("pattern_sizes", pattern_sizes) << std::endl;
        std::cout << vector_to_string("times", times) << std::endl;

        std::cout << std::endl;
    }
}
main.cpp
Displaying main.cpp. 
