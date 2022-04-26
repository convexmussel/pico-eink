
#include "bme680_spi.h"   //Examples
#include "sensorData.h"
#include "DS1307.h"
#include "Gui.h"
#include "image.h"
#include "gfxfont.h"
#include "times_ro32pt7b.h"
#include "times_ro28pt7b.h"
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
    uint16_t x1, x2, y1, y2, x3, y3,x4,y4,x5,y5,x6,y6;
    x1 = 20;
    x2 = 337;
    y1 = 300;
    y2 = 450;

    x3 = 294;
    y3 = 196;

    x4= 520;
    y4 = 200;
    x5 = 433;
    y5=445;

    x6 = 433;
    y6= 433;
    
    char *hello_world = (char*)malloc(15 * sizeof(char));
    char *temp = (char*)malloc(20 * sizeof(char));
    // Prints "Hello world!" on hello_world
    
    while(true)
    {
        updateBME(&bme);
    sprintf(hello_world, "vochtigheid: %.0f%%", bme.humidity);
    sprintf(temp, "temperatuur: %.1f", bme.temp);
    drawString("donderdag", x1,y1, &times_ro32pt7b, horizontal,right);
    drawString(hello_world, x2,y2,&times_ro32pt7b,vertical,right);
    drawString("datum", x3, y3, &times_ro32pt7b, horizontal, right);
    drawString("13 maart\n2021",x4, y4, &times_ro28pt7b, vertical, right);
    drawString(temp,x5,y5,&times_ro32pt7b,vertical,right);
    drawString("tijd:",x6,y6, &times_ro32pt7b, horizontal, right);


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
