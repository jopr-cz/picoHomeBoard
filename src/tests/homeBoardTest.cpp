#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <chrono>
#include <sys/time.h>


#include "jolibMainHelper.h"
#include "gpio_base.h"
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
    uint16_t getBtnInputState()override final{
        return gpio_in;
    }
    bool setGPIO(int number, bool status)override final{
        return true;
    }
};


struct GPIO_SETTING{
    int gpio2set;
    int timeMS;
};

struct ZALUZ_STATE_SETTING{
    ZALUZ::ZALUZ_STATE newPosition;
    unsigned int timeMS;
    bool processed;//toto je pro uziti testovaci funkce (aby nevolala dokola setState)
    ZALUZ_STATE_SETTING(ZALUZ::ZALUZ_STATE new_Pos,unsigned int timeMS):
        newPosition(new_Pos),
        timeMS(timeMS),
        processed(false)
    {

    }
};

struct TestBtnData {
    int expectedZalusPosition;
    int expectedShutterPosition;
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

    static const ZALUZ_SETTING zalSeting[] = {
        {4000000,1000000},        ///< zaluzie 1
        {4000000,1000000},
        {4000000,1000000}
    };

    const int ZaluzIndex=0;//index zaluzie na ktery budu brat dotazy 

    ZALUZIE zaluzie(&gpio, zalSeting,ZaluzIndex+1);
    HomeBoard homeBoard(&zaluzie, &gpio, &serial);

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

        timestampUs=timestampUs*100;//zrychlení času

        timestamMS=timestampUs/1000;
        modul_helper.loop(timestampUs);

        for(auto gp:data.gpios){
            if(timestamMS==gp.timeMS){
                if(gpio.gpio_in!=gp.gpio2set)
                    printf("Gpio tlacitko %d\n",gp.gpio2set);
                gpio.gpio_in=gp.gpio2set;
            }
        }

        for(auto& state:data.zaluz_state){
            if(timestamMS==state.timeMS && state.processed==false){
                //zaluzie.setPosition(state.newPosition,ZaluzIndex);
                zaluzie.setState(state.newPosition,ZaluzIndex);
                state.processed=true;
            }
        }

        if(timestamMS/1000>=data.totalTimeoutS)
            break;

    }

    EXPECT_GE(data.expectedZalusPosition+1,zaluzie.getZaluzPosition(ZaluzIndex));
    EXPECT_LE(data.expectedZalusPosition-1,zaluzie.getZaluzPosition(ZaluzIndex));

    if(data.expectedShutterPosition>=0){
        EXPECT_GE(data.expectedShutterPosition+1,zaluzie.getShutterPosition(ZaluzIndex));
        EXPECT_LE(data.expectedShutterPosition-1,zaluzie.getShutterPosition(ZaluzIndex));
    }

}



INSTANTIATE_TEST_SUITE_P(InlineValues, HomeBoardParamTest, testing::Values(
    //TL - 0x0004 - dolu    -zaluzie index 1
    //TL - 0x0008 - nahoru  -zaluzie index 1
    //TL - 0x0001 - dolu    -zaluzie index 0
    //TL - 0x0002 - nahoru  -zaluzie index 0
    ////////pozice, shutterPos, totalTimout[S],{gpio, time},{zaluzState, time}
    TestBtnData{0,     -1,     10, {{0x2, 100}, {2, 200}},{}},
    TestBtnData{100,   -1,     10, {{0x1, 2000}},{}},
    TestBtnData{0,     -1,     10, {{0x1, 1000}, {0x2, 5000} },{}},
    TestBtnData{75,    -1,     11, {{0x1, 2000}, {0x2, 8000},{0x0, 10000}},{}},
    TestBtnData{100,   -1,     10, {}, {{ZALUZ::CLOSE,3000}}},
    TestBtnData{0,     -1,     10, {}, {{ZALUZ::CLOSE_LIGHT,1000},{ZALUZ::OPEN,5000}}},

    //Test naklonu zaluzii
    TestBtnData{100,   0,       8, {}, {{ZALUZ::CLOSE,100}}},
    TestBtnData{100,   100,     8, {}, {{ZALUZ::CLOSE_LIGHT,100}}},


    //Test shutteru
    TestBtnData{100,   0,   8, {{0x1, 100} },{}},
    TestBtnData{0,   100,  12, {{0x1, 1000}, {0x2, 5000} },{}},
    TestBtnData{100,   50,  12, {{0x1, 100}, {0x2, 7000} , {0x0, 7500} },{}},//sjedu dolu a pak kliknu na natočení žaluzii

    TestBtnData{0,     -1,     10, {{0x2, 100}, {2, 200}},{}}
));







int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
