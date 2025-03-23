#include "fwwasm.h"
#include <array>

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

struct GameButton
{
    float tone_freq;
    uint32_t led_color;
    uint8_t index;
    uint8_t led_position;
    uint8_t button_event;
};

GameButton game_buttons[4];

int getButtonPress()
{
    int retval = BUT_NONE;
    uint8_t event_data[FW_GET_EVENT_DATA_MAX] = { 0 };
    int last_event = getEventData(event_data);
    for (int i = 0; i < 4; i++)
    {
        if (last_event == game_buttons[i].button_event)
        {
            retval = game_buttons[i].index;
        }
    }
    if (last_event == FWGUI_EVENT_GRAY_BUTTON)
    {
        retval = GRA_IDX;
    }
    return retval;
}

void setup()
{
    game_buttons[0].tone_freq = 220.0f; // RED
    game_buttons[0].led_color = COLOR_RED;
    game_buttons[0].led_position = LED_RED;
    game_buttons[0].button_event = FWGUI_EVENT_RED_BUTTON;
    game_buttons[0].index = RED_IDX;

    game_buttons[1].tone_freq = 110.0f; // YELLOW
    game_buttons[1].led_color = COLOR_YELLOW;
    game_buttons[1].led_position = LED_YEL;
    game_buttons[1].button_event = FWGUI_EVENT_YELLOW_BUTTON;
    game_buttons[1].index = YEL_IDX;

    game_buttons[2].tone_freq = 138.59f; // GREEN
    game_buttons[2].led_color = COLOR_GREEN;
    game_buttons[2].led_position = LED_GRE;
    game_buttons[2].button_event = FWGUI_EVENT_GREEN_BUTTON;
    game_buttons[2].index = GRN_IDX;

    game_buttons[3].tone_freq = 164.81f; // BLUE
    game_buttons[3].led_color = COLOR_BLUE;
    game_buttons[3].led_position = LED_BLU;
    game_buttons[3].button_event = FWGUI_EVENT_BLUE_BUTTON;
    game_buttons[3].index = BLU_IDX;
}

int main()
{
    setup();

    while (true)
    {
        if (hasEvent())
        {
            int pressed = getButtonPress();

            if (pressed >= 0 && pressed <= 3)
            {
                GameButton btn = game_buttons[pressed];
                setBoardLED(btn.led_position, GET_RED(btn.led_color), GET_GREEN(btn.led_color), GET_BLUE(btn.led_color), 300, LEDManagerLEDMode::ledpulsefade);
            }
            else if (pressed == GRA_IDX)
            {
                setBoardLED(0, 128, 128, 128, 300, LEDManagerLEDMode::ledpulsefade);
            }
        }
        waitms(100);
    }

    return 0;
}
