#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <climits>
#include <algorithm>

const size_t SIZE = 10000000; 
const size_t THREADS = 20;

int main() {
    std::vector<int> arr(SIZE); 
    for (size_t i = 0; i < SIZE; ++i) {
        arr[i] = std::rand(); // встановлення випадкового числа кожному елементу
    }
    
    auto start = std::chrono::high_resolution_clock::now(); 

    int even_total = 0; // кількість парних чисел
    int greatest_even = INT_MIN; 
    std::mutex mtx_even, mtx_greatest;  // 2 мʼютекси для синхронізації доступу

    std::vector<std::thread> threads;
    for (size_t t = 0; t < THREADS; ++t) { // створення заданої кількості потоків
        threads.emplace_back([&, t]() { // стіорення нового потоку
            int even_count = 0; // лічильник парних чисел у потоці
            int greatest_number = INT_MIN; // зміна, яка зберігає найбіл. парне число в потоці
            size_t start_index = t * SIZE / THREADS; // розрахунок почат. індексу
            size_t end_index = (t + 1) * SIZE / THREADS; // кінцевого індексу
            for (size_t i = start_index; i < end_index; ++i) { 
                if (arr[i] % 2 == 0) { // перевірка на парність
                    ++even_count; // + локального лічильника парних чисел
                    greatest_number = std::max(greatest_number, arr[i]); // оновлення найбільшого парного числа
                }
            }
            { // синхронізований блок для оновлення заг. кіл-ті парнх чисел
                std::lock_guard<std::mutex> lock(mtx_even);
                even_total += even_count;
            }
            { // синхронізований блок для оновлення найбільшого парного числа
                std::lock_guard<std::mutex> lock(mtx_greatest);
                greatest_even = std::max(greatest_even, greatest_number);
            }
        });
    }

    for (auto &th : threads) { // перебір усіх потоків
        th.join(); // очікування завершення кожного потоку
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Even numbers: " << even_total << std::endl;
    std::cout << "Greatest even number: " << greatest_even << std::endl;
    std::cout << "Time: " << elapsed.count() << " ms" << std::endl;

    return 0;
}
