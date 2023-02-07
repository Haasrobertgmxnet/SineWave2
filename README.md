# SineWave2

## 1. Description

SineWave2 contains small C++-program to produce an integer-valued sine wave between 0 and 1023 which is good for 10-bit DAC.
The program has been run on a Seeeduino Xiao board, see here: https://wiki.seeedstudio.com/Seeeduino-XIAO/, since this simple board has the following properties:
   * It works with the Arduino framework.
   * It allows for PWM output as well as 10-bit DAC output.
   
As development framework Visual Studio Code with the PlatformIO extension has been used.
The setup() and loop() functions are very small and the project not necessarily sticks on the Arduino framework. The C++ stuff outside the setup() and loop() functions can be used in other C++ frameworks, e.g. for desktop development in Microsoft Visual Studio.

## 2. Features

   * A table of integer sine values between 0 and 1023 is created along a a [0, 2pi] sine wave. The abscissa values ("x values") are not part of this table.
   * This table is created during compile time. A table of integer values ascending from 0 to 500 is created by template metaprogramming copied from Wikipedia, see here: https://de.wikipedia.org/wiki/C%2B%2B-Metaprogrammierung .
   * Since C++ built-in sine (or std sine) is not constexpr it has been re-programmed.
   * The sine function and its helper functions are constexpr functions. 
   * The principal sine flank between 0 and pi/2 is calculated from a Taylor series cut after the term with x^11. The resulting polynomial is evaluated using the Horner scheme.
   * The whole [0,2 pi]-sine wave is created from the principal flank by symmetry considerations to give an overall constexpr sine.
   * The DAC output is sent to Pin A0, where the PWM output is sent to Pin A1.
   * At Pin A2 an external analog input can be applied. If the voltage at A2 varies the C++ program will change the frequency of the sine wave.
   
## 3. Remarks

### Limitations of the Arduino environment

   * The constexpr functions do not allow for if-statements. Instead, the ternary operator is used.
   * The sine table could not made constexpr. Instead, template metaprogramming had to be used.
   
### Alternative Implementations

   * If the limitations of the Arduino environment are not present, the code can be simplified for the sake of more readability. In particular
      * The template metaprogramming can be replaced by constexpr struct.
      * The ternary operators can be replaced by if-statements.
   * The sine values can be hard-corded in vector-like construct, e.g. a C vector or an std::array which can be made constexpr and moved to seperate header file.
   
   
