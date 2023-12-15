#include <iostream>
#include <string>
#include <syncstream>
#include <format>
#include <thread>
#include <barrier>

using namespace std;


void synchronizedOut(const string &s)
{
    osyncstream(cout) << s;
}
void completion() noexcept
{
    synchronizedOut("the phase is over\n");
}

void f(char ch, int i){
    synchronizedOut(format("З набору {} виконано д?ю {}\n",ch,i));
}

barrier workDone(4, completion);



void worker1(){
    f('a',1);
    f('b',1);
    f('b',5);
    f('c',1);
    f('c',5);
    workDone.arrive_and_wait();
    f('d',1);
    f('d',5);
    f('e',3);
    f('e',7);
    f('f',1);
    f('f',5);
    workDone.arrive_and_wait();
    f('g',1);
    f('g',5);
    f('h',3);
    f('h',7);
    f('i',1);
    f('i',5);
    workDone.arrive_and_wait();
    f('j',1);
    f('j',5);
    f('j',9);
    f('k',4);
}
void worker2(){
    f('a',2);
    f('b',2);
    f('b',6);
    f('c',2);
    f('c',6);
    workDone.arrive_and_wait();
    f('d',2);
    f('d',6);
    f('e',4);
    f('f',2);
    f('f',6);
    workDone.arrive_and_wait();
    f('g',2);
    f('g',6);
    f('h',4);
    f('i',2);
    workDone.arrive_and_wait();
    f('j',2);
    f('j',6);
    f('k',1);
}
void worker3(){
    f('a',3);
    f('b',3);
    f('b',7);
    f('c',3);
    workDone.arrive_and_wait();
    f('d',3);
    f('e',1);
    f('e',5);
    f('f',3);
    f('f',7);
    workDone.arrive_and_wait();
    f('g',3);
    f('h',1);
    f('h',5);
    f('i',3);
    workDone.arrive_and_wait();
    f('j',3);
    f('j',7);
    f('k',2);
}
void worker4(){
    f('a',4);
    f('b',4);
    f('b',8);
    f('c',4);
    workDone.arrive_and_wait();
    f('d',4);
    f('e',2);
    f('e',6);
    f('f',4);
    workDone.arrive_and_wait();
    f('g',4);
    f('h',2);
    f('h',6);
    f('i',4);
    workDone.arrive_and_wait();
    f('j',4);
    f('j',8);
    f('k',3);
}

int main() {
    synchronizedOut("Обчислення розпочато.\n");

    thread t1(worker1);
    thread t2(worker2);
    thread t3(worker3);
    thread t4(worker4);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    synchronizedOut("Обчислення завершено.");
    return 0;
}
