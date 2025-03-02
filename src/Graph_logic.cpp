#include <Graph_logic.h>

using namespace Graph_logic;

void Source::inverse_state()
{
    source.inverse_state();
    bool val = source.get_state();
    frame->set_color((val ? Graph_lib::Color::dark_green : Graph_lib::Color::dark_red));
    button.set_label(std::to_string(val));
}