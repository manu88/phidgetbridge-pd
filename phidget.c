#include "m_pd.h"
#include <phidget22.h>
#include <stddef.h>

static const char objectName[] = "phidget";

static void printPhidgetReturnCodeError(PhidgetReturnCode ret,
                                        const char *prefix) {
  const char *errorDescription = NULL;
  PhidgetReturnCode errCode =
      Phidget_getErrorDescription(ret, &errorDescription);
  if (errCode == EPHIDGET_OK) {
    post("%s: error %i %s", prefix, ret, errorDescription);
  }
}

static t_class *PhidgetClass = NULL;

typedef struct {
  t_object x_obj;

  int numInputs;

  PhidgetVoltageRatioInputHandle voltageRatioInput0;
} PhidgetObject;

static void CCONV onVoltageRatioChange(PhidgetVoltageRatioInputHandle ch,
                                       void *ctx, double voltageRatio) {
  post("VoltageRatio: %lf", voltageRatio);
  PhidgetObject *x = (PhidgetObject *)ctx;
}

static int phidget_configure(PhidgetObject *x) {
  post("phidget_configure for %i inputs", x->numInputs);
  PhidgetReturnCode ret =
      PhidgetVoltageRatioInput_create(&x->voltageRatioInput0);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "PhidgetVoltageRatioInput_create");
    return 0;
  }
  PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(
      x->voltageRatioInput0, onVoltageRatioChange, x);

  ret =
      Phidget_openWaitForAttachment((PhidgetHandle)x->voltageRatioInput0, 5000);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "Phidget_openWaitForAttachment");
    return 0;
  }
  return 1;
}

void phidget_config_msg(PhidgetObject *x) {
  post("configure Phidget");
  phidget_configure(x);
}

void phidget_bang(PhidgetObject *x) { post("bang"); }

void *phidget_new(t_symbol *s, int argc, t_atom *argv) {
  if (argc != 1) {
    pd_error(NULL, "%s requires an argument", objectName);
    return NULL;
  }
  int numInputs = atom_getint(argv);
  post("got %i inputs", numInputs);

  PhidgetObject *x = (PhidgetObject *)pd_new(PhidgetClass);
  if (x == NULL) {
    return NULL;
  }
  x->numInputs = numInputs;
  return (void *)x;
}

void phidget_free(PhidgetObject *x) {
  post("free phidget");

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
  PhidgetClass = class_new(gensym(objectName), (t_newmethod)phidget_new,
                           (t_method)phidget_free, sizeof(PhidgetObject),
                           CLASS_DEFAULT, A_GIMME, 0);
  class_addbang(PhidgetClass, phidget_bang);
  class_addmethod(PhidgetClass, (t_method)phidget_config_msg, gensym("config"),
                  0);
}
