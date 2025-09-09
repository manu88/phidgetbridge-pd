#include "m_pd.h"
#include <phidget22.h>
#include <stddef.h>

static void printPhidgetReturnCodeError(PhidgetReturnCode ret,
                                        const char *prefix) {
  const char *errorDescription = NULL;
  PhidgetReturnCode errCode =
      Phidget_getErrorDescription(ret, &errorDescription);
  if (errCode == EPHIDGET_OK) {
    pd_post("%s: error %i %s", prefix, ret, errorDescription);
  }
}

static t_class *PhidgetClass = NULL;

typedef struct {
  t_object x_obj;

  PhidgetVoltageRatioInputHandle voltageRatioInput0;
} PhidgetObject;

static void CCONV onVoltageRatioChange(PhidgetVoltageRatioInputHandle ch,
                                       void *ctx, double voltageRatio) {
  pd_post("VoltageRatio: %lf", voltageRatio);
  PhidgetObject *x = (PhidgetObject *)ctx;
}

static int phidget_configure(PhidgetObject *x) {
  PhidgetReturnCode ret =
      PhidgetVoltageRatioInput_create(&x->voltageRatioInput0);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "PhidgetVoltageRatioInput_create");
    return 0;
  }
  ret = Phidget_setChannel((PhidgetHandle)x->voltageRatioInput0, 0);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "Phidget_setChannel");
    return 0;
  }
  ret = PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(
      x->voltageRatioInput0, onVoltageRatioChange, x);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(
        ret, "PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler");
    return 0;
  }
  ret =
      Phidget_openWaitForAttachment((PhidgetHandle)x->voltageRatioInput0, 5000);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "Phidget_openWaitForAttachment");
    return 0;
  }
  return 1;
}

void phidget_bang(PhidgetObject *x) {
  pd_post("configure Phidget");
  phidget_configure(x);
}

void *phidget_new(void) {
  PhidgetObject *x = (PhidgetObject *)pd_new(PhidgetClass);
  if (x == NULL) {
    return NULL;
  }
  return (void *)x;
}

void phidget_free(PhidgetObject *x) {
  pd_post("free phidget");

  PhidgetReturnCode ret = Phidget_close((PhidgetHandle)x->voltageRatioInput0);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "Phidget_close");
  }
  ret = PhidgetVoltageRatioInput_delete(&x->voltageRatioInput0);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "PhidgetVoltageRatioInput_delete");
  }
}

void phidget_setup(void) {
  PhidgetClass = class_new(gensym("phidget"), (t_newmethod)phidget_new,
                           (t_method)phidget_free, sizeof(PhidgetObject),
                           CLASS_DEFAULT, 0);
  class_addbang(PhidgetClass, phidget_bang);
}
