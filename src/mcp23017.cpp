#include "mcp23017.h"


#include "hardware/gpio.h"
#include "hardware/i2c.h"



#define MCP23017_ADDRESS 0x20

#define I2C_GPIO_PIN_SDA 26
#define I2C_GPIO_PIN_SLC 27

#define I2C_INSTANCE i2c1



MCP23017::MCP23017():initOK(false){

    i2c_init(I2C_INSTANCE, 400000);
	gpio_set_function(I2C_GPIO_PIN_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_GPIO_PIN_SLC, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_GPIO_PIN_SDA);
	gpio_pull_up(I2C_GPIO_PIN_SLC);


    init();
}

bool MCP23017::setOutput(uint64_t outputs){
    if(!initOK){
        init();
        if(!initOK)
            return false;
    }
    
    if(write_dual_registers(0x12, outputs) !=0){//zapis vystupů do bran
        return false;
    }
    return true;
}



void MCP23017::init(){
    //write_register(0x0A,0x00);
    int result=write_dual_registers(0x00, 0x0000);//nastaveni GPIO jako OUTPUT
    if(result !=0){
        printf("MCP23017 init error %d\n");
        initOK=false;
    }
    initOK=true;
    setOutput(0x0000);
    printf("MCP23017 init OK\n");
}


int MCP23017::write_register(uint8_t reg, uint8_t value) const{
    uint8_t command[] = { reg, value };
	int result = i2c_write_blocking(I2C_INSTANCE, MCP23017_ADDRESS, command, 2, false);
	if (result == PICO_ERROR_GENERIC) {
		return result;
	}
	return PICO_ERROR_NONE;
}


int MCP23017::write_dual_registers(uint8_t reg, int value) const{
    uint8_t command[] = {
			reg,
			static_cast<uint8_t>(value & 0xff),
			static_cast<uint8_t>((value>>8) & 0xff)
	};
    printf("MCP write 0x%X\n",value);
	int result = i2c_write_blocking(I2C_INSTANCE, MCP23017_ADDRESS, command, 3, false);
    
	if (result == PICO_ERROR_GENERIC) {
		return result;
	}
	return PICO_ERROR_NONE;
}
