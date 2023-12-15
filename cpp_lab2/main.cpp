#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <format>
#include <cmath>
#include <execution>
#include <thread>
#include <random>

using namespace std;


template <class F>
void timeit(F&& f)
{
    auto time1 = chrono::system_clock::now();
    f();
    auto time2 = chrono::system_clock::now();
    auto tdiff = time2 - time1;
    if (tdiff > 100000ns)
        cout <<"        " << format("{:%S}", tdiff) << endl;
    else
        cout << tdiff << endl;
}


template <class F>
void timeit(F&& f, int repeat)
{
    auto time1 = chrono::system_clock::now();
    while (repeat--)
        f();
    auto time2 = chrono::system_clock::now();
    auto tdiff = time2 - time1;
    if (tdiff > 100000ns)
        cout << format("{:%S}", tdiff) << endl;
    else
        cout << tdiff << endl;
}

vector<int> getNumbers(int begin, int end, int inc = 1)
{
    vector<int> numbers;
    if (inc > 0)
        for (int i = begin; i < end; i += inc)
        {
            numbers.push_back(i);
        }
    else
        for (int i = begin; i > end; i += inc)
        {
            numbers.push_back(i);
        }
    return numbers;
}

template <typename Iterator, typename T>
struct all_of_block
{
    void operator()(Iterator first, Iterator last, T& result)
    {
        result = std::all_of(execution::seq,first, last, [](int m) { return m % 2 == 0; });
    }
};

template <typename Iterator, typename T>
T parallel_all_of( Iterator first, Iterator last, T num_threads )
{
    unsigned long const length = std::distance(first, last);
    if (!length)
        return 0;
    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    
    
    unsigned long const block_size = length / num_threads;
    vector<T> results(num_threads, 0);
    vector<thread> threads(num_threads - 1);
    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i)
    {
        Iterator block_end = block_start;
        advance(block_end, block_size);
        threads[i] = thread(all_of_block<Iterator, T>(), block_start,
            block_end, std::ref(results[i]));
        block_start = block_end;
    }
    all_of_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
    for (auto& entry : threads)
        entry.join();
    
    return std::all_of(execution::seq, results.begin(), results.end(), [](int m) { return m % 2 == 0; });;
}

template <class F>
void foo(F&& fo, int from, int to, int inc, int cnt)
{

    auto numbers = getNumbers(from, to, inc);
    cout << "Vector from " << from << " to " << to << " with step " << inc << " executed " << cnt << " times per policy" << endl;
    cout << "Vector traversal using different policies" << endl;


    cout << "no policy ";
    timeit([fo, &numbers]() { std::all_of(numbers.begin(), numbers.end(), fo); }, cnt);


    cout << "sequental ";
    timeit([fo, &numbers]() { std::all_of(execution::seq, numbers.begin(), numbers.end(), fo); }, cnt);


    cout << "parallel  ";
    timeit([fo, &numbers]() { std::all_of(execution::par, numbers.begin(), numbers.end(), fo); }, cnt);


    cout << "unseq     ";
    timeit([fo, &numbers]() { std::all_of(execution::unseq, numbers.begin(), numbers.end(), fo); }, cnt);


    cout << "parallel unseq ";
    timeit([fo, &numbers]() { std::all_of(execution::par_unseq, numbers.begin(), numbers.end(), fo); }, cnt);
    cout << endl;
}

int main()
{
    cout << "Library algorithm al_off" << endl;
    cout << "With function '[](int i) { return i % 2 == 0; }' :" << endl;

    foo([](int i) { return i % 2 == 0; }, 1000, 0, -1, 3);

    foo([](int i) { return i % 2 == 0; }, 1000, 0, -1, 1);

    foo([](int i) { return i % 2 == 0; }, 0, 1000, 1, 3);

    foo([](int i) { return i % 2 == 0; }, 0, 1000, 1, 1);
    cout << endl;

    cout << "With function '[](int i) { return i > 0; }' :" << endl;

    foo([](int i) { return i > 0; }, 1000, 0, -1, 3);

    foo([](int i) { return i > 0; }, 1000, 0, -1, 1);

    foo([](int i) { return i > 0; }, 0, 1000, 1, 3);

    foo([](int i) { return i > 0; }, 0, 1000, 1, 1);

    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    vector<double> container(100000000LL, 0);
    random_device r;
    seed_seq seeds{ r(), r(), r(), r(), r(), r() };
    mt19937 e(seeds);
    uniform_real_distribution<double> d(-10, 10);
    for (auto& el : container)
        el = d(e);
    cout<<"Research of own parallel algorithm"<<endl;
    cout << "Hardware threads" << "        " << " K " << "        " << " time" << endl;
    cout << hardware_threads<<"                       "<< 2 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 2); });
    cout << hardware_threads<<"                       "<< 3 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 3); });
    cout << hardware_threads << "                       " << 4 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 4); });
    cout << hardware_threads << "                       " << 5 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 5); });
    cout << hardware_threads << "                       " << 6 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 6); });
    cout << hardware_threads << "                       " << 8 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 8); });
    cout << hardware_threads << "                      " << 12 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 12); });
    cout << hardware_threads << "                      " << 24 ;
    timeit([&container]()
        { parallel_all_of(container.cbegin(), container.cend(), 24); });
    
    

    return 0;
}