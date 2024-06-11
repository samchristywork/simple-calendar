#include <gtk/gtk.h>

void hsv_to_rgb(double h, double s, double v, double *r, double *g, double *b) {
  if (s == 0) {
    *r = *g = *b = v;
  } else {
    h *= 6;
    if (h == 6) {
      h = 0;
    }

    int i = (int)h;
    double f = h - i;
    double p = v * (1 - s);
    double q = v * (1 - s * f);
    double t = v * (1 - s * (1 - f));

    switch (i) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    case 5:
      *r = v;
      *g = p;
      *b = q;
      break;
    }
  }
}

void cairo_set_source_hsva(cairo_t *cr, double h, double s, double v,
                           double a) {
  double r, g, b;
  hsv_to_rgb(h, s, v, &r, &g, &b);
  cairo_set_source_rgba(cr, r, g, b, a);
}
