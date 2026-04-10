#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex mtx;

void worker(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "thread " << id << " running" << std::endl;
}

int counter = 0;

void incrementUnsafe() {
    for (int i = 0; i < 100000; i++) {
        counter++;
    }
}

void incrementSafe() {
    for (int i = 0; i < 100000; i++) {
        std::lock_guard<std::mutex> lock(mtx);
        counter++;
    }
}

std::mutex mtx1;
std::mutex mtx2;

void threadA() {
    std::lock_guard<std::mutex> lockA(mtx1);  // acquires mtx1
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // small delay
    std::lock_guard<std::mutex> lockB(mtx2);  // waits for mtx2
    std::cout << "thread A done" << std::endl;
}

void threadB() {
    std::lock_guard<std::mutex> lockB(mtx1);  // acquires mtx2
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // small delay
    std::lock_guard<std::mutex> lockA(mtx2);  // waits for mtx1
    std::cout << "thread B done" << std::endl;
}

int main() {

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    std::thread t3(worker, 3);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "all threads done" << std::endl;

    // unsafe version
    counter = 0;
    std::thread u1(incrementUnsafe);
    std::thread u2(incrementUnsafe);
    u1.join();
    u2.join();
    std::cout << "unsafe counter: " << counter << " (should be 200000)" << std::endl;

    // safe version
    counter = 0;
    std::thread s1(incrementSafe);
    std::thread s2(incrementSafe);
    s1.join();
    s2.join();
    std::cout << "safe counter: " << counter << " (should be 200000)" << std::endl;

    std::thread ta(threadA);
    std::thread tb(threadB);
    ta.join();
    tb.join();

    return 0;
}