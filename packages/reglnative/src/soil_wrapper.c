#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include "../SOIL/src/SOIL.h"

#define Val_none Val_int(0)

static value Val_some(value v) {
  CAMLparam1(v);
  CAMLlocal1(some);
  some = caml_alloc_small(1, 0);
  Field(some, 0) = v;
  CAMLreturn(some);
}

CAMLprim value load_image(value string, value force_channels) {
  CAMLparam2(string, force_channels);
  CAMLlocal2(record_image_data, dataArr);
  record_image_data = caml_alloc_small(4, 0);
  int width, height, channels;
  unsigned char* data = SOIL_load_image(String_val(string), &width, &height, &channels, Int_val(force_channels));
  if (data == NULL) {
    CAMLreturn (Val_none);
  } else {
    Field(record_image_data, 0) = Val_int(width);
    Field(record_image_data, 1) = Val_int(height);
    Field(record_image_data, 2) = Val_int(channels);
    // Try wrapping the `data` pointer in a box, so ctypes doesn't try to
    // dereference an unsigned char* but a boxed one.
    int size = width * height * channels;
    dataArr = caml_alloc_small(size, 0);
    for(int i = 0; i < size; i++) {
      Field(dataArr, i) = Val_int(data[i]);
    }
    Field(record_image_data, 3) = dataArr;
    SOIL_free_image_data(data);
    CAMLreturn(Val_some(record_image_data));
  }
}
