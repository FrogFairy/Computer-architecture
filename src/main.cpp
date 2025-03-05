#include <Graph_lib/Window.h>
#include <Graph_logic.h>

#include <iostream>

int main()
try
{
    Graph_lib::Window win{window_point, window_width, window_height, "Logic_scheme"};

    Graph_logic::GND gnd1{Graph_lib::Point(500, window_height - 20 - elem_height), elem_width / 6, elem_height};
    Graph_logic::GND gnd2{Graph_lib::Point(500, 20), elem_width / 6, elem_height};

    Graph_logic::Source A0{Graph_lib::Point(20, 20), elem_width, elem_height, "A0", "", 0, Graph_lib::Color::dark_red};
    Graph_logic::Source A1{Graph_lib::Point(20, 20 + elem_height + margin_between_sources), elem_width, elem_height, "A1", "", 0, 
                            Graph_lib::Color::dark_blue, Graph_lib::Point(20 + elem_width * 6, 20 + elem_height * 1.5 + margin_between_sources)};
    Graph_logic::Source A2{Graph_lib::Point(20, 20 + 2 * elem_height + margin_between_sources * 2), elem_width, elem_height, "A2", "", 0, 
                            Graph_lib::Color::dark_green, Graph_lib::Point(20 + elem_width * 4, 20 + elem_height / 2 * 5 + margin_between_sources * 2)};
    Graph_logic::Source A3{Graph_lib::Point(20, 20 + 3 * elem_height + margin_between_sources * 3), elem_width, elem_height, "A3", "", 0, 
                            Graph_lib::Color::dark_yellow};

    Graph_logic::Source C{Graph_lib::Point(20, window_height - 20 - elem_height), elem_width, elem_height, "C"};
    Graph_logic::Not not1{Graph_lib::Point(20 + 2 * elem_width, window_height - 20 - 2 * elem_height), elem_width, elem_height};

    Graph_logic::And and1{Graph_lib::Point(400, 20 + elem_height * 5 + margin_between_and * 5), elem_width, elem_height};
    Graph_logic::And and2{Graph_lib::Point(400, 20 + elem_height * 4 + margin_between_and * 4), elem_width, elem_height};
    Graph_logic::And and3{Graph_lib::Point(400, 20 + elem_height * 3 + margin_between_and * 3), elem_width, elem_height};
    Graph_logic::And and4{Graph_lib::Point(400, 20 + elem_height * 2 + margin_between_and * 2), elem_width, elem_height, "", 0,
                            Graph_lib::Color::black, Graph_lib::Point(400 + elem_width * 2, 20 + elem_height * 5 / 2 + margin_between_and * 2)};
    Graph_logic::And and5{Graph_lib::Point(400, 20 + elem_height + margin_between_and), elem_width, elem_height, "", 0,
                            Graph_lib::Color::black, Graph_lib::Point(400 + elem_width * 3, 20 + elem_height * 3 / 2 + margin_between_and)};
    Graph_logic::And and6{Graph_lib::Point(400, 20), elem_width, elem_height, "", 0,
                            Graph_lib::Color::black, Graph_lib::Point(400 + elem_width * 3 / 2, 20 + elem_height / 2)};

    Graph_logic::Or or1{Graph_lib::Point(600, window_height - 20 - 2 * elem_height), elem_width, elem_height, 0, "F3"};
    Graph_logic::Or or2{Graph_lib::Point(600, 20 + elem_height * 3 + margin_between_or * 3), elem_width, elem_height, 0, "F2"};
    Graph_logic::Or or3{Graph_lib::Point(600, 20 + elem_height * 2 + margin_between_or * 2), elem_width, elem_height, 0, "F1"};
    Graph_logic::Or or4{Graph_lib::Point(600, 20 + elem_height + margin_between_or), elem_width, elem_height, 0, "F0"};

    win.attach(gnd1);
    win.attach(gnd2);
    win.attach(A3);
    win.attach(A2);
    win.attach(A1);
    win.attach(A0);
    win.attach(C);
    win.attach(not1);
    win.attach(and1);
    win.attach(and2);
    win.attach(and3);
    win.attach(and4);
    win.attach(and5);
    win.attach(and6);
    win.attach(or1);
    win.attach(or2);
    win.attach(or3);
    win.attach(or4);

    C >> not1;

    A2 >> and1;
    not1 >> and1;
    and1 >> or1;
    gnd1 >> or1;
    or1 >> Graph_logic::Plug{};

    A3 >> and2;
    C >> and2;
    A1 >> and3;
    not1 >> and3;
    and3 >> or2;
    and2 >> or2;
    or2 >> Graph_logic::Plug{};

    A2 >> and5;
    C >> and5;
    A0 >> and4;
    not1 >> and4;
    and4 >> or3;
    and5 >> or3;
    or3 >> Graph_logic::Plug{};

    A1 >> and6;
    C >> and6;
    gnd2 >> or4;
    and6 >> or4;
    or4 >> Graph_logic::Plug{};

    return Graph_lib::gui_main();

    // Logic::Source source(1);
    // Logic::And and1(1);
    // Logic::Or or1;
    // Logic::Not not1;
    // Logic::Not not2;
    // source >> and1 >> or1;
    // source >> and1;
    // source >> or1;
    // source >> not1 >> not2;
    // std::cout << "Source: " << source.get_state() 
    //           << "\nAnd: " << and1.get_state() 
    //           << "\nOr: " << or1.get_state() 
    //           << "\nNot1: " << not1.get_state() 
    //           << "\nNot2: " << not2.get_state() << std::endl;
    
    // source.set_state(0);
    // std::cout << "Source: " << source.get_state() 
    //           << "\nAnd: " << and1.get_state() 
    //           << "\nOr: " << or1.get_state() 
    //           << "\nNot1: " << not1.get_state() 
    //           << "\nNot2: " << not2.get_state() << std::endl;

    return 0;
}
catch(std::runtime_error& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cerr << "Something went wrong..." << std::endl;
    return 2;
}