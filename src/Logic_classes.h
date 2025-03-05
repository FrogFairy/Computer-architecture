#ifndef LOGIC_CLASSES
#define LOGIC_CLASSES

#include <vector>
#include <stdexcept>

namespace Logic
{
    struct Operation;
    
    struct Element
    {
        Element(bool state = 0) : state{state} {}

        Operation& operator >>(Operation& op);

        bool get_state() const { return state; }

        void update_state();
        
    protected:
        virtual void change_state() {}

        std::vector<Element*> inputs {};
        std::vector<Operation*> outputs {};
        bool state = 0;
    };

    struct Source : public Element
    {
        Source(bool state = 0) : Element(state) {}

        void inverse_state() 
        { 
            state = !state;
            update_state();
        }
    };

    struct Operation : public Element
    {
        Operation(bool inverted = 0, int counts_operand = 2) 
            : Element(), inverted{inverted}, counts_operand{counts_operand} {}

        void add_input(Element& elem);
        
        int get_counts_operand() { return counts_operand; }
        int is_invert() { return inverted; }
    
    protected:
        bool inverted = 0;
        int counts_operand = 2;
    };

    struct And : public Operation
    {
        And(bool inverted = 0) : Operation(inverted, 2) {}
    
    protected:
        void change_state() override
        {
            state = !((inputs[0]->get_state() && inputs[1]->get_state()) == inverted);
        }
    };

    struct Or : public Operation
    {
        Or(bool inverted = 0) : Operation(inverted, 2) {}
    
    protected:
        void change_state() override
        {
            state = !((inputs[0]->get_state() || inputs[1]->get_state()) == inverted);
        }
    };

    struct Not : public Operation
    {
        Not() : Operation(1, 1) {}
    
    protected:
        void change_state() override
        {
            state = !(inputs[0]->get_state());
        }
    };

    struct GND : public Element
    {
        GND() : Element() {}
    };
}

#endif // LOGIC_CLASSES