// This is the second Arduino program, which can be used once the calibration values are found.
// Please scroll down to the "void tare_scale(void)" method.
#include "U8glib.h"

#include "HX711.h"

volatile boolean TurnDetected;
volatile int Rotary_Flag = 0;
// Rotary Encoder Module connections
#define RotaryCLK 2
pin 2 of Arduino
#define RotaryDT 3
    pin 3
#define RotarySW 4
    to pin 4
// Rotary encoder CLK pin connected to
// Rotary encoder DT pin connected to
// Rotary encoder Switch pin connected
// once the 2 calibration values
// HX711 Module connections
#define CLK_1 5   // CLK of HX711 connected to pin 5 of Arduino -> Load Cell 1
#define DOUT_1 6  // DOUT of HX711 connected to pin 6 of Arduino -> Load Cell 1
#define CLK_2 12  // CLK of HX711 connected to pin 5 of Arduino -> Load Cell 2
#define DOUT_2 11 // DOUT of HX711 connected to pin 6 of Arduino -> Load Cell 2
    int reset_screen_counter = 0;
volatile int current_units = 0;
float unit_conversion;
int decimal_place;
HX711 scale_1(DOUT_1, CLK_1);                       // Init of the HX711 --> Load
Cell 1 HX711 scale_2(DOUT_2, CLK_2);                // Init of the HX711 --> Load
Cell 2 U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_FAST); // Init of the OLED screen
// Interrupt routine runs if Rotation detected from Rotary encoder
void rotarydetect()
{
    Rotary_Flag = 1; // Set Rotary flag from 0 to 1
    delay(500);
}
// Used to change the measurement units (0=grams, 1=KG,2=pounds)
void change_units()
{
    if (current_units == 0)
        current_units = 1;
    else if (current_units == 1)
        current_units = 2;
    else if (current_units == 2)
        current_units = 0;
}
// Run at Startup and when Resetting with Rotary encoder switch

void startupscreen(void)
{
    u8g.setFont(u8g_font_unifont);
    u8g.firstPage();
    do
    {
        u8g.drawStr(0, 10, "Clear Scale");
        u8g.drawStr(0, 28, "Click to zero...");
    } while (u8g.nextPage());
}
// Reset Scale to zero
void tare_scale(void)
{
    /* Here, the calibration values that are found with the other program are inputted. The "loadcell_1_calibration" is the calibration value for the load cell (and in turn the hx711) connected to the pins 5 & 6 on the arduino (load cell 1 on the schematic, the first value we found). The "loadcell_2_calibration" is the calibration value for the load cell (and in turn the hx711) connected to pins 11 & 12 on the arduino (load cell 2 on the schematic, the second value we found).
     */
    long loadcell_1_calibration = 103725;
    long loadcell_2_calibration = 110490;
    scale_1.set_scale(loadcell_1_calibration); // Calibration Factor obtained from calibration sketch --> Load Cell 1
    scale_1.tare();                            // Reset the scale to 0  --> Load Cell 1
    scale_2.set_scale(loadcell_2_calibration); // Calibration Factor obtained from calibration sketch --> Load Cell 2
    scale_2.tare();                            // Reset the scale to 0  --> Load Cell 2
}
// Start Displaying information on OLED
void start_scale(void)
{
    char temp_current_units[15];
    String KG = "KG";

    String GRAMS = "GRAMS";
    String LBS = "POUNDS";
    if (current_units == 0)
    {
        GRAMS.toCharArray(temp_current_units, 15);
        unit_conversion = 1000;
        decimal_place = 0;
    }
    else if (current_units == 1)
    {
        KG.toCharArray(temp_current_units, 15);
        unit_conversion = 1;
        decimal_place = 3;
    }
    else
    {
        LBS.toCharArray(temp_current_units, 15);
        unit_conversion = 2.2046226218;
        decimal_place = 3;
    }
    u8g.setFont(u8g_font_unifont);
    u8g.firstPage();
    do
    {
        u8g.drawStr(0, 10, temp_current_units);
        u8g.setPrintPos(38, 28);
        float final_mass;
        final_mass = scale_1.get_units(3) +
                     scale_2.get_units(3);
        u8g.print(final_mass * unit_conversion,
                  decimal_place);
    } while (u8g.nextPage());
}
void setup(void)
{
    // Set pinmode for Rotary encoder pins
    pinMode(RotarySW, INPUT_PULLUP);
    pinMode(RotaryCLK, INPUT_PULLUP);
    pinMode(RotaryDT, INPUT_PULLUP);

    attachInterrupt(0, rotarydetect, RISING);
    u8g.setRot180();
    u8g.setColorIndex(1);
    u8g.setFont(u8g_font_unifont);
    String start_count_string = "Starting up....";
    char start_count[15]; // Used to String to Char
    conversion for (int x = 12; x < 16; x++)
    {
        start_count_string.toCharArray(start_count, x);
        u8g.firstPage();
        do
        {
            u8g.drawStr(0, 10, "ARDUINO SCALE");
            u8g.drawStr(0, 28, start_count);
        } while (u8g.nextPage());
        delay(500); // Delay between dots
    }
}
void loop(void)
{
    if (!digitalRead(RotarySW))
    {
        if (reset_screen_counter == 1)
        {
            tare_scale();
            reset_screen_counter = 2;
            delay(500);
        }
        else
        {
            if (reset_screen_counter == 2)
            {
                reset_screen_counter = 0;
                delay(500);
            }
        }
    }
    if (Rotary_Flag == 1)
    {
        change_units();  // change the measuring units
        Rotary_Flag = 0; // reset flag to zero
    }
    if (reset_screen_counter == 0)
    {
        startupscreen();
        reset_screen_counter = 1; // set to 1 and wait for Rotary
        click to zero scale
    }
    if (reset_screen_counter == 2)
    {
        start_scale();
    }
}
