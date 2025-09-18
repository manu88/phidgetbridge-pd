#include <phidget22.h>
#include <stdio.h>

static void CCONV onVoltageRatioChange(PhidgetVoltageRatioInputHandle ch,
                                       void *ctx, double voltageRatio) {
  printf("VoltageRatio: %lf\n", voltageRatio);
}

int main(int argc, char *argv[]) {
  PhidgetVoltageRatioInputHandle voltageRatioInput0;

  PhidgetVoltageRatioInput_create(&voltageRatioInput0);

  PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(
      voltageRatioInput0, onVoltageRatioChange, NULL);

  Phidget_openWaitForAttachment((PhidgetHandle)voltageRatioInput0, 5000);

  // Wait until Enter has been pressed before exiting
  getchar();

  Phidget_close((PhidgetHandle)voltageRatioInput0);

  PhidgetVoltageRatioInput_delete(&voltageRatioInput0);
}
