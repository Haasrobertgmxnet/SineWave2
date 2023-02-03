#ifdef INCLUDE_ARDUINO_H
#include <Arduino.h>
#endif

#define CALIBRATED_SINE

#ifdef EXAMPLE_SINE

//THIS IS FROM HERE: https://wiki.seeedstudio.com/Seeeduino-XIAO/
#define DAC_PIN A0 // Make code a bit more legible
 
float x = 0; // Value to take the sin of
float increment = 0.02;  // Value to increment x by each time
int frequency = 440; // Frequency of sine wave
 
void setup() 
{
  analogWriteResolution(10); // Set analog out resolution to max, 10-bits
  analogReadResolution(12); // Set analog input resolution to max, 12-bits
 
  SerialUSB.begin(9600);
}
 
void loop() 
{
  // Generate a voltage value between 0 and 1023. 
  // Let's scale a sin wave between those values:
  // Offset by 511.5, then multiply sin by 511.5.
  int dacVoltage = (int)(511.5 + 511.5 * sin(x));
  x += increment; // Increase value of x
 
  // Generate a voltage between 0 and 3.3V.
  // 0= 0V, 1023=3.3V, 512=1.65V, etc.
  analogWrite(DAC_PIN, dacVoltage);
 
  // Now read A1 (connected to A0), and convert that
  // 12-bit ADC value to a voltage between 0 and 3.3.
  float voltage = analogRead(A1) * 3.3 / 4096.0;
  SerialUSB.println(voltage); // Print the voltage.
  delay(1); // Delay 1ms
}

#endif

#ifdef CALIBRATED_SINE

#include <array>

constexpr uint16_t TABLE_SIZE= 500;
constexpr float pi = 3.14159265359f;
constexpr float f= 50.0f;

constexpr float _abs(float x) {
    return (x < 0) ? -x : x;
}

// ! Cave: Recursion
constexpr float _mod2pi(float x) {
    return (x < 0.0) ? _mod2pi(x + 2.0 * pi) : ((x > 2.0 * pi) ? _mod2pi(x - 2.0 * pi) : x);
}

// sine between 0 and pi/2
// !! use it only for x between 0 and pi/2
// cutted taylor expansion
// incomplete Horner scheme to evaluate the polynomial
constexpr float pricipalFlankOfSine(float x) {
    return (_abs(x - 0.25 * pi) > 0.25 * pi) ? -10.0 :
        (x * (1.0 - x * x / 6.0 * (1.0 - x * x / 20.0 * (1.0 - x * x / 42.0 * (1.0 - x * x / 72.0 * (1.0 - x * x / 110.0))))));
}

// sine between 0 and pi, the positive segment
constexpr float positiveSine(float x) {
    return (_abs(x - 0.25 * pi) <= 0.25 * pi) ? pricipalFlankOfSine(x) :
        ((_abs(x - 0.75 * pi) <= 0.25 * pi) ? pricipalFlankOfSine(pi - x) : -10.0);
}

// sine between pi and 2*pi, the negative segment
constexpr float negativeSine(float x) {
    return (_abs(x - 1.25 * pi) <= 0.25 * pi) ? -pricipalFlankOfSine(x - pi) :
        ((_abs(x - 1.75 * pi) <= 0.25 * pi) ? -pricipalFlankOfSine(2.0 * pi - x) : -10.0);
}

// overall sine between 0 and 2*pi
constexpr float sine(float x) {
    return (_abs(_mod2pi(x) - 0.5 * pi) <= 0.5 * pi) ? positiveSine(_mod2pi(x)) : negativeSine(_mod2pi(x));
}

// sine for integer argument and ...
// ... with integer result values between 0 and 1023
// uses TABLE_SIZE as global constant
constexpr unsigned int tenBitSine(unsigned int n) {
    return (unsigned int)(1023 * 0.5 * (1.0 + sine(2.0 * pi * (float)n / TABLE_SIZE)));
}

// This is from here: https://de.wikipedia.org/wiki/C%2B%2B-Metaprogrammierung
// More precisely: https://de.wikipedia.org/wiki/C%2B%2B-Metaprogrammierung#Generierung_statischer_Tabellen_zur_Kompilierungszeit

/**
 * Variadisches Template für die rekursive Hilfs-Struktur.
 */
template<int INDEX = 0, int ...D>
struct Helper : Helper<INDEX + 1, D..., tenBitSine(INDEX)> { };

/**
 * Spezialisierung des Templates um bei einer Größe von TABLE_SIZE die Rekursion zu beenden.
 */
template<int ...D>
struct Helper<TABLE_SIZE, D...> {
    static constexpr std::array<int, TABLE_SIZE> table = { D... };
};

constexpr std::array<int, TABLE_SIZE> table = Helper<>::table;

enum {
    FOUR = table[2] // Nutzung zur Kompilierungszeit
};

void setup()
{
  analogWriteResolution(10); // Set analog out resolution to max, 10-bits
  analogReadResolution(12); // Set analog input resolution to max, 12-bits
  pinMode(A1,OUTPUT);
  
  Serial.begin(9600);
}

void loop()
{
  for (uint16_t n = 0; n < TABLE_SIZE; ++n)
  {
    auto y= table[n];
    analogWrite(A0, y); // DAC Output
    analogWrite(A1, y); // PWM Output

    auto rd= analogRead(A2);
    float scal= static_cast<float>(rd)/4096.0;
    float ts= static_cast<float>(1.0/(TABLE_SIZE*scal*f));
    delayMicroseconds((int)1000000*ts);

    //Serial Print Section
    Serial.print(y);
    Serial.print("  ");
    Serial.print(rd);
    Serial.println();
  }
  // for (auto x : sineValues.values){
  //   Serial.print("sine: ");
  //   Serial.print(x);
  //   Serial.println();
  // }
        
}

#endif