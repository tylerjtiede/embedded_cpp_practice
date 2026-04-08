#include <iostream>
#include <cstdint>
#include <cassert>

// print a number as 8-bit binary so we can see what's happening
void printBits(uint8_t val) {
    for (int i = 7; i>= 0; i--) {
        std::cout << ((val >> i) & 1); // shift right by i and mask with 1 to get the bit
    }
    std::cout << " (" << (int)val << ")" << std::endl; // also print the decimal value for clarity
}

// set a specific bit in a register
void setBit(uint8_t& reg, int bit) {
    (reg |= (1 << bit));
}

// clear a specific bit in a register
void clearBit(uint8_t& reg, int bit) {
    (reg &= ~(1 << bit));
}

// return true if a specific bit is set
bool isBitSet(uint8_t reg, int bit) {
    return (reg & (1 << bit));
}

// set a field of `width` bits starting at `startBit` to `value`
// example: setField(reg, 4, 2, 0b1011)
// sets bits 5-2 to 1011 without touching bits 7-6 or bits 1-0
void setField(uint8_t& reg, int startBit, int width, uint8_t value) {
    assert(!(value >> width));
    int mask = (1 << width) - 1;
    reg &= ~(mask << startBit);
    reg |= (value << startBit);
}

int main() {
    uint8_t reg = 0b00000000; // start with all bits cleared

    // set bit 3
    reg |= (1 << 3); // set bit 3 by ORing with 0b00001000
    printBits(reg);

    // set bit 5
    reg |= (1 << 5);
    printBits(reg);

    // clear bit 3
    reg &= ~(1 << 3);
    printBits(reg);

    // toggle bit 5
    reg ^= (1 << 5);
    printBits(reg);

    // read bit 3 - is it set?
    if (reg & (1 << 3)) {
        std::cout << "bit 3 is set" << std::endl;
    }
    else {
        std::cout << "bit 3 is clear" << std::endl;
    }

    setBit(reg, 2);
    printBits(reg);

    clearBit(reg, 2);
    printBits(reg);

    bool ret = isBitSet(reg, 2);
    std::cout << ret << std::endl;

    uint8_t reg2 = 0b10110101;
    uint8_t upper = (reg2 >> 4) & 0x0F;
    uint8_t lower = reg2 & 0x0F;

    std::cout << "upper nibble: ";
    printBits(upper);
    std::cout << "lower nibble: ";
    printBits(lower);

    // pack them back together
    uint8_t packed = (upper << 4) | lower;
    std::cout << "packed: ";
    printBits(packed);

    uint8_t reg3 = 0b00000000;
    setField(reg3, 2, 4, 0b1011);  // set bits 5-2 to 1011
    printBits(reg3);               // should show 00101100


    uint8_t status = 0b10110100;

    // check if bits 4 AND 2 are both set
    uint8_t mask = (1 << 4) | (1 << 2);   // 0b00010100
    if ((status & mask) == mask) {
        std::cout << "both bits set" << std::endl;
    }

    // check if ANY of bits 4, 2, or 1 are set
    if (status & ((1 << 4) | (1 << 2) | (1 << 1))) {
        std::cout << "at least one bit set" << std::endl;
    }

    return 0;
}