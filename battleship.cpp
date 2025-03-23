#include <fwwasm.h>

auto main() -> int {
    setBoardLED(1, 255, 255, 0, 1000, LEDManagerLEDMode::ledpulsefade);
    waitms(1000);
    exitToMainAppMenu();
    return 0;
}
