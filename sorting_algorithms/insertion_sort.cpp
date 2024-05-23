#include <iostream>
#include <vector>

void insertionSort(std::vector<int> &vec) {
    for (int i = 0; i < vec.size(); i ++) {
        int key = vec[i];
        int j = i - 1;

        while (j >= 0 && vec[j] > key) {
            vec[j + 1] = vec[j];
            j = j - 1;
        }
        vec[j + 1] = key;
    }
}


int main() {
    std::vector<int> vec = {4, 3, 8, 4, 2, 8, 10, 12};


    std::cout << "Current vector: ";
    for (const auto &el : vec) {
        std::cout << el << " ";
    }
    std::cout << std::endl;

    insertionSort(vec);

    std::cout << "Sorted vector: ";
    for (const auto &el : vec) {
        std::cout << el << " ";
    }
    std::cout << std::endl;

    return 0;
}
