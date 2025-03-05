#include <Graph_logic.h>

using namespace Graph_logic;

Wire::Wire(Graph_lib::Point start, Graph_lib::Point end, Wire_type w_type = line, bool state = 0, bool text = true)
    : start{start}, end{end}, w_type{w_type}
{
    if (w_type == upper)
    {
        add(start);
        add(Graph_lib::Point(start.x + (end.x - start.x) * 0.9, start.y));
        add(Graph_lib::Point(start.x + (end.x - start.x) * 0.9, end.y));
        add(end);
        last_node = start;
        margin = (point(1).x - point(0).x) / 10;
    }
    else if (w_type == lower)
    {
        circle = new Graph_lib::Circle(start, 5);
        add(start);
        add(Graph_lib::Point(start.x, end.y));
        add(end);
        circle->set_fill_color(Graph_lib::Color::black);
        last_node = point(1);
        margin = (end.x - point(1).x) / 10;
    }
    else
    {
        add(start);
        add(end);
        last_node = start;
        margin = (end.x - start.x) / 10;
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
    Graph_lib::Point end(op.loc.x, op.loc.y + height / (counts_operand + 1) * op.inputs.size());
    Wire* wire = nullptr;
    if (outputs.size() == 1)
    {
        Graph_lib::Point start(this->loc.x + width, this->loc.y + height / 2);
        wire = new Wire(start, end, Wire::upper, this->elem->get_state(), true);
    }
    else
    {
        wire = new Wire(wires[0]->get_node_point(), end, Wire::lower, this->elem->get_state(), false);
    }
    own->attach(*wire);
    wires.push_back(wire);

    return op;
}

void Element::operator >>(Plug plug)
{
    Graph_lib::Point start(loc.x + width, loc.y + height / 2);
    Graph_lib::Point end(start.x + 50, start.y);
    Wire* wire = new Wire(start, end, Wire::line, this->elem->get_state(), true);
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