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

#define MAX_INPUTS 4
typedef struct {
  t_object x_obj;

  int numInputs;

  PhidgetVoltageRatioInputHandle voltageRatioInputs[MAX_INPUTS];
} PhidgetObject;

static void CCONV onVoltageRatioChange(PhidgetVoltageRatioInputHandle ch,
                                       void *ctx, double voltageRatio) {
  PhidgetObject *x = (PhidgetObject *)ctx;
  for (int i = 0; i < x->numInputs; i++) {
    if (x->voltageRatioInputs[i] == ch) {
      post("VoltageRatio on input %i: %lf", i, voltageRatio);
    }
  }
}

static int configureInput(PhidgetObject *x,
                          PhidgetVoltageRatioInputHandle input) {
  PhidgetReturnCode ret = PhidgetVoltageRatioInput_create(&input);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "PhidgetVoltageRatioInput_create");
    return 0;
  }
  PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(
      input, onVoltageRatioChange, x);

  ret = Phidget_openWaitForAttachment((PhidgetHandle)input, 5000);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "Phidget_openWaitForAttachment");
    return 0;
  }
  return 1;
}

static int phidget_configure(PhidgetObject *x) {
  post("phidget_configure for %i inputs", x->numInputs);
  for (int i = 0; i < x->numInputs; i++) {
    if (!configureInput(x, x->voltageRatioInputs[i])) {
      return 0;
    }
  }
  return 1;
}

void phidget_config_msg(PhidgetObject *x) {
  post("configure Phidget");
  phidget_configure(x);
}

void phidget_bang(PhidgetObject *x) {
  (void)x;
  post("bang");
}

void *phidget_new(t_symbol *s, int argc, t_atom *argv) {
  (void)s;
  if (argc != 1) {
    pd_error(NULL, "%s requires an argument", objectName);
    return NULL;
  }
  int numInputs = atom_getint(argv);
  if (numInputs > 4) {
    post("too many inputs (%i)", numInputs);
    return NULL;
  }
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
  for (int i = 0; i < x->numInputs; i++) {
    PhidgetReturnCode ret =
        Phidget_close((PhidgetHandle)x->voltageRatioInputs[i]);
    if (ret != EPHIDGET_OK) {
      printPhidgetReturnCodeError(ret, "Phidget_close");
    }
    ret = PhidgetVoltageRatioInput_delete(&x->voltageRatioInputs[i]);
    if (ret != EPHIDGET_OK) {
      printPhidgetReturnCodeError(ret, "PhidgetVoltageRatioInput_delete");
    }
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
