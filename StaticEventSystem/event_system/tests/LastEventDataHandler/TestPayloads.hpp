#pragma once
#include <string>
#include <cstdint>

// Payload type 1: Only uint8_t
struct PayloadU8 {
    uint8_t value;
};

// Payload type 2: uint16_t and int
struct PayloadU16Int {
    uint16_t u16;
    int i32;
};

// Payload type 3: std::string
struct PayloadString {
    std::string str;
};

// Payload type 4: bool and int
struct PayloadBoolInt {
    bool flag;
    int number;
};

// Payload type 5: uint8_t, std::string, bool
struct PayloadMixed {
    uint8_t u8;
    std::string name;
    bool valid;
};
