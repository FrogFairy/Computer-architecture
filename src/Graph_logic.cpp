#include <Graph_logic.h>

using namespace Graph_logic;

Wire::Wire(Graph_lib::Point start, Graph_lib::Point end, Wire_type w_type = line, bool state = 0, 
    bool text = true, Graph_lib::Color color = Graph_lib::Color::black, Graph_lib::Point must_point = Graph_lib::Point(-1, -1))
    : start{start}, end{end}, w_type{w_type}, must_point{must_point}
{
    set_color(color);
    set_style(Graph_lib::Line_style(line_width));
    if (must_point.x > 0)
    {
        add(start);
        add(must_point);
        add(Graph_lib::Point(must_point.x, end.y));
        add(end);
        margin = (point(1).x - point(0).x) / 5;
        last_node = Graph_lib::Point(start.x + margin, start.y);
    }
    else if (w_type == upper)
    {
        add(start);
        add(Graph_lib::Point(start.x + (end.x - start.x) * 0.9, start.y));
        add(Graph_lib::Point(start.x + (end.x - start.x) * 0.9, end.y));
        add(end);
        margin = (point(1).x - point(0).x) / 5;
        last_node = Graph_lib::Point(start.x + margin, start.y);
    }
    else if (w_type == lower)
    {
        circle = new Graph_lib::Circle(start, 5);
        add(start);
        add(Graph_lib::Point(start.x, end.y));
        add(end);
        circle->set_fill_color(Graph_lib::Color::black);
        margin = (end.x - point(1).x) / 5;
        last_node = Graph_lib::Point(point(1).x + margin, point(1).y);
    }
    else if (w_type == line)
    {
        add(start);
        add(end);
        margin = (end.x - start.x) / 5;
        last_node = Graph_lib::Point(start.x + margin, start.y);
    }

    if (text)
    {
        this->text = new Graph_lib::Text(Graph_lib::Point(start.x + 5, start.y - 10), std::to_string(state).c_str());
        this->text->set_font_size(font_size);
    }
}

void Wire::draw_lines() const
{
    if (color().visibility() && 1 < point().size())
        for (unsigned int i = 1; i < point().size(); ++i)
            fl_line(point(i - 1).x, point(i - 1).y, point(i).x, point(i).y);
    if (circle)
        circle->draw_lines();
    if (text)
        text->draw_lines();
}

Operation& Element::operator >>(Operation& op)
{
    auto* op_logic = (Logic::Operation*)(op.elem);
    *(this->elem)>>*op_logic;
    outputs.push_back(&op);
    op.add_input(*this);

    int counts_operand = op_logic->get_counts_operand();
    Graph_lib::Point end(op.loc.x, op.loc.y + op.height / (counts_operand + 1) * op.inputs.size());
    Wire* wire = nullptr;
    if (outputs.size() == 1)
    {
        Graph_lib::Point start(this->loc.x + width, this->loc.y + height / 2);
        wire = new Wire(start, end, Wire::upper, this->elem->get_state(), true, color, must_point);
    }
    else
    {
        wire = new Wire(wires[0]->get_node_point(), end, Wire::lower, this->elem->get_state(), false, color);
    }
    own->attach(*wire);
    wires.push_back(wire);

    return op;
}

void Element::operator >>(Plug plug)
{
    Graph_lib::Point start(loc.x + width, loc.y + height / 2);
    Graph_lib::Point end(start.x + 50, start.y);
    Wire* wire = new Wire(start, end, Wire::line, this->elem->get_state(), true, color, must_point);
    Graph_lib::Text* out_label = new Graph_lib::Text(Graph_lib::Point(start.x + 5, start.y + 25), out_text);
    out_label->set_font_size(font_size);
    shapes.push_back(out_label);
    own->attach(*out_label);
    own->attach(*wire);
    wires.push_back(wire);
}

void Element::update_state()
{
    for (Operation* op : outputs)
    {
        op->wires[0]->set_state(op->elem->get_state());
        own->redraw();
        op->elem->update_state();
    }
}

void Operation::add_input(Element& el)
{
    auto* op_logic = (Logic::Operation*)(this->elem);

    try
    {
        Logic::Element* el_elem = el.elem;
        op_logic->add_input(*el_elem);
    }
    catch(const std::exception& e)
    {
        throw;
    }
    
    inputs.push_back(&el);
}

void Operation::attach_inv_circle()
{
    Logic::Operation* op = (Logic::Operation*)(elem);
    if (op->is_invert())
    {
        Graph_lib::Circle* circle = new Graph_lib::Circle(Graph_lib::Point(loc.x + width, loc.y + height / 2), 
                                                            inversion_circle_radius);
        own->attach(*circle);
        shapes.push_back(circle);
    }
}

void And::attach(Graph_lib::Window& win)
{
    Graph_lib::Open_polyline* rec = new Graph_lib::Open_polyline();
    rec->add(Graph_lib::Point(loc.x + width * 0.75, loc.y));
    rec->add(Graph_lib::Point(loc.x, loc.y));
    rec->add(Graph_lib::Point(loc.x, loc.y + height));
    rec->add(Graph_lib::Point(loc.x + width * 0.75, loc.y + height));

    Graph_lib::Arc* arc = new Graph_lib::Arc(Graph_lib::Point(loc.x + width / 2, loc.y), width / 2, height, -100, 100);

    own = &win;
    attach_inv_circle();
    win.attach(*rec);
    win.attach(*arc);
    shapes.push_back(rec);
    shapes.push_back(arc);
}

void Or::attach(Graph_lib::Window& win)
{
    Graph_lib::Arc* arc1 = new Graph_lib::Arc(Graph_lib::Point(loc.x - width / 2 + 2, loc.y), width / 2, height, -80, 80);
    Graph_lib::Lines* lines = new Graph_lib::Lines(std::initializer_list<Graph_lib::Point> 
                                                {Graph_lib::Point(loc.x - 11, loc.y), 
                                                Graph_lib::Point(loc.x + 11 + width / 3, loc.y),
                                                Graph_lib::Point(loc.x - 11, loc.y + height), 
                                                Graph_lib::Point(loc.x + width / 3, loc.y + height)});
    Graph_lib::Arc* arc2 = new Graph_lib::Arc(Graph_lib::Point(loc.x - width / 3, loc.y), width / 3 * 4, height, -100, 100);

    own = &win;
    attach_inv_circle();
    win.attach(*arc1);
    win.attach(*lines);
    win.attach(*arc2);
    shapes.push_back(arc1);
    shapes.push_back(lines);
    shapes.push_back(arc2);
}

void Not::attach(Graph_lib::Window& win)
{
    Graph_lib::Closed_polyline* lines = new Graph_lib::Closed_polyline();
    lines->add(Graph_lib::Point(loc.x, loc.y));
    lines->add(Graph_lib::Point(loc.x, loc.y + height));
    lines->add(Graph_lib::Point(loc.x + width, loc.y + height / 2));

    own = &win;
    attach_inv_circle();
    win.attach(*lines);
    shapes.push_back(lines);
}

void GND::attach(Graph_lib::Window& win)
{
    Graph_lib::Lines* lines = new Graph_lib::Lines(std::initializer_list<Graph_lib::Point>
                                                    {Graph_lib::Point(loc.x, loc.y + height / 3),
                                                    Graph_lib::Point(loc.x, loc.y + height / 3 * 2),
                                                    Graph_lib::Point(loc.x + width / 2, loc.y + height / 6),
                                                    Graph_lib::Point(loc.x + width / 2, loc.y + height / 6 * 5),
                                                    Graph_lib::Point(loc.x + width, loc.y),
                                                    Graph_lib::Point(loc.x + width, loc.y + height)});
    
    own = &win;
    win.attach(*lines);
    shapes.push_back(lines);
}