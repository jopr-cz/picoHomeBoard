#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <chrono>
#include <sys/time.h>


#include "jolibMainHelper.h"
#include "gpio_base.h"

#include "../zaluzie.h"


class GPIO_TEST:public GPIO_BASE
{
public:
    GPIO_TEST(/* args */):GPIO_BASE(){
        gpio_in=0;
        for(int i=0;i<16;i++){
            lastChange.push_back(0);
        }
    };
    virtual ~GPIO_TEST(){};
    uint16_t gpio_in;
protected:
    uint16_t getBtnInputState()override final{
        return gpio_in;
    }

    std::vector<uint64_t> lastChange;//gpio zmena

    bool setGPIO(int number, bool status)override final{
        if(number>=lastChange.size()){
            printf("Out of gpio %d\n",number);
            set_error_code(1);
            return true;
        }
        uint64_t diff=timestamp-lastChange[number];
        printf("OUTPUT %i -> %i set time: %u ms  diffchange:%lu\n",number,status,timestampMS,diff/1000);
        if(diff<ZALUZ::motorTotalDelayTime && status==true){
            printf("Nedodrzen cas na vypnuti motoru gpio:%d diff %lu ms\n",number,diff/1000);
            set_error_code(2);
        }
        lastChange[number]=timestamp;

        return true;
    }
};


struct GPIO_SETTING{
    int gpio2set;
    unsigned int timeMS;
    bool processed;
    GPIO_SETTING(int gpio_Set,unsigned int time_MS):
        gpio2set(gpio_Set),
        timeMS(time_MS),
        processed(false)
    {}
};

struct ZALUZ_STATE_SETTING{
    uint8_t newPosition;
    unsigned int timeMS;
    bool processed;//toto je pro uziti testovaci funkce (aby nevolala dokola setState)
    ZALUZ_STATE_SETTING(int new_Pos,unsigned int timeMS):
        newPosition(new_Pos),
        timeMS(timeMS),
        processed(false)
    {

    }
};

struct TestBtnData {
    int expectedZalusPosition;
    int expectedShutterPosition;
    unsigned int totalTimeoutS;
    std::vector<GPIO_SETTING> gpios;
    std::vector<ZALUZ_STATE_SETTING> zaluz_state;
};
 
class ZaluzieBTNParameterizedTest : public testing::TestWithParam<TestBtnData> {
};

TEST_P(ZaluzieBTNParameterizedTest, ZaluzieTestTlacitek){
    static const ZALUZ_SETTING zaluzSettingArray[] = {
        {4000000,1000000},
        {4000000,1000000},
        {4000000,1000000}
    };
    const int ZaluzIndex=0;//index zaluzie na ktery budu brat dotazy

    MAIN_HELPER modul_helper;
    GPIO_TEST gpio;
    modul_helper.addModul(&gpio);
    ZALUZIE zaluzie(&gpio,zaluzSettingArray,ZaluzIndex+1);
    modul_helper.addModul(&zaluzie);
    
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

        for(auto& gp:data.gpios){
            if(timestamMS>gp.timeMS && gp.processed==false){
                gpio.gpio_in=gp.gpio2set;
                gp.processed=true;
            }
        }

        for(auto& state:data.zaluz_state){
            if(timestamMS==state.timeMS && state.processed==false){
                zaluzie.setPosition(state.newPosition,ZaluzIndex);
                state.processed=true;
            }
        }

        if(timestamMS/1000>=data.totalTimeoutS)
            break;

        if(gpio.errorCode()!=0){
            EXPECT_EQ(0,gpio.errorCode());
            break;
        }
    }

    EXPECT_GE(data.expectedZalusPosition+1,zaluzie.getZaluzPosition(ZaluzIndex));
    EXPECT_LE(data.expectedZalusPosition-1,zaluzie.getZaluzPosition(ZaluzIndex));

    if(data.expectedShutterPosition>=0){
        EXPECT_GE(data.expectedShutterPosition+1,zaluzie.getShutterPosition(ZaluzIndex));
        EXPECT_LE(data.expectedShutterPosition-1,zaluzie.getShutterPosition(ZaluzIndex));
    }
}


INSTANTIATE_TEST_SUITE_P(InlineValues, ZaluzieBTNParameterizedTest, testing::Values(
    //TL - 0x0004 - dolu    -zaluzie index 1
    //TL - 0x0008 - nahoru  -zaluzie index 1
    //TL - 0x0001 - dolu    -zaluzie index 0
    //TL - 0x0002 - nahoru  -zaluzie index 0
    //pozice-test, shutterPos, totalTimout[S],{gpio, time},{zaluzPosition, time}
    TestBtnData{0,   0,    10, {{0x2, 100}, {2, 200}}, {}},
    TestBtnData{100, 100,  10, {{0x1, 2000}},{}},
    TestBtnData{75,   0,   10, {{0x1, 1000}, {0x2, 7000},{0x0, 9000}},{}},
    TestBtnData{100, 100,  10, {}, {{100,3000}}},
    TestBtnData{80,   0,   10, {}, {{80,3000}}},
    TestBtnData{0,    0,   10, {}, {{80,1000},{0,5000}}},
    TestBtnData{100, 100,   10, {}, {{80,1000},{100,6000}}},
    TestBtnData{0,    0,   12, {{0x1, 100}, {0x2, 6000}},{}},//uplně dolů a pak uplně nahoru
    TestBtnData{100,  0,   12, {{0x1, 100}, {0x2, 7000}, {0x0, 8000}},{}},//žaluzie zataženy ale na průduch
    TestBtnData{100,  50,  12, {{0x1, 100}, {0x2, 7000}, {0x1, 7500} ,{0x0, 8000}},{}},//rychle kliknuti, nahoru dolu

    //  DOUBLE CLICK test:
    TestBtnData{100,  100,  10, {{0x1, 1000}, {0x0, 1200}, {0x1, 1400} ,{0x0, 2000}},{}},//dvojklid dolu

    TestBtnData{100,   -1,   10, {{0x2, 100}, {0x1, 300}},{}} //zavřít žaluzie 
));





TEST(ZaluzieBtnRandomTest, ZaluzieTestTlacitek){
    //Test pro náhodné klikání na tlačítka
    //nekontroluje se pozice žaluzie, ale jen jestli se neobjeví chyba na motoru

    static const ZALUZ_SETTING zaluzSettingArray[] = {
        {4000000,1000000},
        {4000000,1000000},
        {4000000,1000000}
    };

    const int ZaluzIndex=0;//index zaluzie na ktery budu brat dotazy
    const unsigned int totalTimeoutS=1000;// jak dlouho test bude trvat v sekundach  [v sekundach]
    MAIN_HELPER modul_helper;
    GPIO_TEST gpio;
    modul_helper.addModul(&gpio);
    ZALUZIE zaluzie(&gpio,zaluzSettingArray,ZaluzIndex+1);
    modul_helper.addModul(&zaluzie);
    

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    long long timestampUs;
    uint32_t timestamMS;
    gpio.gpio_in=0x0000;
    bool test_pass=false;
    printf("Start TEST\n");

    while(true){
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        timestampUs= (end_time.tv_sec - start_time.tv_sec) * 1000000LL + (end_time.tv_nsec - start_time.tv_nsec) / 1000LL;

        timestampUs=timestampUs*100;//zrychlení času

        timestamMS=timestampUs/1000;
        modul_helper.loop(timestampUs);

        // náhodné klikání na tlačítka
        if(timestamMS%100==0 && rand() % 5 == 0){//každou 100ms s pravděpodobností 1/5
            //int btnTypeDown=rand()%2;

            int btn = rand() & 0x0f;//náhodná kombinace tlačítek 0x00-0xff
            if(rand() % 5 == 0)//jkeště vypnutí má vyšší pravděpodobnost
                btn=0x00;
            gpio.gpio_in = btn;

        }


        if(timestamMS/1000>=totalTimeoutS)
            break;

        if(gpio.errorCode()!=0){
            EXPECT_EQ(0,gpio.errorCode());
            break;
        }
    }

}



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
