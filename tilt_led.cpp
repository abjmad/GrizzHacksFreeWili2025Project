#include <fwwasm.h>

auto main() -> int {
    setBoardLED(0, 255, 0, 0, 1000, LEDManagerLEDMode::ledpulsefade);
    waitms(1000);
    exitToMainAppMenu();
    return 0;
}
