// Author: Brian Wu
// Written: Oct 21st, 2022 
// FOR ESP32
/*
Program to test at what PWM frequency would you be able to see the "blinking" state of the LED.
It starts from LED_MAX_FREQUENCY and goes down 2Hz per button press, and prints out the current PWM to Serial Monitor.

Results:
Around 40-42 Hz is when the human eye can notice it flashing/blinking.
*/


#define interruptPin 0
#define PRESS_DELAY 200 // defined in microseconds
#define ledR 16
#define ledG 17
#define ledB 18
#define LED_MAX_FREQUENCY 64 

// Function Prototypes
void turnLED();
void IRAM_ATTR change_frequency();
void IRAM_ATTR debouncechange_frequency();

// Global Variables
volatile unsigned long lastDebounceTime = 0;
volatile uint8_t freqChange = 0;
volatile bool changeFlag = false;

void setup()
{
    Serial.begin(115200);
    delay(10);

    Serial.println("\nBooting up ESP32...");
    
    pinMode(interruptPin, INPUT_PULLUP); // The switch is always "1", until pressed which will turn it "0"
    delay(1000); // To prevent change_frequency from being accidentally called because it senses "rising edge"

    attachInterrupt(digitalPinToInterrupt(interruptPin), debouncechange_frequency, RISING);

    ledcAttachPin(ledR, 1); // assign RGB led pins to channels
    ledcAttachPin(ledG, 2);
    ledcAttachPin(ledB, 3);

    // Sets up the ability to range in values from in resolution. 
    ledcSetup(1, LED_MAX_FREQUENCY, 6); // Sets the resolution to range from 0 to 63
    ledcSetup(2, LED_MAX_FREQUENCY, 6);
    ledcSetup(3, LED_MAX_FREQUENCY, 6);
    changeFlag = true;
}

void loop()
{
    turnLED(); // Turns on the LED
    // Everytime the button is pressed, it will change the PWM Frequency.
    if (changeFlag)
    {
        Serial.print("The PWM frequency is: ");
        Serial.println(ledcSetup(1, LED_MAX_FREQUENCY - freqChange, 6)); // Sets the resolution to range from 0 to 63
        ledcSetup(2, LED_MAX_FREQUENCY - freqChange, 6); // Sets the resolution to range from 0 to 63
        ledcSetup(3, LED_MAX_FREQUENCY - freqChange, 6); // Sets the resolution to range from 0 to 63
        changeFlag = false;
    }

}

// Turns the LED barely on to notice the frequency
void turnLED()
{

    ledcWrite(1, 60);
    ledcWrite(2, 60);
    ledcWrite(3, 60);
    
}

// IRAM_ATTR stores the function in flash ram so it is fast as possible
void IRAM_ATTR debouncechange_frequency()
{
    noInterrupts();
    // Makes it so that the actual interrupt function will not be called until a certain delay
    // has passed between the last bounce
    if ((millis() - lastDebounceTime) >= PRESS_DELAY)
    {
        change_frequency();
        lastDebounceTime = millis();
    }
    interrupts();
}


// Function that changes the PWM frequency
void IRAM_ATTR change_frequency()
{
    if (freqChange >= LED_MAX_FREQUENCY)
    {
        freqChange = 0;
    }
    else
    {
        freqChange += 2;
    }
    changeFlag = true;
}
