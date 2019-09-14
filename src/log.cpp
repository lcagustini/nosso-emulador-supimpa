#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <bitset>

using namespace std;

extern "C" {

    void print(uint8_t a, uint8_t x, uint8_t y, uint8_t sp, uint16_t pc, uint8_t p) {
        cout << setfill('0')
            << "| pc = 0x" << hex << setw(4) << pc
            << " | a = 0x" << hex << setw(2) << (unsigned) a
            << " | x = 0x" << hex << setw(2) << (unsigned) x
            << " | y = 0x" << hex << setw(2) << (unsigned) y
            << " | sp = 0x01" << hex << setw(2) << (unsigned) sp
            << " | p[NV-BDIZC] = " << bitset<8>(p) << " |" << endl;
    }

    void printls(uint8_t a, uint8_t x, uint8_t y, uint8_t sp, uint16_t pc, uint8_t p, uint16_t addr, uint8_t data) {
        cout << setfill('0')
            << "| pc = 0x" << hex << setw(4) << pc
            << " | a = 0x" << hex << setw(2) << (unsigned) a
            << " | x = 0x" << hex << setw(2) << (unsigned) x
            << " | y = 0x" << hex << setw(2) << (unsigned) y
            << " | sp = 0x01" << hex << setw(2) << (unsigned) sp
            << " | p[NV-BDIZC] = " << bitset<8>(p)
            << " | MEM[0x" << hex << setw(4) << addr
            << "] = 0x" << hex << setw(2) << (unsigned) data << " |" << endl;
    }

}
