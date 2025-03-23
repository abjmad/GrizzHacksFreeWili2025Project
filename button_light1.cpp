#include "fwwasm.h"
#include <stdint.h>  // Include the standard integer types
#include <bitset>    // For binary conversion
#include <cstdlib>   // For rand() and srand()
#include <ctime>     // For time() to seed random number generator

#define COLOR_RED 0x990000
#define COLOR_YELLOW 0x999900
#define COLOR_GREEN 0x009900
#define COLOR_BLUE 0x000099
#define COLOR_PINK 0xFF66CC  // Pink for endgame phase
#define COLOR_WHITE 0xFFFFFF // White for displaying score

#define GET_RED(x) ((x >> 16) & 0xFF)
#define GET_GREEN(x) ((x >> 8) & 0xFF)
#define GET_BLUE(x) (x & 0xFF)

// Define all LED indexes for clarity
#define LED_1 0
#define LED_2 1
#define LED_3 2
#define LED_4 3
#define LED_5 4
#define LED_6 5
#define LED_7 6

#define RED_IDX 0
#define YEL_IDX 1  // Yellow button index
#define GRN_IDX 2  // Green button index
#define BLU_IDX 3  // Blue button index
#define GRA_IDX 4  // Gray button index

#define BUT_NONE -1
#define TIME_LIMIT 10000  // 10 seconds (10000 milliseconds)
#define MAX_SCORE_LEDS 7  // 7 LEDs to represent the score in binary

uint32_t color_pattern[] = {
    COLOR_YELLOW, COLOR_BLUE, COLOR_YELLOW, COLOR_BLUE, COLOR_YELLOW, COLOR_WHITE,
    COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE, COLOR_YELLOW,
    COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_GREEN, COLOR_WHITE, COLOR_GREEN,
    COLOR_BLUE, COLOR_YELLOW, COLOR_WHITE, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
    COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_WHITE, COLOR_GREEN,
    COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN, COLOR_YELLOW, COLOR_GREEN, COLOR_YELLOW,
    COLOR_BLUE, COLOR_GREEN, COLOR_WHITE, COLOR_YELLOW, COLOR_BLUE, COLOR_GREEN,
    COLOR_YELLOW, COLOR_BLUE, COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE,
    COLOR_GREEN, COLOR_YELLOW
};

// Define the countdown colors (Red, Yellow, Green, Blue)
uint32_t countdown_colors[] = {COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE};

uint32_t countdown_purple = 0x800080; // Purple color

// Declare startTime variable to track the time
unsigned long startTime;  // Store the start time for the timer

// Define the GameButton struct
struct GameButton {
    float tone_freq;
    uint32_t led_color;
    uint8_t index;
    uint8_t led_position;
    uint8_t button_event;

    // Constructor to initialize the struct
    GameButton(float tone, uint32_t color, uint8_t idx, uint8_t led_pos, uint8_t button_evt)
        : tone_freq(tone), led_color(color), index(idx), led_position(led_pos), button_event(button_evt) {}
};

// Declare the array of GameButton and manually initialize each element
GameButton game_buttons[5] = {
    GameButton(220.0f, COLOR_RED, RED_IDX, LED_1, FWGUI_EVENT_RED_BUTTON),
    GameButton(110.0f, COLOR_YELLOW, YEL_IDX, LED_2, FWGUI_EVENT_YELLOW_BUTTON),
    GameButton(138.59f, COLOR_GREEN, GRN_IDX, LED_3, FWGUI_EVENT_GREEN_BUTTON),
    GameButton(164.81f, COLOR_BLUE, BLU_IDX, LED_4, FWGUI_EVENT_BLUE_BUTTON),
    GameButton(0.0f, 0x808080, GRA_IDX, LED_5, FWGUI_EVENT_GRAY_BUTTON)  // Gray button with custom LED value (gray color)
};

uint32_t background_color = COLOR_YELLOW;  // Start with yellow background
int color_change_count = 0;  // Track the number of color changes
int score = 0;  // Track the score
int total_steps = 50;  // Total number of steps in the sequence (50)
int current_step = 0;  // Current step in the pattern
bool scoringPhase = true;  // Flag to control scoring phase

int getButtonPress() {
    int retval = BUT_NONE;
    uint8_t data[FW_GET_EVENT_DATA_MAX] = {0};
    int last_event = getEventData(data);
    for (int i = 0; i < 5; i++) {
        if (last_event == game_buttons[i].button_event) {
            retval = game_buttons[i].index;
        }
    }
    return retval;
}

// Function to turn off all LEDs
void turnOffAllLEDs() {
    // Turn off all LEDs
    setBoardLED(LED_1, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_2, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_3, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_4, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_5, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_6, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_7, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
}

// Function to update the background
void updateBackground() {
    // Use addPanel() and showPanel() to update the background color
    addPanel(0, 1, 0, 0, 0, GET_RED(background_color), GET_GREEN(background_color), GET_BLUE(background_color), 1);
    showPanel(0);
}

// Function to perform the countdown affecting the LEDs
void countdown() {
    for (int i = 0; i < 4; i++) {
        // Set all LEDs to the current countdown color
        setBoardLED(LED_1, GET_RED(countdown_colors[i]), GET_GREEN(countdown_colors[i]), GET_BLUE(countdown_colors[i]), 1000, LEDManagerLEDMode::ledsimplevalue);
        setBoardLED(LED_2, GET_RED(countdown_colors[i]), GET_GREEN(countdown_colors[i]), GET_BLUE(countdown_colors[i]), 1000, LEDManagerLEDMode::ledsimplevalue);
        setBoardLED(LED_3, GET_RED(countdown_colors[i]), GET_GREEN(countdown_colors[i]), GET_BLUE(countdown_colors[i]), 1000, LEDManagerLEDMode::ledsimplevalue);
        setBoardLED(LED_4, GET_RED(countdown_colors[i]), GET_GREEN(countdown_colors[i]), GET_BLUE(countdown_colors[i]), 1000, LEDManagerLEDMode::ledsimplevalue);
        setBoardLED(LED_5, GET_RED(countdown_colors[i]), GET_GREEN(countdown_colors[i]), GET_BLUE(countdown_colors[i]), 1000, LEDManagerLEDMode::ledsimplevalue);
        setBoardLED(LED_6, GET_RED(countdown_colors[i]), GET_GREEN(countdown_colors[i]), GET_BLUE(countdown_colors[i]), 1000, LEDManagerLEDMode::ledsimplevalue);
        setBoardLED(LED_7, GET_RED(countdown_colors[i]), GET_GREEN(countdown_colors[i]), GET_BLUE(countdown_colors[i]), 1000, LEDManagerLEDMode::ledsimplevalue);

        // Wait for 1 second (1000 milliseconds) before changing the color
        waitms(1000);
    }
}

// Function to change the background color based on the fixed 50-step pattern
void changeBackgroundColor() {
    // Set the background color to the current color in the pattern
    background_color = color_pattern[current_step];

    // Update the background to reflect the new color
    updateBackground();

    // Increment the step counter
    current_step++;

    // If we've reached the end of the 50 steps, reset the step counter
    if (current_step == total_steps) {
        current_step = 0;  // Reset to the first step to repeat the sequence
    }
}

// Function to check if the pressed button is correct
void checkCorrectButton(int pressed) {
    // Check if the pressed button matches the current background color
    if ((background_color == COLOR_YELLOW && pressed == YEL_IDX) ||
        (background_color == COLOR_BLUE && pressed == BLU_IDX) ||
        (background_color == COLOR_GREEN && pressed == GRN_IDX) ||
        (background_color == COLOR_WHITE && pressed == GRA_IDX)) {  // GRA_IDX now corresponds to white background
        score++;  // Increment score for a correct press
    }
}

// Function to convert the decimal score to a 7-bit binary value and light up LEDs
void displayBinaryOnLED(int score) {
    // Ensure the score is within the 7-bit range (0 to 127)
    if (score < 0 || score > 127) {
        return;  // Invalid score, do nothing
    }

    // Convert the score to a 7-bit binary representation
    std::bitset<7> binaryValue(score);

    // Loop through each bit and control the corresponding LED
    for (int i = 0; i < 7; i++) {
        if (binaryValue[i] == 1) {
            // Turn ON the LED if the bit is 0 (inverted logic)
            setBoardLED(i, 0, 255, 0, 700, LEDManagerLEDMode::ledsimplevalue);  // White color for ON
        } else {
            // Turn OFF the LED if the bit is 1 (inverted logic)
            setBoardLED(i, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);  // Off
        }
    }
}

int main() {
    // Perform countdown before the main game starts
    countdown();  // This will show Red, Yellow, Green, Blue on LEDs for 1 second each

    // Now start the main 10-second timer
    startTime = millis();  // Record the start time when the timer begins
    unsigned long countdown_start = millis();  // Store the start time of the countdown
    int lights_left = 7;  // Number of lights still on (representing time left)

    // Active Phase (Scoring Phase)
    while (scoringPhase) {
        // Update the background with the current color
        updateBackground();

        // Display the remaining time using purple lights
        // Turn off one light every 1.428 seconds (total 10 seconds / 7 lights)
        if ((millis() - countdown_start) >= (10000 / 7) * (7 - lights_left)) {
            // Turn off the light at the current position
            setBoardLED(7 - lights_left, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);  // Turn off the LED
            lights_left--;  // Decrease the number of lights left
        }

        // Check if 10 seconds have passed, and enter endgame phase
        if (millis() - startTime >= TIME_LIMIT) {
            scoringPhase = false;  // End the scoring phase
            background_color = COLOR_PINK;  // Change background to pink for the endgame phase
        }

        if (hasEvent()) {
            int pressed = getButtonPress();

            // Check if the button press is correct (only if in scoring phase)
            if (scoringPhase) {
                checkCorrectButton(pressed);  // This will increment the score if the correct button is pressed
            }

            // Change background color when a color button is pressed (only if in scoring phase)
            if (scoringPhase && (pressed == YEL_IDX || pressed == BLU_IDX || pressed == GRN_IDX || pressed == GRA_IDX)) {
                changeBackgroundColor();
            }
        }

        waitms(100);  // Add a small delay to control update frequency
    }

    // Transition to Endgame Phase
    background_color = COLOR_PINK;  // Set background to pink for endgame
    updateBackground();  // Ensure the panel shows the pink background for the endgame phase

    // Display the binary score on LEDs once at the end of the scoring phase
    displayBinaryOnLED(score);

    // Wait for the red button press to exit the game
    while (true) {
        if (hasEvent()) {
            int pressed = getButtonPress();
            if (pressed == RED_IDX) {
                // Red button is pressed, turn off all lights and exit the program
                turnOffAllLEDs();  // Turn off all LEDs
                break;  // Exit the loop and close the program
            }
        }

        waitms(100);  // Add a small delay to control update frequency
    }

    return 0;  // Return an integer value (typically 0 for successful execution)
}