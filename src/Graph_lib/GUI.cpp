#include <sstream>
#include <string>
#include <iostream>

#include "GUI.h"

using namespace Graph_lib;

void Button::attach(Window& win)
{
  pw = new Fl_Button{loc.x, loc.y, width, height, label.c_str()};
  if (do_it)
    pw->callback(reinterpret_cast<Fl_Callback*>(do_it), this);  // pass this widget
  own = &win;
}

int In_box::get_int()
{
  Fl_Input& pi = reference_to<Fl_Input>(pw);
  const char* p = pi.value();
  if (!isdigit(p[0]))
    return -999999;
  return atoi(p);
}

std::string In_box::get_string()
{
  Fl_Input& pi = reference_to<Fl_Input>(pw);
  return std::string(pi.value());
}

void In_box::attach(Window& win)
{
  pw = new Fl_Input{loc.x, loc.y, width, height, label.c_str()};
  own = &win;
}

void Out_box::put(int i)
{
  Fl_Output& po = reference_to<Fl_Output>(pw);
  std::stringstream ss;
  ss << i;
  po.value(ss.str().c_str());
}

void Out_box::put(const std::string& s) { reference_to<Fl_Output>(pw).value(s.c_str()); }

void Out_box::attach(Window& win)
{
  pw = new Fl_Output{loc.x, loc.y, width, height, label.c_str()};
  own = &win;
}

int Menu::attach(Button& b)
{
  b.width = width;
  b.height = height;

  switch (k)
  {
  case horizontal:
    b.loc = Point{loc.x + offset, loc.y};
    offset += b.width + margin;
    break;
  case vertical:
    b.loc = Point{loc.x, loc.y + offset};
    offset += b.height + margin;
    break;
  }
  selection.push_back(&b);
  return int(selection.size() - 1);
}

int Menu::attach(Button* p)
{
  //  owned.push_back(p);
  return attach(*p);
}

void Choice_box::attach(Window& win)
{
   pw = new Fl_Choice(loc.x, loc.y, width, height, label.c_str());
   pw->callback(reinterpret_cast<Fl_Callback*>(do_it), this);  // pass this widget
   own = &win;
}

void Choice_box::add(const std::string& option)
{
  static_cast<Fl_Choice*>(pw)->add(option.c_str());
}

int Choice_box::get_value()
{
  return static_cast<Fl_Choice*>(pw)->value();
}

void Choice_box::set_value(int ind)
{
  static_cast<Fl_Choice*>(pw)->value(ind);
}

void File_chooser_box::attach(Window& win)
{
  win.attach(btn);
  win.attach(out_box);
  win.attach(err_rectangle);
  own = &win;
}

void File_chooser_box::choose_file()
{
  Fl_File_Chooser *chooser = new Fl_File_Chooser{".", files.c_str(), 0, "Files"};
  chooser->show();

  while (chooser->shown())
  {
    Fl::wait();
  }
  
  if (chooser->value())
  {
    out_box.put(chooser->value());
    window_callback(own, chooser->value());
  }
}

void File_chooser_box::change_state(State state)
{
  if(state == ERR)
  {
    err_rectangle.set_color(FL_RED);
  }
  else
  {
    err_rectangle.set_color(FL_BACKGROUND_COLOR);
  }
}