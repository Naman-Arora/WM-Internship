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