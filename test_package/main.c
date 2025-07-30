#include "allocator.h"
#include <stdint.h>

int main() {
    uint8_t memory[128];
    Allocator allocator;
    initAllocator(&allocator, 16, memory, 128);
    return 0;
}