#include <iostream>
#include <fstream>

std::ofstream debug_log("debug_log.txt");

#define DEBUG_PROGRAMM 
#include "odyssey.h"

int main () {
    
    odyssey::utility::graph<int, int> g1;
    odyssey::utility::graph<int, int> g2;
    
    g1.push_vertex(10);
    g1.push_vertex(20);
    g1.emplace_incidence(100, 0, 1);
    
    g1.pop_vertex();
    g1.pop_vertex();
    
    return 0;
}