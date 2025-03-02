#include <Logic_classes.h>

using namespace Logic;

Operation& Element::operator >>(Operation& op) 
{ 
    outputs.push_back(&op); 
    op.add_input(*this);
    return op;
}

void Element::update_state()
{
    for (Operation* op : outputs)
    {
        op->change_state();
        op->update_state();
    }
}

void Operation::add_input(Element& elem)
{
    if (inputs.size() == counts_operand) 
        throw std::runtime_error("Too much inputs. Should be " + std::to_string(counts_operand) + ".");
    inputs.push_back(&elem);
    if (inputs.size() == counts_operand) change_state();
}