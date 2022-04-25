#include "Gui.h"
#include "image.h"
//struct holding the buffer info
Display display;
enum colors textColor;
enum direction textDirection;
void initializeDisplay(uint16_t height, uint16_t width)
{
    //allocate the memory for the buffer
    //buffer is structered row and then column
    uint8_t buffer[height * width];
    //set the display struct variables
    display.image = (uint8_t *) malloc(height * (width/8));
    display.height = height;
    display.width = width;
}

void drawString(const char* text, uint16_t x, uint16_t y, const font* font, bool direction, uint8_t spacing)
{
    uint16_t offset;
    uint8_t charIndex;
    //check if character is kown in font
    for(uint8_t i = 0; i < font->lenght; i++)
    {
        if(*text == font->lookup[i])
        {
            charIndex = i;
        }
    }

    //check if the file has ended
    while(*text != '\0')
    {
        direction == vertical ? drawChar(text, x, y + offset, font, direction, black) : drawChar(text, x + offset, y, font, direction, black);
        offset += font->width[charIndex] + spacing;
        text++;
    }
}

void drawChar(const char *character, uint16_t x, uint16_t y, const font* font, uint8_t direction, uint8_t color)
{
    uint8_t charIndex;
    const char *bitmap;
    uint16_t x_offset, y_offset;
    //check if character is kown in font
    for(uint8_t i = 0; i < font->lenght; i++)
    {
        if(*character == font->lookup[i])
        {
            charIndex = i;
            bitmap = font->char_addr[i];
        }
    }
    
    //get the data from the font file
    for(uint8_t q =0; q<(font->width[charIndex]); q++)
    {
        //loop through the y bytes
        for(uint8_t z =0; z < font->byte_count; z++)
        {
            //set each pixel to the correct color
            for(int8_t i =0; i < 8; i++){
                //set the appropriate pixel
                setPixel((bitmap[q * font->byte_count + z] >> i) & 1, x+x_offset, y+y_offset);
                y_offset += 1;
                            
                }
        }
        //incrament the y offset
        x_offset += 1;
        //reset the y_offset after moving the x offset up
        y_offset = 0;
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
    uint16_t byte = (y/8) + (x * (display.height/8));
    //determine what bit in the byte needs to be set
    uint8_t y_bit = y%8;  
    
    //set the color bit
    if(color == black)
    {
        display.image[byte] = display.image[byte] & ((1 << y_bit));
    }
    else
    {
        display.image[byte] = display.image[byte] | (color << y_bit);
    }
}

//paint the image buffer a solid color
void paintSolid(uint8_t color)
{
    //loop through all the bytes in the buffer and set them to black
    for(int i; i < (display.height * (display.width/8)); i++)
    {
        display.image[i] = (color * 255);
    }
}
