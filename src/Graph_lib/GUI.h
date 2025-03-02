// This is a GUI support code to the chapters 12-16 of the book
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//
#ifndef GUI_GUARD
#define GUI_GUARD

#include "Graph.h"
#include "Window.h"

namespace Graph_lib {

//------------------------------------------------------------------------------

using Address = void*;                            // Address is a synonym for void*
using Callback = void (*)(Address, Address);      // FLTK's required function type for all callbacks
using Callback_for_file = void (*) (Window*, const std::string&); // Function for return a string - the path to the file - in window 

//------------------------------------------------------------------------------

template <class W> W& reference_to (Address pw)
// treat an address as a reference to a W
{
  return *static_cast<W*>(pw);
}

//------------------------------------------------------------------------------

class Widget
{
  // Widget is a handle to an Fl_widget - it is *not* an Fl_widget
  // We try to keep our interface classes at arm's length from FLTK

public:
  Widget(Point xy, int w, int h, const std::string& s, Callback cb) : loc{xy}, width{w}, height{h}, label{s}, do_it{cb}
  {
  }

  virtual void move (int dx, int dy)
  {
    hide();
    pw->position(loc.x += dx, loc.y += dy);
    show();
  }

  virtual void hide () { pw->hide(); }

  virtual void show () { pw->show(); }

  virtual void redraw () { pw->redraw(); }

  virtual void set_color(Color back_color, Color frame_color) { pw->color(back_color.as_int(), frame_color.as_int()); }
  virtual void set_fill_color(Color color) { pw->color2(color.as_int()); }

  virtual void set_label(const std::string& label) { pw->label(label.c_str()); }

  virtual void set_font_size(int fnt_size) { pw->labelsize(fnt_size); }

  virtual void set_label_color(Graph_lib::Color color) { pw->labelcolor(color.as_int()); }

  virtual void attach (Window&) = 0;

  Window& window () { return *own; }
  Widget& widget () { return *own_widget; }

  Point loc;
  int width;
  int height;
  std::string label;
  Callback do_it;

  virtual ~Widget() {}

  virtual void clean_pointer() { delete pw; }

  // Widget& operator= (const Widget&) {};  // don't copy Widgets
  // Widget(const Widget&) {};
  Fl_Widget* pw;  // connection to the FLTK Widget

protected:
  Window* own;    // every Widget belongs to a Window
  Widget* own_widget;
  
};

//------------------------------------------------------------------------------

struct Button : public Widget
{
  Button(Point xy, int w, int h, const std::string& label, Callback cb) : Widget{xy, w, h, label, cb} {}

  void attach (Window&);

  void set_widget(Widget* widget) { own_widget = widget; }
};

//------------------------------------------------------------------------------

struct In_box : Widget
{
  In_box(Point xy, int w, int h, const std::string& s) : Widget{xy, w, h, s, nullptr} {}

  int get_int ();
  std::string get_string ();

  void set_font_size(int s) 
  { 
    pw->labelsize(s);
    ((Fl_Input*) pw)->textsize(s); 
  }

  void attach (Window& win);
};

//------------------------------------------------------------------------------

struct Out_box : Widget
{
  Out_box(Point xy, int w, int h, const std::string& s) : Widget{xy, w, h, s, nullptr} {}

  void put (int);
  void put (const std::string&);

  void set_font_size(int s) 
  { 
    pw->labelsize(s);
    ((Fl_Output*) pw)->textsize(s); 
  }

  void attach (Window& win);
};

//------------------------------------------------------------------------------

struct Menu : Widget
{
  enum Kind
  {
    horizontal,
    vertical
  };

  Menu(Point xy, int w, int h, Kind kk, const std::string& label) : Widget{xy, w, h, label, nullptr}, k{kk}, offset{0}, margin{20}
  {
  }

  Vector_ref<Button> selection;
  Kind k;
  int offset;
  int margin;

  int attach (Button& b);  // Menu does not delete &b
  int attach (Button* p);  // Menu deletes p

  void show ()  // show all buttons
  {
    for (int i = 0; i < selection.size(); ++i)
      selection[i].show();
  }

  void hide () // hide all buttons
  {
    for (int i = 0; i < selection.size(); ++i)
      selection[i].hide();
  }

  void move (int dx, int dy)  // move all buttons
  {
    for (int i = 0; i < selection.size(); ++i)
      selection[i].move(dx, dy);
  }

  void attach (Window& win)  // attach all buttons
  {
    for (int i = 0; i < selection.size(); ++i)
      win.attach(selection[i]);
    own = &win;
  }

  void set_font_size(int s)
  {
    for (int i = 0; i < selection.size(); ++i)
      selection[i].set_font_size(s);
  }
};

//------------------------------------------------------------------------------

struct Choice_box : Widget
{
  Choice_box(Point xy, int w, int h, const std::string& label) : Widget{xy, w, h, label, nullptr} {}
  void attach (Window&);
  void add(const std::string& option);
  int get_value();
  void set_value(int ind);

  void set_font_size(int s) 
  { 
    pw->labelsize(s);
    ((Fl_Choice*) pw)->textsize(s); 
  }
};

//------------------------------------------------------------------------------

struct File_chooser_box : Widget
{
   enum State
  {
    OK,
    ERR
  };

  File_chooser_box(Point xy, int w, int h, const std::string& label, const std::string& btn_label, const std::string& files,
                   Callback_for_file window_callback, Callback button_callback) 
    : Widget{xy, w, h, label, nullptr}, btn{Button(Point(xy.x - w / 3 * 2, xy.y), w / 3 * 2, h, btn_label, button_callback)}, files{files},
    window_callback{window_callback}, out_box{Out_box(xy, w, h, label)},
	  err_rectangle{Point{xy.x - margin, xy.y - margin}, w + 2 * margin, h + 2 * margin}
	{
		err_rectangle.set_color(FL_BACKGROUND_COLOR);
    err_rectangle.set_style(Graph_lib::Line_style(Graph_lib::Line_style::solid, 3));
	}

  void set_font_size(int s)
  {
    btn.set_font_size(s);
    out_box.set_font_size(s);
  }
  
  Button btn;
  Out_box out_box;
  Rectangle err_rectangle;

  int margin = 3;

  Fl_File_Chooser *chooser;
  std::string files;
  
  Callback_for_file window_callback;

  void choose_file();
  void change_state(State state);
  void attach(Window&);
};

//------------------------------------------------------------------------------

struct Label_widget : Widget
{
public:
    Label_widget(Graph_lib::Point xy, const std::string& text, int w = 300, int h = 20)
        : Widget{xy, w, h, "", nullptr}, text{text} 
    {}
    
    void attach (Window& win) override
    {
      pw = new Fl_Box(FL_NO_BOX, loc.x, loc.y, width, height, text.c_str());
      own = &win;
    }

    int length()
    {
      return text.length();
    }

private:
    std::string text;
};

struct Scrollable_Menu : Widget {
public:
    Scrollable_Menu(Point xy, int w, int h, Menu& m)
        : Widget{ xy, w, h, m.label, nullptr }, menu{ m }
    {
    }

    Menu& menu;

    int attach(Button& b) { return menu.attach(b); }
    int attach(Button* p) { return menu.attach(p); }

    void attach(Window& win) override {
        Fl_Scroll* scroll = new Fl_Scroll(loc.x, loc.y, width, height);
        scroll->box(FL_FLAT_BOX);
        scroll->type(Fl_Scroll::VERTICAL);

        for (int i = 0; i < menu.selection.size(); ++i) {
            win.attach(menu.selection[i]);
            scroll->add(menu.selection[i].pw);
        }

        pw = scroll;
        own = &win;
    }

    void show() { menu.show(); }
    void hide() { menu.hide(); }
    void move(int dx, int dy) { menu.move(dx, dy); }
    void set_font_size(int s) { menu.set_font_size(s); }
};

}  // namespace Graph_lib

#endif  // GUI_GUARD