# WM Corporate Office Trash Mass Measurer

## Schematics

This image is the schematic of the Arduino and the connections to the rest of the electrical components.

![schematic](/images/schematic.jpeg)

The list of the components: [Google Sheets](https://docs.google.com/spreadsheets/d/1fJyYHJwZsYlW4-pT5zG5G21gCzmxWYxnucfjncXrOdE/edit?usp=sharing)

An explination of how the entire system works: [Google Drive](https://drive.google.com/file/d/1w-fdO968TKD7DCSyudjJUFCal9qf6_Ja/view?usp=sharing)

The design of the device is described below. The code necessary to run the device is also written below.  

## The Design – Top View

The picture below is the top view of the device that I built, which utilizes 2 screws with different head, the M4 and the M5, to screw onto pieces of the wood (the load cell itself has 2 screw holes on each side; it is important that the M4 head is on the same side where the wood is screwed on to the top (there is also an arrow on that side, showing the direction the load cell needs to be pointing, and where the mass should be applied)  

![Top View schematic](/images/top_view.jpeg)

## The Design – Side View

The picture below is the side view of the load cell, where one important thing to note is the spacers that prevent the wood to directly touch the epoxy on the load cell as that cannot have any direct force applied since the Wheatstone bridge (how a load cell works, by having the resistance change when a mass is applied) is directly beneath there.  

![Side View Schematic](/images/side_view.jpeg)

## The Code

To actually use the Arduino to complete its function, please take a look at the code attached below. The C**alibration Code** must be used before the **PostCalibrationCode** since the **Calibration** is used to calibrate the scale so that the **PostCalibrationCode** can record the mass of the trash in the trashcans.  

### Calibration Code

```cpp
/* This is the calibration code for the both the load cells connected to the Arduino. After the Arduino is wired up, run this code in the Arduino IDE, which can be found at this link: <https://www.arduino.cc/en/software> This code is important to run, as the load cells need to calibrated in order to be used.In order to run this program, use the inbuilt Library Manager (tools > Library Manager) to download the HX711.h and U8glib.h libraries; they are necessary to run the programs. Then, copy this code to the Arduino, and then start the serial monitor at the 9600 band.This code, when "#define DOUT  11" and "#define CLK  12" is commented out, will give the calibration value for Load Cell 1 on the schematic. Please note this value down: "loadcell_1_calibration"; Then uncomment out "#define DOUT  11" and "#define CLK  12" and comment out "#define DOUT  6" and "#define CLK  5" so that the calibration value for Load Cell 2 on the schematic can be found. Please also note this value down: "loadcell_2_calibration". Also, please fully read the comments in this code to understand the procedure (more procedures are written below)
 */
#include "HX711.h"

#define DOUT 6 // Arduino pin 6 connect to HX711 DOUT --> Loadcell 1 (Part of pair 1)
#define CLK 5  //  Arduino pin 5 connect to HX711 CLK --> Loadcell 1 (Part of pair 1)
// #define DOUT  11  // Arduino pin 11 connect to HX711 DOUT -> Loadcell 2 (Part of pair 2)
// #define CLK  12  //  Arduino pin 12 connect to HX711 CLK -> Loadcell 2 (Part of pair 2)
HX711 scale(DOUT, CLK); // Init of library
void setup()
{
    Serial.begin(9600);
    scale.set_scale(); // Start scale
    scale.tare();      // Reset scale to zero
}
void loop()
{
    double knownWeight = 0.174;
    /* it is important to already have the mass of an object when calibrating the scale; it has to be in kilograms for this calibration, I used the mass of an iPhone X without the case, which has a mass of 174g or .174kg after letting the load cell sit for about 5 seconds, the known mass was placed on the scale and then I waited for 5 more seconds to get the calibration value after this, comment out the current pair of DOUT and CLK values, and uncomment out the second pair of values repeat the steps of uploading the program, waiting, placing the known mass, waiting again, and noting the values found, we are now free to use the second program.*/
    float current_weight = scale.get_units(20);          // get average of 20 scale readings
    float scale_factor = (current_weight / knownWeight); // divide the result by a known weight
    Serial.println(scale_factor);                        // Print the scale factor to use
}
```

### PostCalibrationCode

```cpp
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
```
