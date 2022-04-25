
#include "bme680_spi.h"   //Examples
#include "sensorData.h"
#include "DS1307.h"
#include "Gui.h"
#include "image.h"
#include "Open_Sans_Regular_32.h"

enum direction directionText;
int main(void)
{
    struct sensorData bme = {};
    struct timeData DS1307 = {};
    initDS();
    setupBME();
    int32_t test  = 987654321; 
    //initDisplay();
	//while(1) {
        //DEV_Delay_ms(500);
		//update(test);
        //test += 1;
        //setupBME();
        //updateBME(&bme);
        //printf("%f\n",bme.pressure);
       // printf("%f\n",bme.humidity);
        //printf("%f\n",bme.gas);
       // printf("%f\n",bme.temp);
        //updateDS(&DS1307);

	//}
    printf("test\n");
    initializeDisplay(800, 480);
    char car = '1';
    drawChar(&car,0,0,&Open_Sans_Regular_32, vertical, black);

	// EPD_2in9_V2_test();
    // EPD_2in9bc_test();
    // EPD_2in9b_V3_test();
    // EPD_2in9d_test();

    // EPD_2in13_V2_test();
	// EPD_2in13_V3_test();
    // EPD_2in13bc_test();
    // EPD_2in13b_V3_test();
    // EPD_2in13d_test();
    
    // EPD_2in66_test();
    // EPD_2in66b_test();
    
    // EPD_2in7_test();

    // EPD_3in7_test();
	
	// EPD_4in2_test();
    // EPD_4in2b_V2_test();
    //EPD_5in65f_test();

    // EPD_5in83_V2_test();
    // EPD_5in83b_V2_test();

    //EPD_7in5_V2_test();
    // EPD_7in5b_V2_test();

    

    return 0;
}
