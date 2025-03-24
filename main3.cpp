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
    // ініціалізація масиву випадковими числами
    std::vector<int> arr(SIZE);
    for (size_t i = 0; i < SIZE; ++i) {
        arr[i] = std::rand();
    }
    
    auto start = std::chrono::high_resolution_clock::now();

    // оголошення атомарної змінної для загальної кількості парних чисел 
    std::atomic<unsigned int> even(0);

    // оголошення атомарної змінної для найбільшого знайденого парного числа 
    std::atomic<int> greatest(INT_MIN);

    // створення вектора для зберігання об'єктів потоків
    std::vector<std::thread> threads;

    // запуск THREADS потоків для паралельного опрацювання масиву
    for (size_t t = 0; t < THREADS; ++t) {
        
        // створення нового потоку за допомогою лямбда-функції, де t копіюється, а інші змінні захоплюються за посиланням
        threads.emplace_back([&, t]() {
            int even_count = 0; // лічильник парних чисел для поточного потоку
            int greatest_number = INT_MIN; // змінна для найбільшого парного числа
            
            // обчислення початкового та кінцевого індексів сегмента масиву для поточного потоку
            size_t start_index = t * SIZE / THREADS;
            size_t end_index = (t + 1) * SIZE / THREADS;
            
            // прохід по призначеному діапазону масиву
            for (size_t i = start_index; i < end_index; ++i) {
                if (arr[i] % 2 == 0) { // перевірка на парність
                    ++even_count; // збільшення лічильника парних чисел
                    greatest_number = std::max(greatest_number, arr[i]); // // оновлення локальної змінної, якщо поточне число більше
                }
            }
            // атомарне додавання локального підрахунку парних чисел до загальної змінної
            even.fetch_add(even_count, std::memory_order_seq_cst);

            // оновлення атомарної змінної, що зберігає найбільше парне число
            int current = greatest.load(std::memory_order_seq_cst);
            while (true) {
                if (greatest_number > current) {

                    // спроба атомарного оновлення greatest
                    if (greatest.compare_exchange_strong(current, greatest_number, std::memory_order_seq_cst)) {
                        break;
                    }
                    // current оновиться автоматично
                } else {
                    break; // якщо greatest_number не перевищує current, оновлення не потрібне
                }
            }
        });
    }

    // очікування завершення всіх потоків, шляхом виклику join() для кожного потоку
    for (auto &th : threads) {
        th.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Even numbers: " << even.load(std::memory_order_seq_cst) << std::endl;
    std::cout << "Greatest even number: " << greatest.load(std::memory_order_seq_cst) << std::endl;
    std::cout << "Time: " << elapsed.count() << " ms" << std::endl;

    return 0;
}
