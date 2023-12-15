#include <iostream>
#include <future>
#include <string>
#include <syncstream>
#include <chrono>
using namespace std;

//Усі обчислення: A,B,C1,C2,D1,D2,F
//“Повільні” обчислення:B,C2
//Залежності: B(A), C1(B), D1(C1,C2),F(D1,D2)

void slow (const string& name){
    this_thread::sleep_for(chrono::seconds(7));
    osyncstream(cout) << name << '\n';
}

void quick (const string& name){
    this_thread::sleep_for(chrono::seconds(1));
    osyncstream(cout) << name << '\n';
}
void quickABC1() {
    quick("A");
    slow("B");
    quick("C1");
}
void work()
{
    auto now = chrono::system_clock::now();
    auto res1 = async(launch::async, quickABC1);
    slow("C2");
    quick("D2");
    res1.get();
    quick("D1");
    quick("F");
    osyncstream(cout) << "Work is done!" << format("{:%S}", chrono::system_clock::now() - now) <<'\n';
}

int main() {
    work();
    return 0;
}
