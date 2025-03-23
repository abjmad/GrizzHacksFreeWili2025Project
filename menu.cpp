#include <fwwasm.h>

auto main() -> int {
    // Show a hot pink LED pulse so we know it's running
    setBoardLED(0, 255, 105, 180, 1000, LEDManagerLEDMode::ledpulsefade);

    // Force screen to display something
    addPanel(0, 1, 0, 0, 0, 0, 0, 0, 0);  // panel 0, visible
    addControlText(0, 0, 20, 30, 0, 24, 255, 255, 255, "📋 Menu Loaded!");

    // Loop forever to keep the script alive
    while (true) {
        waitms(1000);
    }

    return 0;
}