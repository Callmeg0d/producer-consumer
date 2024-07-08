#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv;

bool isProducerDone = false;

void producer() {
    for (int i = 1; i <= 11; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.push(i);
        std::cout << "Produced: " << i << std::endl;
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
    }
    isProducerDone = true;
}

void consumer(char consumerId) {
    for (int i = 1; i <= 11; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty() || isProducerDone; });
        if (!buffer.empty()) {
            int data = buffer.front();
            buffer.pop();

            std::cout << "Consumer " << consumerId << " received the data: " << data << std::endl;
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        else if (isProducerDone)
            break;
    }
}

int main() {
    std::thread producerThread(producer);
    const int numConsumers = 3;
    const char* names = "ABC";
    // Создание потоков потребителей
    std::vector<std::thread> consumer_threads;
    for (int i = 0; i < numConsumers; i++) {
        char name = names[i];
        consumer_threads.push_back(std::thread(consumer, name));
    }
    
    for (int i = 0; i < numConsumers; ++i) {
        consumer_threads[i].join();
    }
    producerThread.join();
    return 0;
}
