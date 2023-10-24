#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <chrono>
#include <sys/time.h>


#include "jolibMainHelper.h"
#include "gpio.h"
#include "serial_linuxTCP.h"
#include "../zaluzie.h"
#include "../homeBoard.h"


class GPIO_TEST:public GPIO_BASE
{
public:
    GPIO_TEST(/* args */):GPIO_BASE(){
        gpio_in=0;
    };
    virtual ~GPIO_TEST(){};
    uint16_t gpio_in;
protected:
    uint16_t getBtnInputState()override{
        return gpio_in;
    }
};


struct GPIO_SETTING{
    int gpio2set;
    int timeMS;
};

struct ZALUZ_STATE_SETTING{
    int newPosition;
    int timeMS;
    bool processed;//toto je pro uziti testovaci funkce (aby nevolala dokola setState)
};

struct TestBtnData {
    int expectedZalusPosition;
    int totalTimeoutS;
    std::vector<GPIO_SETTING> gpios;
    std::vector<ZALUZ_STATE_SETTING> zaluz_state;
};
 
class HomeBoardParamTest : public testing::TestWithParam<TestBtnData> {
};

TEST_P(HomeBoardParamTest, HomeBoardParamTest){
    MAIN_HELPER modul_helper;
    GPIO_TEST gpio;
    SerialLinuxTCP serial;

    ZALUZIE zaluzie(&gpio);
    HomeBoard homeBoard(&zaluzie, 0x02,&serial);

    modul_helper.addModul(&gpio);
    modul_helper.addModul(&zaluzie);
    modul_helper.addModul(&homeBoard);

    
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    long long timestampUs;
    uint32_t timestamMS;
    gpio.gpio_in=0x0000;
    bool test_pass=false;
    TestBtnData data=GetParam();
    printf("Start TEST\n");

    for(auto& state:data.zaluz_state){
        state.processed=false;
    }

    while(true){
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        timestampUs= (end_time.tv_sec - start_time.tv_sec) * 1000000LL + (end_time.tv_nsec - start_time.tv_nsec) / 1000LL;
        timestamMS=timestampUs/1000;
        modul_helper.loop(timestampUs);

        for(auto gp:data.gpios){
            if(timestamMS==gp.timeMS)
                gpio.gpio_in=gp.gpio2set;
        }

        for(auto& state:data.zaluz_state){
            if(timestamMS==state.timeMS && state.processed==false){
                zaluzie.setPosition(state.newPosition,1);
                state.processed=true;
            }
        }

        if(timestamMS/1000>=data.totalTimeoutS)
            break;

    }
    EXPECT_EQ(data.expectedZalusPosition,zaluzie.getZaluzPosition(1));
}


INSTANTIATE_TEST_SUITE_P(InlineValues, HomeBoardParamTest, testing::Values(
    ////////pozice, totalTimout[S],{gpio, time},{zaluzState, time}
    TestBtnData{0,      10, {{0x008, 100}, {2, 200}}},
    TestBtnData{100,    10, {{0x0004, 2000}}},
    TestBtnData{80,     10, {{0x0004, 2000}, {0x0008, 8000},{0x0000, 9000}}},
    TestBtnData{100,    10, {}, {{ZALUZ::CLOSE,3000}}},
    TestBtnData{80,     10, {}, {{ZALUZ::CLOSE_LIGHT,3000}}},
    TestBtnData{0,      10, {}, {{ZALUZ::CLOSE_LIGHT,1000},{ZALUZ::OPEN,6000}}},
    TestBtnData{100,    10, {}, {{ZALUZ::CLOSE_LIGHT,1000},{ZALUZ::CLOSE,6000}}}
));







int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}