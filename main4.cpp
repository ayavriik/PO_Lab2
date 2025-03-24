#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <climits>
#include <algorithm>

const size_t SIZE = 10000000;

int main() {
    
    // ініціалізація масиву випадковими числами
    std::vector<int> arr(SIZE);
    for (size_t i = 0; i < SIZE; ++i) {
        arr[i] = std::rand();
    }
    
    auto start = std::chrono::high_resolution_clock::now();

    int even_count = 0; // лічильник для кількості парних чисел
    int greatest_even = INT_MIN; // змінна для зберігання найбільшого парного числа, початкове значення - мінімальне значення

    // прохід по всіх елементах масиву
    for (size_t i = 0; i < SIZE; ++i) {
        if (arr[i] % 2 == 0) { // перевірка на парність
            ++even_count; // збільшення лічильника парних чисел
            greatest_even = std::max(greatest_even, arr[i]); // // оновлення найбільшого парного числа
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Even numbers: " << even_count << std::endl;
    std::cout << "Greatest even number: " << greatest_even << std::endl;
    std::cout << "Time: " << elapsed.count() << " ms" << std::endl;

    return 0;
}
