#ifndef GRAPH_LOGIC
#define GRAPH_LOGIC

#include <constants.h>
#include <Graph_lib/GUI.h>
#include <Logic_classes.h>

#include <memory>
#include <vector>

namespace Graph_logic
{
    struct Source_box : public Fl_Box
    {
        Source_box(int x, int y, int w, int h, bool state)
            : Fl_Box{FL_FRAME_BOX, x, y, w, h, ""},
            state{state}
        {
            color(state ? FL_DARK_GREEN : FL_DARK_RED);
        }
        
        int handle(int event)
        {
            switch(event)
            {
                case FL_PUSH:
                {
                    state = !state;
                    color(state ? FL_DARK_GREEN : FL_DARK_RED);
                    redraw();
                    do_callback();
                    return 1;
                }
                
                default:
                    return Fl_Widget::handle(event);
            }
        }
    
    private:
        bool state;
    };

    struct Wire : public Graph_lib::Shape
    {
        enum Wire_type
        {
            line = 0,
            upper = 1,
            lower = 2
        };

        Wire(Graph_lib::Point start, Graph_lib::Point end, Wire_type w_type, bool state, bool text);

        ~Wire() 
        {
            if (circle)
                delete circle;
            if (text)
                delete text;
        }

        void draw_lines() const override;

        Graph_lib::Point get_node_point()
        {
            return Graph_lib::Point(last_node.x + margin, last_node.y);
        }

        void set_state(bool state)
        {
            text->set_label(std::to_string(state).c_str());
        }
    
    private:
        Graph_lib::Circle* circle = nullptr;
        Graph_lib::Text* text = nullptr;
        Graph_lib::Point start;
        Graph_lib::Point end;
        Graph_lib::Point last_node;
        int margin;
        Wire_type w_type;
    };

    struct Operation;
    struct Plug {};

    struct Element : public Graph_lib::Widget
    {
        Element(Graph_lib::Point xy, int w, int h, Graph_lib::Callback cb) 
            : Graph_lib::Widget(xy, w, h, "element", cb)
        {}

        virtual ~Element() 
        { 
            delete elem;
            for (int i = 0; i < wires.size(); ++i)
                delete wires[i];
        }

        void attach(Graph_lib::Window& win) {}

        Operation& operator >>(Operation& op);
        void operator >> (Plug plug);

        Logic::Element* elem;
    
    protected:
        void update_state();

        std::vector<Element*> inputs;
        std::vector<Operation*> outputs;
        std::vector<Wire*> wires;
    };

    struct Operation : public Element
    {
        Operation(Graph_lib::Point xy, int w, int h) 
            : Element(xy, w, h, nullptr)
        {}

        ~Operation() 
        {
            for (auto el : shapes)
            {
                delete el;
            }
        }

        void attach(Graph_lib::Window& win) {}
        void add_input(Element& elem);
    
    protected:
        std::vector<Graph_lib::Shape*> shapes;
    };

    struct Source : public Element
    {
        Source(Graph_lib::Point xy, int w, int h, bool state=0)
            : Element(xy, w, h, cb_inverse_state)
        { elem = new Logic::Source(state); }

        void attach(Graph_lib::Window& win)
        {
            pw = new Source_box(loc.x, loc.y, width, height, elem->get_state());
            pw->callback(reinterpret_cast<Fl_Callback*>(do_it), this);
            pw->color(elem->get_state() ? FL_DARK_GREEN : FL_DARK_RED);
            own = &win;
        }
    
    private:
        void inverse_state()
        {
            auto* el = (Logic::Source*) elem;
            el->inverse_state();
            wires[0]->set_state(el->get_state());
            update_state();
        }

        static void cb_inverse_state(Graph_lib::Address, Graph_lib::Address addr)
        {
            auto* pb = (Source*)addr;
            pb->inverse_state();
        }
    };

    struct And : public Operation
    {
        And(Graph_lib::Point xy, int w, int h, bool inverted=0)
            : Operation(xy, w, h), and1(inverted)
        { elem = new Logic::And(inverted); }

        void attach(Graph_lib::Window& win)
        {
            // pw = new Fl_Box(FL_OVAL_BOX, loc.x, loc.y, width, height, "");
            Graph_lib::Open_polyline* rec = new Graph_lib::Open_polyline();
            rec->add(Graph_lib::Point(loc.x + width * 0.75, loc.y));
            rec->add(Graph_lib::Point(loc.x, loc.y));
            rec->add(Graph_lib::Point(loc.x, loc.y + height));
            rec->add(Graph_lib::Point(loc.x + width * 0.75, loc.y + height));

            Graph_lib::Arc* arc = new Graph_lib::Arc(Graph_lib::Point(loc.x + width / 2, loc.y), width / 2, height);
            win.attach(*rec);
            win.attach(*arc);
            shapes.push_back(rec);
            shapes.push_back(arc);
            own = &win;
        }

    private:
        Logic::And and1;
    };

    struct Or : public Operation
    {
        
    }
}

#endif // GRAPH_LOGIC