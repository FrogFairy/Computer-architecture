#ifndef GRAPH_LOGIC
#define GRAPH_LOGIC

#include <constants.h>
#include <Graph_lib/GUI.h>
#include <Logic_classes.h>

#include <memory>

namespace Graph_logic
{
    struct Element : public Graph_lib::Widget
    {
        Element(Graph_lib::Point xy, int w, int h, bool state = 0) 
            : Graph_lib::Widget(xy, w, h, "element", nullptr)
        {}

        void attach(Graph_lib::Window& win)
        {
            win.attach();
        }
    };

    struct Source : public Element
    {
        Source(Graph_lib::Point xy, int w, int h, bool state = 0)
            : Element{xy, w, h, state}, source{state},
            button{Graph_lib::Button(xy, w, h, std::to_string(state), cb_inverse_state)}
        {
            frame.set_color((state ? Graph_lib::Color::dark_green : Graph_lib::Color::dark_red));
        }
        
        void attach(Graph_lib::Window& win)
        {
            Element::attach(win);
            button.attach(win);
            button.set_font_size(font_size);
            button.set_widget(this);
        }
    
    private:
        Logic::Source source;
        Graph_lib::Button button;

        void inverse_state();
        
        static void cb_inverse_state(Graph_lib::Address, Graph_lib::Address addr)
        {
            auto* pb = (Graph_lib::Button*)addr;
            Source& source = (Source&)(pb->widget());
            source.inverse_state();
        }
    };
}

#endif // GRAPH_LOGIC