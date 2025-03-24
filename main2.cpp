#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <climits>
#include <algorithm>

const size_t SIZE = 10000000;
const size_t THREADS = 20;

int main() {

    // вектор з заданого SIZE елементів
    std::vector<int> arr(SIZE);
    for (size_t i = 0; i < SIZE; ++i) {
        arr[i] = std::rand(); // задавання рандомного значення елементам масиву
    }
    
    auto start = std::chrono::high_resolution_clock::now();

    // оголошення атомарних змінних для зберігання загальної кількості парних чисел та найбільшого парного числа
    std::atomic<unsigned int> even(0); // початкове значення 0
    std::atomic<int> greatest(INT_MIN); // початкове значення - мін. ціле число

    std::vector<std::thread> threads;
    for (size_t t = 0; t < THREADS; ++t) { // створення потоків для паралельної обробки масиву
        
        threads.emplace_back([&, t]() { // створення потоку з лямбда-функцією // приймає всі змінні за посиланням та копіює t
            int even_count = 0; // змінна для підрахунку парних чисел
            int greatest_number = INT_MIN; // змінна для зберігання найбільшого парного числа
            
            // обчислення початкового та кінцевого індексів для даного потоку, що забезпечує розподіл роботи
            size_t start_index = t * SIZE / THREADS;
            size_t end_index = (t + 1) * SIZE / THREADS;
            
            // обхід відповідного сегменту масиву
            for (size_t i = start_index; i < end_index; ++i) {
                if (arr[i] % 2 == 0) { // перевірка на парність
                    ++even_count; // збільшення лічильника парних чисел
                    greatest_number = std::max(greatest_number, arr[i]); // оновлення локального найбільшого парного числа
                }
            }
            // оновлення атомарної змінної even за допомогою циклу compare_exchange
            unsigned int current_even = even.load(std::memory_order_seq_cst);
            while (true) {
                unsigned int new_even = current_even + even_count; // обчислення нового значення
                
                // спроба атомарного оновлення even: якщо успішно, вихід з циклу
                if (even.compare_exchange_strong(current_even, new_even, std::memory_order_seq_cst)) {
                    break;
                } else {
                    std::cout << "Thread " << t << " failed to update even count\n";
                }
            }

            // завантаження поточного значення greatest
            int current_greatest = greatest.load(std::memory_order_seq_cst);
            while (true) {

                // якщщо поточне значення вже більше або дорівнює локальному greatest_number, оновлення не потрібно
                if (current_greatest >= greatest_number)
                    break;

                // спроба атомарного оновлення greatest: якщо успішно, вихід з циклу
                if (greatest.compare_exchange_strong(current_greatest, greatest_number, std::memory_order_seq_cst)) {
                    break;
                
                } else {
                    std::cout << "Thread " << t << " failed to update greatest number\n";
                }
            }
        });
    }

    // очікування завершення всіх потоків
    for (auto &th : threads) {
        th.join(); // головний потік чекає, поки кожен з потоків завершить виконання
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Even numbers: " << even.load(std::memory_order_seq_cst) << std::endl;
    std::cout << "Greatest even number: " << greatest.load(std::memory_order_seq_cst) << std::endl;
    std::cout << "Time: " << elapsed.count() << " ms" << std::endl;

    return 0;
}
