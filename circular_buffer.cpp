#include <iostream>
#include <cstdint>
#include <cassert>

class CircularBuffer {
private:
    static constexpr int CAPACITY = 8;
    uint8_t m_buffer[CAPACITY];
    int m_head = {};    // write index
    int m_tail = {};    // read index
    int m_count = {};   // number of items currently in buffer

public:
    bool write(uint8_t data) {
        // return false if buffer is full
        // write data at m_head
        // advance m_head, wrapping around using modulo
        // increment m_count
        if (m_count == CAPACITY) {
            return false;
        }
        else {
            m_buffer[m_head] = data;
            m_head = (m_head+1) % CAPACITY;
            m_count++;
            return true;
        }
    }

    bool read(uint8_t& data) {
        // return false if buffer is empty
        // read data from m_tail into data parameter
        // advance m_tail, wrapping around
        // decrement m_count
        if (!m_count) {
            return false;
        }
        else {
            data = m_buffer[m_tail];
            m_tail = (m_tail + 1) % CAPACITY;
            m_count--;
            return true;
        }
    }

    bool isFull()  const { return m_count == CAPACITY; }
    bool isEmpty() const { return m_count == 0; }
    int  count()   const { return m_count; }
};

int main() {
    CircularBuffer buf;

    // write 8 items — should fill it
    for (int i = 0; i < 8; i++) {
        buf.write(i);
    }
    std::cout << "full: " << buf.isFull() << std::endl;

    // try writing when full — should return false
    std::cout << "write when full: " << buf.write(99) << std::endl;

    // read all items — should come out in order
    uint8_t val;
    while (!buf.isEmpty()) {
        buf.read(val);
        std::cout << (int)val << std::endl;
    }

    std::cout << "empty: " << buf.isEmpty() << std::endl;

    // read when empty — should return false
    std::cout << "read when empty: " << buf.read(val) << std::endl;

    // write 5 items
    for (int i = 0; i < 5; i++) {
        buf.write(i * 10);   // 0, 10, 20, 30, 40
    }

    // read 3 items
    buf.read(val); std::cout << (int)val << std::endl;   // 0
    buf.read(val); std::cout << (int)val << std::endl;   // 10
    buf.read(val); std::cout << (int)val << std::endl;   // 20

    // write 6 more — this forces the write head to wrap around
    for (int i = 0; i < 6; i++) {
        buf.write(50 + i);   // 50, 51, 52, 53, 54, 55
    }

    std::cout << "count: " << buf.count() << std::endl;  // should be 8 (5-3+6)

    // read everything out in order
    while (!buf.isEmpty()) {
        buf.read(val);
        std::cout << (int)val << std::endl;
}
}