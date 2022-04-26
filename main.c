
#include "bme680_spi.h"   //Examples
#include "sensorData.h"
#include "DS1307.h"
#include "Gui.h"
#include "image.h"
#include "gfxfont.h"
#include "times_ro32pt7b.h"
enum direction directionText;
enum ScreenRotation ScreenRotation;

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
        test += 1;
        //setupBME();
        //updateBME(&bme);
        //printf("%f\n",bme.pressure);
       // printf("%f\n",bme.humidity);
        //printf("%f\n",bme.gas);
       // printf("%f\n",bme.temp);
        //updateDS(&DS1307);

	//}
    printf("test\n");
    initializeDisplay(480, 800);
    char car = 'a';
    paintSolid(white);
    show(1);
    uint16_t number = 100;
    test = 0; 
    uint16_t x1, x2, y1, y2, x3, y3;
    x1 = 20;
    x2 = 335;
    y1 = 300;
    y2 = 467;

    x3 = 300;
    y3 = 200;
    while(true)
    {
        printf("test");
    drawString("Donderdag", x1,y1, &times_ro32pt7b, horizontal,right);
    drawString("VochtigheiD", x2,y2,&times_ro32pt7b,vertical,right);
    drawString("Datum", x3, y3, &times_ro32pt7b, horizontal, right);
    show(1);
    }


    // while (true)
    // {
    //   drawString("Hallo Maaike is dit cool?", 0,400, times_ro32pt7b, vertical,white);
    //   for(int i =0; i < 800; i++)
    //   {
    //     setPixel(black,i,200);
    //   }

    //   drawNumber(number,0,100,times_ro32pt7b,vertical,white);
    //   number++;
    //   test % 5 == 0 ? show(false): show(true);
    //   paintSolid(white);
    //   test+=1;
    // }
    



    
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
