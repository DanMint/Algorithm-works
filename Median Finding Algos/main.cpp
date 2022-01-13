#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>



struct IMedianFindingAlgorithm {
    virtual std::string getName () const = 0;
    virtual std::pair<int, int> getMedian (std::vector<int>) const = 0;
};


struct StandardAlgorithm : IMedianFindingAlgorithm {
    std::string getName () const override { return "Standard algorithm"; }

    std::pair<int, int> getMedian (std::vector<int> sequence) const override {
        const size_t n = sequence.size();

        std::nth_element(sequence.begin(), sequence.begin() + n / 2, sequence.end());
        const int higher_median = sequence[n / 2];

        if (n % 2 == 0) {
            std::nth_element(sequence.begin(), sequence.begin() + n / 2 - 1, sequence.end());
            const int lower_median = sequence[n / 2 - 1];

            return std::pair<int, int>(lower_median, higher_median);
        }

        else
            return std::pair<int, int>(higher_median, higher_median);
    }
};


struct NaiveAlgorithm : IMedianFindingAlgorithm {
    std::string getName () const override { return "Naive algorithm"; }

    std::pair<int, int> getMedian (std::vector<int> sequence) const override {
        std::sort(sequence.begin(), sequence.end());

        const size_t n = sequence.size();

        return
                (n % 2 == 0) ? /// If it is even
                std::pair<int, int>(sequence[n / 2 - 1], sequence[n / 2]) :
                std::pair<int, int>(sequence[n / 2], sequence[n / 2]);
    }
};


struct QuickSelectAlgorithm : IMedianFindingAlgorithm {
    std::string getName () const override { return "Quickselect algorithm"; }

    std::pair<int, int> getMedian (std::vector<int> sequence) const override {
        const size_t n = sequence.size();
        const size_t higherMedian = quickSelect(sequence, 0, n, n / 2);

        return
                (n % 2 == 0) ? /// If it is even
                std::pair<int, int>(quickSelect(sequence, 0, n, n / 2 - 1), higherMedian) :
                std::pair<int, int>(higherMedian, higherMedian);
    }

private:
    int quickSelect (
            std::vector<int>& sequence,
            const size_t l,
            const size_t r,
            const size_t k
    ) const {
        const auto red_and_blue_count = partition(sequence, l, r);

        const size_t white_start = l + red_and_blue_count.first;
        const size_t blue_start = r - red_and_blue_count.second;

        if (k < white_start)
            return quickSelect(sequence, l, white_start, k);
        else if (k < blue_start)
            return sequence[k];
        else
            return quickSelect(sequence, blue_start, r, k);
    }

    /// returns amounts of red and blue elements.
    std::pair<size_t, size_t> partition (
            std::vector<int>& sequence,
            const size_t l,
            const size_t r
    ) const {
        const int pivot = sequence[(l + r) / 2];
        size_t c = l;
        size_t f = l;

        /// Red part:
        while (c != r) {
            if (sequence[c] < pivot) /// is red
                std::swap(sequence[c], sequence[f ++]);

            ++ c;
        }

        const size_t red_count = f - l;

        /// White part:
        c = f;

        while (c != r) {
            if (sequence[c] == pivot) /// is white
                std::swap(sequence[c], sequence[f ++]);

            ++ c;
        }

        const size_t blue_count = r - f;

        return std::pair<size_t, size_t>(red_count, blue_count);
    }
};


struct Generator {
    std::vector<int> generate (const size_t n) const {
        std::vector<int> sequence;

        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<int> _distribution(-1000, 1000);

        /// generate n random numbers in [from, to]
        for (size_t i = 0; i < n; i ++)
            sequence.emplace_back(_distribution(generator));

        return sequence;
    }
};


struct Tester {
    typedef std::pair<int, int> output;

    explicit Tester (const std::vector<const IMedianFindingAlgorithm*>& algorithms): _algorithms(algorithms) {}

    /// Returns an output and computation time for every algorithm.
    std::vector<std::pair<output, size_t>> test (const std::vector<int>& sequence) const {
        std::vector<std::pair<output, size_t>> outputs;

        for (const IMedianFindingAlgorithm* algorithm : _algorithms) {
            std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
            const auto output = algorithm->getMedian(sequence);
            std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
            const size_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            outputs.emplace_back(output, ms);
        }

        return outputs;
    }

    bool check (const std::vector<output>& outputs, const output& trueOutput) const {
        for (const output& currentOutput : outputs)
            if (currentOutput != trueOutput)
                return false;

        return true;
    }

private:
    std::vector<const IMedianFindingAlgorithm*> _algorithms;
};


int main () {
    Generator generator;

    const StandardAlgorithm sa;
    const NaiveAlgorithm na;
    const QuickSelectAlgorithm qsa;
    const std::vector<const IMedianFindingAlgorithm*> algorithms({ &sa, &na, &qsa });

    const Tester tester(algorithms);

    /// size of the sequence
    std::vector<size_t> tests({
                                      10000000,
                                      20000000,
                                      30000000,
                                      40000000,
                                      50000000,
                                      60000000,
                                      70000000,
                                      80000000,
                                      90000000,
                                      100000000
                              });
    std::vector<std::vector<size_t>> algorithm_time_relation(algorithms.size());

    const size_t k_repetitions_count = 5; /// every test will be repeated 5 times.

    for (const size_t current_size_of_sequence : tests) {
        std::vector<size_t> average_ms(algorithms.size());

        for (size_t _ = 0; _ < k_repetitions_count; _ ++) {
            std::vector<int> sequence = generator.generate(current_size_of_sequence);

            const std::vector<std::pair<Tester::output, size_t>> outputs = tester.test(sequence);

            std::vector<Tester::output> only_outputs;
            only_outputs.reserve(outputs.size());

            for (const auto& el : outputs)
                only_outputs.emplace_back(el.first);

            if (!tester.check(only_outputs, only_outputs[0])) {
                std::cout << "Test failed!" << std::endl << "Sequence: ";

                for (const int number : sequence)
                    std::cout << number << " ";

                std::cout << std::endl;

                for (size_t i = 0; i < algorithms.size(); i ++)
                    std::cout << algorithms[i]->getName() << ": " << (
                            only_outputs[i].first == only_outputs[i].second ?
                            std::to_string(only_outputs[i].first) :
                            "(" + std::to_string(only_outputs[i].first) + ", " +
                            std::to_string(only_outputs[i].second) + ")"
                    ) << std::endl;

                return 0;
            }

            for (size_t i = 0; i < outputs.size(); i ++)
                average_ms[i] += outputs[i].second;
        }

        for (size_t i = 0; i < average_ms.size(); i ++)
            algorithm_time_relation[i].emplace_back(average_ms[i] / k_repetitions_count);
    }

    for (size_t i = 0; i < algorithms.size(); i ++) {
        std::cout << algorithms[i]->getName() << std::endl;

        for (size_t j = 0; j < tests.size(); j ++)
            std::cout << "n = " << tests[j] << ": " << algorithm_time_relation[i][j] << std::endl;

        std::cout << std::endl;
    }
}
