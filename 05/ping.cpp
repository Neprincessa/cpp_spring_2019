#include <iostream>
#include <mutex>
#include <thread>

#define N 500000

std::mutex m1;
std::mutex m2;

void ping()
{
    for(int i = 0; i < N; i++)
    {
        m1.lock();
        std::cout<<"ping\n";
        m2.unlock();
    }
}

void pong()
{
    for(int i = 0; i < N; i++)
    {
        m2.lock();
        std::cout<<"pong\n";
        m1.unlock();
    }
}

int main()
{
    std::thread t1(ping);
    std::thread t2(pong);
    m2.lock();
    t1.join();
    t2.join();
    return 0;
}