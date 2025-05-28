#include "format.hpp"
#include <cairo/cairo.h>

/**
 * The global variable to store all drawing items and their parameters
 */
DrawInfo drawinfo;

/**
 * Reads line parameters from input stream
 * @param code The input file stream to read from
 * @param item The DrawItem to store the line parameters
 * Format: x1 y1 x2 y2 width color
 */
void 
input_line(
  istream& code,
  DrawItem *item
) {
  // Start point (params[0], params[1]) 
  // End point (params[2], params[3])
  for (int i = 0; i < 4; i++) {
    code >> item->params[i];
  }
  // Width (params[6])
  code >> item->params[6];
  // Color 
  code >> item->colorParams;
  item->colorParams = item->colorParams.substr(1);
}

/**
 * Reads circle parameters from input stream
 * @param code The input file stream to read from
 * @param item The DrawItem to store the circle parameters
 * Format: centerX centerY radius color
 */
void 
input_circ(
  istream& code,
  DrawItem *item
) {
  // Center point (params[0], params[1]) 
  for (int i = 0; i < 2; i++) {
    code >> item->params[i];
  }
  // Radius (params[6])
  code >> item->params[6];
  // Color 
  code >> item->colorParams;
  item->colorParams = item->colorParams.substr(1);
}

/**
 * Reads triangle parameters from input stream
 * @param code The input file stream to read from
 * @param item The DrawItem to store the triangle parameters
 * Format: x1 y1 x2 y2 x3 y3 color
 */
void 
input_tria(
  istream& code,
  DrawItem *item
) {
  // Points (params[0-5])
  for (int i = 0; i < 6; i++) {
    code >> item->params[i];
  }
  // Color 
  code >> item->colorParams;
  item->colorParams = item->colorParams.substr(1);
}

/**
 * Reads rectangle parameters from input stream
 * @param code The input file stream to read from
 * @param item The DrawItem to store the rectangle parameters
 * Format: x1 y1 x2 y2 color
 */
void 
input_rect(
  istream& code,
  DrawItem *item
) {
  // Points (params[0-3])
  for (int i = 0; i < 4; i++) {
    code >> item->params[i];
  }
  // Color 
  code >> item->colorParams;
  item->colorParams = item->colorParams.substr(1);
}

/**
 * Reads drawing commands from a file and stores them in DrawInfo
 * @param fileName Name of the file to read commands from
 */
void 
input(
  string fileName
) {

  fstream code(fileName, ios::in);
  if (!code.is_open()) {
    cout << "Could not open file!" << endl;
  } else {
    string opt;
    while (code >> opt) {
      drawinfo.push_back((DrawItem) { opt });
      if (opt == "line") input_line(code, &drawinfo.back());
      if (opt == "circ") input_circ(code, &drawinfo.back());
      if (opt == "tria") input_tria(code, &drawinfo.back());
      if (opt == "rect") input_rect(code, &drawinfo.back());
    }
    code.close();
  }
}

/**
 * Draws a line on the cairo surface
 * @param cr Cairo context to draw on
 * @param item DrawItem containing line parameters
 */
void 
draw_line(
  cairo_t *cr,
  DrawItem *item
) {
  cairo_set_line_width(cr, item->params[6]);
    
  int r, g, b;
  sscanf(item->colorParams.c_str(), "%02x%02x%02x", &r, &g, &b);
  cairo_set_source_rgb(cr, r / 255.0, g / 255.0, b / 255.0);
    
  cairo_move_to(cr, item->params[0], item->params[1]);
  cairo_line_to(cr, item->params[2], item->params[3]);
  cairo_stroke(cr);
}

/**
 * Draws a circle on the cairo surface
 * @param cr Cairo context to draw on
 * @param item DrawItem containing circle parameters
 */
void 
draw_circ(
  cairo_t *cr,
  DrawItem *item
) {
  int r, g, b;
  sscanf(item->colorParams.c_str(), "%02x%02x%02x", &r, &g, &b);
  cairo_set_source_rgb(cr, r / 255.0, g / 255.0, b / 255.0);
    
  cairo_arc(
    cr, 
    item->params[0], 
    item->params[1], 
    item->params[6],
    0, 
    2 * M_PI
  );
  cairo_fill(cr);
}

/**
 * Draws a triangle on the cairo surface
 * @param cr Cairo context to draw on
 * @param item DrawItem containing triangle parameters
 */
void 
draw_tria(
  cairo_t *cr,
  DrawItem *item
) {
  int r, g, b;
  sscanf(item->colorParams.c_str(), "%02x%02x%02x", &r, &g, &b);
  cairo_set_source_rgb(cr, r / 255.0, g / 255.0, b / 255.0);
    
  cairo_move_to(cr, item->params[0], item->params[1]);
  cairo_line_to(cr, item->params[2], item->params[3]);
  cairo_line_to(cr, item->params[4], item->params[5]);
  cairo_close_path(cr);
  cairo_fill(cr);
}

/**
 * Draws a rectangle on the cairo surface
 * @param cr Cairo context to draw on
 * @param item DrawItem containing rectangle parameters
 */
void 
draw_rect(
  cairo_t *cr,
  DrawItem *item
) {
  int r, g, b;
  sscanf(item->colorParams.c_str(), "%02x%02x%02x", &r, &g, &b);
  cairo_set_source_rgb(cr, r / 255.0, g / 255.0, b / 255.0);
    
  cairo_rectangle(
    cr, 
    item->params[0], 
    item->params[1],
    item->params[2] - item->params[0],
    item->params[3] - item->params[1]
  );
  cairo_fill(cr);
}

/**
 * Creates a PNG image with all shapes from DrawInfo
 * @param width Width of the output image
 * @param height Height of the output image
 * @param ouName Output filename
 */
void 
draw(
  int width,
  int height,
  string ouName
) {

  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(surface);

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);

  for (DrawItem item: drawinfo) {
    if(item.itemName == "line") draw_line(cr, &item);
    if(item.itemName == "circ") draw_circ(cr, &item);
    if(item.itemName == "tria") draw_tria(cr, &item);
    if(item.itemName == "rect") draw_rect(cr, &item);
  }

  ouName += ".png";
  cairo_surface_write_to_png(surface, ouName.c_str());

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

int width, height;
string ouName;

int 
main(
  int argc, 
  char* argv[]
) {

  if (argc < 4) exit(0);
  width = abs(atoi(argv[1]));
  height = abs(atoi(argv[2]));
  ouName = string(argv[3]);

  string opt;
  while (cin >> opt) {
    drawinfo.push_back((DrawItem) { opt });
    if (opt == "line") input_line(cin, &drawinfo.back());
    if (opt == "circ") input_circ(cin, &drawinfo.back());
    if (opt == "tria") input_tria(cin, &drawinfo.back());
    if (opt == "rect") input_rect(cin, &drawinfo.back());
  }

  draw(width, height, ouName);
  return 0;
}