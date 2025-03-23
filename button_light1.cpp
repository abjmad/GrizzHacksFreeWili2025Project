#include "fwwasm.h"
#include <stdint.h>  // Include the standard integer types

#define COLOR_RED 0x990000
#define COLOR_YELLOW 0x999900
#define COLOR_GREEN 0x009900
#define COLOR_BLUE 0x000099

#define GET_RED(x) ((x >> 16) & 0xFF)
#define GET_GREEN(x) ((x >> 8) & 0xFF)
#define GET_BLUE(x) (x & 0xFF)

#define LED_RED 5
#define LED_YEL 2
#define LED_GRE 3
#define LED_BLU 4

#define RED_IDX 0
#define YEL_IDX 1
#define GRN_IDX 2
#define BLU_IDX 3
#define GRA_IDX 4

#define BUT_NONE -1
#define TIME_LIMIT 10000  // 10 seconds (10000 milliseconds)
#define MAX_SCORE_LEDS 7  // 7 LEDs to represent the score in binary

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
    GameButton(220.0f, COLOR_RED, RED_IDX, LED_RED, FWGUI_EVENT_RED_BUTTON),
    GameButton(110.0f, COLOR_YELLOW, YEL_IDX, LED_YEL, FWGUI_EVENT_YELLOW_BUTTON),
    GameButton(138.59f, COLOR_GREEN, GRN_IDX, LED_GRE, FWGUI_EVENT_GREEN_BUTTON),
    GameButton(164.81f, COLOR_BLUE, BLU_IDX, LED_BLU, FWGUI_EVENT_BLUE_BUTTON),
    GameButton(0.0f, 0x808080, GRA_IDX, LED_YEL, FWGUI_EVENT_GRAY_BUTTON)  // Gray button with custom LED value (gray color)
};

uint32_t background_color = COLOR_YELLOW;  // Start with yellow background
int color_change_count = 0;  // Track the number of color changes
int score = 0;  // Track the score
unsigned long startTime;  // Start time to track 10 seconds

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

void turnOffAllLEDs() {
    // Turn off all LEDs
    setBoardLED(LED_RED, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_YEL, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_GRE, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
    setBoardLED(LED_BLU, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);
}

void updateBackground() {
    // Use addPanel() and showPanel() to update the background color
    addPanel(0, 1, 0, 0, 0, GET_RED(background_color), GET_GREEN(background_color), GET_BLUE(background_color), 1);
    showPanel(0);
}

void changeBackgroundColor() {
    // Cycle through the colors: yellow -> blue -> green -> yellow
    if (background_color == COLOR_YELLOW) {
        background_color = COLOR_BLUE;
    } else if (background_color == COLOR_BLUE) {
        background_color = COLOR_GREEN;
    } else if (background_color == COLOR_GREEN) {
        background_color = COLOR_YELLOW;
    }

    // Increment the change counter
    color_change_count++;
}

void checkCorrectButton(int pressed) {
    // Check if the pressed button matches the current background color
    if ((background_color == COLOR_YELLOW && pressed == YEL_IDX) ||
        (background_color == COLOR_BLUE && pressed == BLU_IDX) ||
        (background_color == COLOR_GREEN && pressed == GRN_IDX)) {
        score++;  // Increment score for a correct press
    }
}

void displayScore() {
    // Display the score in binary using the LEDs
    for (int i = 0; i < MAX_SCORE_LEDS; i++) {
        // Check if the bit is 1 and light up the LED accordingly
        if ((score >> i) & 1) {
            setBoardLED(i, 255, 255, 255, 255, LEDManagerLEDMode::ledpulsefade);  // Light up LED if bit is 1
        } else {
            setBoardLED(i, 0, 0, 0, 0, LEDManagerLEDMode::ledpulsefade);  // Turn off LED if bit is 0
        }
    }
}

void flashButtonLED(int pressed) {
    // Flash the LED corresponding to the button pressed
    if (pressed == YEL_IDX) {
        setBoardLED(LED_YEL, 255, 255, 0, 255, LEDManagerLEDMode::ledpulsefade);
    } else if (pressed == BLU_IDX) {
        setBoardLED(LED_BLU, 0, 0, 255, 255, LEDManagerLEDMode::ledpulsefade);
    } else if (pressed == GRN_IDX) {
        setBoardLED(LED_GRE, 0, 255, 0, 255, LEDManagerLEDMode::ledpulsefade);
    }
}

int main() {
    startTime = millis();  // Record the start time when the program begins

    while (true) {
        turnOffAllLEDs();  // Turn off all LEDs first

        // Update the background with the current color
        updateBackground();

        // Check if 10 seconds have passed
        if (millis() - startTime >= TIME_LIMIT) {
            // End the program after 10 seconds, enter the endgame phase
            break;
        }

        if (hasEvent()) {
            int pressed = getButtonPress();

            // Check if gray button (GRA_IDX) is pressed, exit the program
            if (pressed == GRA_IDX) {
                break;  // Exit the loop and end the program
            }

            // Change background color when a color button is pressed
            if (pressed == YEL_IDX || pressed == BLU_IDX || pressed == GRN_IDX) {
                changeBackgroundColor();
                flashButtonLED(pressed);  // Flash the LED corresponding to the button pressed
            }

            // Check if the button press is correct
            checkCorrectButton(pressed);

        }

        waitms(100);  // Add a small delay to control update frequency
    }

    // Endgame phase: Display the score using LEDs
    displayScore();

    // Wait for the gray button press to exit the program
    while (true) {
        if (hasEvent()) {
            int pressed = getButtonPress();
            if (pressed == GRA_IDX) {
                break;  // Exit the program when the gray button is pressed
            }
        }
        waitms(100);
    }

    return 0;
}
