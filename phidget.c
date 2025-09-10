#include "m_pd.h"
#include <phidget22.h>
#include <stddef.h>
#include <string.h>

static const char objectName[] = "phidget";

static void printPhidgetReturnCodeError(PhidgetReturnCode ret,
                                        const char *prefix) {
  const char *errorDescription = NULL;
  PhidgetReturnCode errCode =
      Phidget_getErrorDescription(ret, &errorDescription);
  if (errCode != EPHIDGET_OK) {
    post("%s: error %i %s", prefix, ret, errorDescription);
  }
}

static t_class *PhidgetClass = NULL;

#define MAX_INPUTS 4
typedef struct {
  t_object x_obj;
  t_outlet *outs[MAX_INPUTS];
  int numInputs;

  PhidgetVoltageRatioInputHandle voltageRatioInputs[MAX_INPUTS];
} PhidgetObject;

static void CCONV onVoltageRatioChange(PhidgetVoltageRatioInputHandle ch,
                                       void *ctx, double voltageRatio) {
  PhidgetObject *x = (PhidgetObject *)ctx;
  for (int i = 0; i < x->numInputs; i++) {
    if (x->voltageRatioInputs[i] == ch) {
      outlet_float(x->outs[i], (float)voltageRatio);
      return;
    }
  }
}

static int configureInput(PhidgetObject *x,
                          PhidgetVoltageRatioInputHandle *input, int channel) {
  post("PhidgetVoltageRatioInput_create");
  PhidgetReturnCode ret = PhidgetVoltageRatioInput_create(input);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "PhidgetVoltageRatioInput_create");
    return 0;
  }
  post("Phidget_setChannel");
  Phidget_setChannel((PhidgetHandle)*input, channel);

  post("PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler");
  PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(
      *input, onVoltageRatioChange, x);

  post("Phidget_openWaitForAttachment");
  ret = Phidget_open((PhidgetHandle)*input);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "Phidget_openWaitForAttachment");
    return 0;
  }
  return 1;
}

static int phidget_configure(PhidgetObject *x) {
  post("phidget_configure for %i inputs", x->numInputs);
  for (int i = 0; i < x->numInputs; i++) {
    post("configure input %i", i);
    if (!configureInput(x, &x->voltageRatioInputs[i], i)) {
      x->voltageRatioInputs[i] = NULL;
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
  memset(x->voltageRatioInputs, 0,
         x->numInputs * sizeof(PhidgetVoltageRatioInputHandle));
  x->numInputs = numInputs;
  for (int i = 0; i < x->numInputs; i++) {
    x->outs[i] = outlet_new(&x->x_obj, &s_float);
  }

  return (void *)x;
}

void phidget_free(PhidgetObject *x) {
  post("free phidget");
  for (int i = 0; i < x->numInputs; i++) {
    if (x->voltageRatioInputs[i] == NULL) {
      continue;
    }
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
  for (int i = 0; i < x->numInputs; i++) {
    outlet_free(x->outs[i]);
  }
}

void phidget_start_msg(PhidgetObject *x, t_floatarg arg) {
  int start = arg;
  post("start %i", start);
  for (int i = 0; i < x->numInputs; i++) {
    post("start ch %i: %i", i, start);
    PhidgetReturnCode ret = PhidgetVoltageRatioInput_setBridgeEnabled(
        x->voltageRatioInputs[i], start);
    if (ret != EPHIDGET_OK) {
      printPhidgetReturnCodeError(ret,
                                  "PhidgetVoltageRatioInput_setBridgeEnabled");
    }
  }
}

void phidget_gain_msg(PhidgetObject *x, t_floatarg arg0, t_floatarg arg1) {
  int channel = arg0;
  int gain = arg1;
  post("set gain on chan %i: %i", channel, gain);
  if (channel >= x->numInputs || channel < 0) {
    pd_error(x, "invalid channel %i", channel);
    return;
  }
  if (gain < BRIDGE_GAIN_1 || gain > BRIDGE_GAIN_128) {
    pd_error(x, "invalid gain %i", gain);
    return;
  }
  PhidgetReturnCode ret = PhidgetVoltageRatioInput_setBridgeGain(
      x->voltageRatioInputs[channel], gain);
  if (ret != EPHIDGET_OK) {
    printPhidgetReturnCodeError(ret, "PhidgetVoltageRatioInput_setBridgeGain");
  }
}

void phidget_setup(void) {
  PhidgetClass = class_new(gensym(objectName), (t_newmethod)phidget_new,
                           (t_method)phidget_free, sizeof(PhidgetObject),
                           CLASS_DEFAULT, A_GIMME, 0);
  class_addbang(PhidgetClass, phidget_bang);
  class_addmethod(PhidgetClass, (t_method)phidget_config_msg, gensym("config"),
                  0);
  class_addmethod(PhidgetClass, (t_method)phidget_start_msg, gensym("start"),
                  A_FLOAT, 0);
  class_addmethod(PhidgetClass, (t_method)phidget_gain_msg, gensym("gain"),
                  A_FLOAT, A_FLOAT, 0);
}
