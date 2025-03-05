#ifndef CONSTANTS
#define CONSTANTS

#include <Graph_lib/Graph.h>

constexpr Graph_lib::Point window_point{400, 100};
constexpr int window_width = 700;
constexpr int window_height = 700;

constexpr int elem_width = 50;
constexpr int elem_height = elem_width / 2;
constexpr int margin_between_sources = (window_height - 40 - 6 * elem_height) / 4;
constexpr int margin_between_and = (window_height - 40 - 6 * elem_height) / 7;
constexpr int margin_between_or = (window_height - 40 - 6 * elem_height) / 5;
constexpr int line_width = 3;

constexpr int inversion_circle_radius = 5;

constexpr int font_size = 25;

#endif // CONSTANTS