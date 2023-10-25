#ifndef HEADER_H_MCP23017     // equivalently, #if !defined HEADER_H_
#define HEADER_H_MCP23017

#include "jolib.h"


class MCP23017{
public:
    MCP23017();

    bool setOutput(uint64_t outputs);

protected:
    bool initOK;


    void init();

    int write_register(uint8_t reg, uint8_t value) const;
    int write_dual_registers(uint8_t reg, int value) const;

};


#endif