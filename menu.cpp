#include <fwwasm.h>
#include <stdio.h>  // <- for snprintf

const int PANEL_INDEX = 0;
const int NUM_ITEMS = 3;
int menuIndex = 0;

// Menu labels
const char* menuItems[NUM_ITEMS] = {
    "2-Player Motion Game",
    "Battleship",
    "Stats Page"
};

// Script paths for each item (placeholder for now)
const char* scriptPaths[NUM_ITEMS] = {
    "/scripts/tilt_led.wasm",      // Placeholder
    "/scripts/battleship.wasm",    // Placeholder
    "/scripts/stats.wasm"          // Placeholder
};

// Draws the current menu with a ▶ on the selected line
void drawMenu() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        char line[64];
        if (i == menuIndex) {
            snprintf(line, sizeof(line), "▶  %s", menuItems[i]);
        } else {
            snprintf(line, sizeof(line), "   %s", menuItems[i]);
        }

        // Re-draw the line
        addControlText(PANEL_INDEX, i, 20, 40 + i * 20, 0, 2, 255, 255, 255, line);
    }
}

auto main() -> int {
    // Setup panel
    addPanel(PANEL_INDEX, 1, 0, 0, -1, 0, 0, 0, 0);

    drawMenu();  // Draw initial menu

    while (true) {
        if (hasEvent()) {
            unsigned char data[FW_GET_EVENT_DATA_MAX];
            int eventType = getEventData(data);

            switch (eventType) {
                case FWGUI_EVENT_GRAY_BUTTON: // 🔼 Up
                    menuIndex = (menuIndex - 1 + NUM_ITEMS) % NUM_ITEMS;
                    drawMenu();
                    break;

                case FWGUI_EVENT_YELLOW_BUTTON: // 🔽 Down
                    menuIndex = (menuIndex + 1) % NUM_ITEMS;
                    drawMenu();
                    break;

                case FWGUI_EVENT_GREEN_BUTTON: // ✅ Select
                    runZoomIOScript(scriptPaths[menuIndex]);
                    break;

                case FWGUI_EVENT_RED_BUTTON: // 🟥 Back
                    exitToMainAppMenu();
                    break;
            }
        }

        waitms(50);
    }

    return 0;
}
