#ifndef IMAGE_PICKER_C_H
#define IMAGE_PICKER_C_H

#include "../core/core.h"
#include "render.h"

typedef struct{
  Point Position;
  PolyRender* images;
}ImagePicker;

ImagePicker number_picker_init(Point p, PolyRender* images);


#endif
