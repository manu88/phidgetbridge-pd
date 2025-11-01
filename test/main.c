#include <phidget22.h>
#include <stdio.h>

static void CCONV onVoltageRatioChange(PhidgetVoltageRatioInputHandle ch,
                                       void *ctx, double voltageRatio) {
  printf("VoltageRatio: %lf\n", voltageRatio);
}

int main(int argc, char *argv[]) {
  int channel = 1;
  PhidgetVoltageRatioInputHandle voltageRatioInput0;

  if (PhidgetVoltageRatioInput_create(&voltageRatioInput0) != 0) {
    printf("PhidgetVoltageRatioInput_create error\n");
  }
  if (Phidget_setChannel((PhidgetHandle)voltageRatioInput0, channel) != 0) {
    printf("Phidget_setChannel error\n");
  }
  if (PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(
          voltageRatioInput0, onVoltageRatioChange, NULL) != 0) {
    printf("PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler error\n");
  }

  PhidgetVoltageRatioInput_BridgeGain gain;
  if (PhidgetVoltageRatioInput_getBridgeGain(voltageRatioInput0, &gain) != 0) {
    printf("PhidgetVoltageRatioInput_getBridgeGain error\n");
  }
  printf("gain = %i\n", gain);

  // PhidgetVoltageRatioInput_setBridgeGain(voltageRatioInput0, BRIDGE_GAIN_1);
  Phidget_openWaitForAttachment((PhidgetHandle)voltageRatioInput0, 5000);

  printf("Go\n");
  // Wait until Enter has been pressed before exiting
  getchar();

  Phidget_close((PhidgetHandle)voltageRatioInput0);

  PhidgetVoltageRatioInput_delete(&voltageRatioInput0);
}
