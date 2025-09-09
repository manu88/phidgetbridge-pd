#include "m_pd.h"
#include <phidget22.h>

static t_class *PhidgetClass = NULL;

typedef struct {
  t_object x_obj;

  PhidgetManagerHandle manager;
} PhidgetObject;

void phidget_bang(PhidgetObject *x) { pd_post("Hello world !!"); }

void *phidget_new(void) {
  PhidgetObject *x = (PhidgetObject *)pd_new(PhidgetClass);
  PhidgetManager_create(&x->manager);
  return (void *)x;
}

void phidget_free(PhidgetObject *x) { pd_post("free phidget"); }

void phidget_setup(void) {
  PhidgetClass = class_new(gensym("phidget"), (t_newmethod)phidget_new,
                           (t_method)phidget_free, sizeof(PhidgetObject),
                           CLASS_DEFAULT, 0);
  class_addbang(PhidgetClass, phidget_bang);
}
