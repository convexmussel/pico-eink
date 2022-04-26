#include "Gui.h"
#include "image.h"
//struct holding the buffer info
Display display;
enum colors textColor;
enum direction textDirection;
enum ScreenRotation ScreenRotation2;

void initializeDisplay(uint16_t height, uint16_t width)
{
    //allocate the memory for the buffer
    //buffer is structered row and then column
    uint8_t buffer[height * width];
    //set the display struct variables
    display.image = (uint8_t *) malloc(height * (width/8) + 1);
    display.height = height;
    display.width = width;

    DEV_Module_Init();
    // //iniatlise the display
    EPD_7IN5_V2_Init();

    EPD_7IN5_V2_Clear();
    DEV_Delay_ms(500);
    // //wait 50 ms for the screen to finish initilizing 
    sleep_ms(50);
    // //send the power off command to the display 
    EPD_7IN5_V2_Sleep();
}

void drawNumber(const uint16_t number,  uint16_t x, uint16_t y, const GFXfont *font, bool direction, uint16_t textRotation)
{
    char buf[20];
    itoa(number, buf, 10);
    drawString(buf, x, y, font, direction,textRotation);
}

void drawString(const char* text, uint16_t x, uint16_t y, const GFXfont *font, bool direction, uint16_t textRotation)
{
    uint16_t advanceX, advancey;
    int x_distance, y_distance;
    GFXglyph *glyph;
    while(*text != '\0')
    {
        if(*text == '\n')
        {
            GFXglyph *glyph = &font->glyph['A' - font->first];
            advanceX = font->yAdvance + abs(glyph->yOffset);
            printf("%d\n", glyph->yOffset);
            advancey =0;
            text++;
            continue;
        }
        GFXglyph *glyph = &font->glyph[*text - font->first];
        x_distance = glyph->xOffset;
        y_distance = abs(glyph->yOffset);
        
        
        if(direction == horizontal)
        {
            if(textRotation == normal)
            {
                drawChar(text, x+advanceX+x_distance,y-y_distance, font,horizontal, textRotation);
                advanceX += glyph->xAdvance;
            }
            else if (textRotation = right)
            {
                drawChar(text, x+advanceX ,y-x_distance, font,horizontal, textRotation);
                advanceX += y_distance + (glyph->xAdvance - glyph->width);
            }
            
        }
        else
        {
            if(textRotation == normal)
            {
                drawChar(text, x + x_distance,y+advancey , font,horizontal, textRotation);
                advancey += font->yAdvance + y_distance;
            }
            else if (textRotation = right)
            {
                drawChar(text, x - y_distance + advanceX,y-advancey-x_distance, font,horizontal, textRotation);
                advancey += glyph->xAdvance;
            }

        }
        text++;
    }
}

void drawChar(const char *character, uint16_t x, uint16_t y, const GFXfont *font, bool direction, uint16_t textRotation)
{
    //check if the character exist in font
    if(*character < font->first || *character > font->last)
    {
        printf("Character not in font");
    }

    //get the bitmap
    GFXglyph *glyph = &font->glyph[*character - font->first];
    uint8_t *bitmap = font->bitmap;
    int16_t x_offset,y_offset;
    uint16_t byteCounter = 0;

    //send the data into the buffer
    for(uint16_t i = 0; i < (glyph->width * glyph->height); i++)
    {

        uint16_t bit = i % 8;
        uint16_t byte = i / 8;


        if(direction == horizontal)
        {
            if(textRotation == right)
            {
                if(i % glyph->width == 0){
                    y_offset = 0; 
                    x_offset += 1;
                }
            }
            else
            {
                if(i % glyph->width == 0){
                    x_offset = 0; 
                    y_offset += 1;
                }
            }

        }
        else
        {
            if(i % glyph->width == 0){
                y_offset = 0; 
                x_offset += 1;
            } 
        }
        //set the data to the array
        (bitmap[glyph->bitmapOffset + byte] >> (7-bit)) & 1 ? setPixel(black, x+x_offset, y+y_offset) : setPixel(white, x+x_offset, y+y_offset);

        if(direction == horizontal)
        {
            switch (textRotation)
            {
            case normal: x_offset++; break;   
            case right: y_offset--; break;
            default: break;
            } textRotation;
        }
        else
        {
             switch (textRotation)
            {
            case normal: y_offset--; break;   
            case right: y_offset++; break;
            default: break;
            } textRotation;
        }
        
        //increment the offset
        //direction == horizontal ? x_offset++ : y_offset--;
    }
}

//function to set the color of the pixel
// 0 = black
// 1 = white
void setPixel(uint8_t color, uint16_t x, uint16_t y)
{
    if(x > display.width || y > display.height)
    {
        printf("Adress out of range, is the display initilized?");
    }
    //calculate x byte by defiding the x number by the number of bits in an unsinged byte and add the y number of bits to that number
    uint16_t byte = (x/8) + (y * (display.width/8));
    //determine what bit in the byte needs to be set
    uint16_t x_bit = (7-x%8);
    
    //set the color bit
    if(color == black)
    {
        display.image[byte] = display.image[byte] & (~(1 << x_bit));
    }
    else
    {
        display.image[byte] = display.image[byte] | (color << x_bit);
    }
    //printf("%d", (display.image[byte] >>x_bit) & 1);
}

//paint the image buffer a solid color
void paintSolid(uint8_t color)
{
    //loop through all the bytes in the buffer and set them to black
    for(int i =0; i < (display.height * (display.width/8)); i++)
    {
        display.image[i] = (color * 255);
    }
}

//Function that publishes the buffer to the eink display; 
void show(bool speed)
{
    EPD_7IN5_V2_Init();
    speed? EPD_SET_LUT_FAST() : EPD_SET_LUT_SLOW();
    EPD_7IN5_V2_Clear();
    EPD_7IN5_V2_Display(display.image);
    printf("Goto Sleep...\r\n");
    EPD_7IN5_V2_Sleep();
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
}