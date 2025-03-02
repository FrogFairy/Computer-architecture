#include <Graph_lib/Window.h>
#include <Graph_logic.h>

#include <iostream>

int main()
try
{
    Graph_lib::Window win{window_point, window_width, window_height, "Logic_scheme"};
    Graph_logic::Source source{Graph_lib::Point(20, 20), elem_width, elem_height};
    win.attach(source);
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