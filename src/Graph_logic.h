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

        Wire(Graph_lib::Point start, Graph_lib::Point end, Wire_type w_type, 
            bool state, bool text, Graph_lib::Color color, Graph_lib::Point must_point);

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
            last_node = Graph_lib::Point(last_node.x + margin, last_node.y);
            return Graph_lib::Point(last_node.x - margin, last_node.y);
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
        Graph_lib::Point must_point;
        double margin;
        Wire_type w_type;
    };

    struct Operation;
    struct Plug {};

    struct Element : public Graph_lib::Widget
    {
        Element(Graph_lib::Point xy, int w, int h, Graph_lib::Callback cb, std::string out_text = "",
            Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1)) 
            : Graph_lib::Widget(xy, w, h, "element", cb), color{color}, must_point{must_point}, out_text{out_text}
        {}

        virtual ~Element() 
        { 
            delete elem;
            for (int i = 0; i < wires.size(); ++i)
                delete wires[i];
            for (auto el : shapes)
            {
                delete el;
            }
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
        std::vector<Graph_lib::Shape*> shapes;
    
    private:
        Graph_lib::Color color;
        Graph_lib::Point must_point;
        std::string out_text;
    };

    struct Operation : public Element
    {
        Operation(Graph_lib::Point xy, int w, int h, std::string out_text = "",
            Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1)) 
            : Element(xy, w, h, nullptr, out_text, color, must_point)
        {}

        void add_input(Element& elem);
    
    protected:
        virtual void attach(Graph_lib::Window& win) {}
        void attach_inv_circle();
    
    private:
        std::string out_text;
    };

    struct Source : public Element
    {
        Source(Graph_lib::Point xy, int w, int h, std::string label, std::string out_text = "", bool state=0, 
            Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1))
            : text{label}, Element(xy, w, h, cb_inverse_state, out_text, color, must_point)
        { elem = new Logic::Source(state); }

        void attach(Graph_lib::Window& win)
        {
            pw = new Source_box(loc.x, loc.y, width, height, elem->get_state());
            pw->callback(reinterpret_cast<Fl_Callback*>(do_it), this);
            pw->color(elem->get_state() ? FL_DARK_GREEN : FL_DARK_RED);
            own = &win;

            Graph_lib::Text* label_text = new Graph_lib::Text(Graph_lib::Point((loc.x + width) / 2 - 2, loc.y - 3), text);
            label_text->set_font_size(font_size / 6 * 5);
            shapes.push_back(label_text);
            win.attach(*label_text);
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

        std::string text;
    };

    struct And : public Operation
    {
        And(Graph_lib::Point xy, int w, int h, std::string out_text = "", bool inverted=0, 
            Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1))
            : Operation(xy, w, h, out_text, color, must_point)
        { elem = new Logic::And(inverted); }

        void attach(Graph_lib::Window& win);
    };

    struct Or : public Operation
    {
        Or(Graph_lib::Point xy, int w, int h, bool inverted=0, std::string out_text = "", 
            Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1))
            : Operation(xy, w, h, out_text, color, must_point)
        { elem = new Logic::Or(inverted); }

        void attach(Graph_lib::Window& win);
    };

    struct Not : public Operation
    {
        Not(Graph_lib::Point xy, int w, int h, std::string out_text = "",
            Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1))
            : Operation(xy, w, h, out_text, color, must_point)
        { elem = new Logic::Not(); }

        void attach(Graph_lib::Window& win);
    };

    struct GND : public Element
    {
        GND(Graph_lib::Point xy, int w, int h,
            Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1))
            : Element(xy, w, h, nullptr, "", color, must_point)
        { elem = new Logic::GND(); }

        void attach(Graph_lib::Window& win);
    };
}

#endif // GRAPH_LOGIC