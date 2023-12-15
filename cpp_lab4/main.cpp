#include <iostream>
#include <string>
#include <format>
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <thread>
#include <random>
#include <set>
#include <fstream>
#include <functional>
#include <chrono>
#include <syncstream>

using namespace std;


void synchronizedOut(const string &s) {
    osyncstream(cout) << s;
}


class DS {
private:
    int a;
    char b;
    shared_mutex mu_a;
    shared_mutex mu_b;
public:
    DS() : a(1), b('a') {};

    ~DS() = default;

    int getInt() {
        shared_lock<shared_mutex> guard(mu_a);
        return this->a;
    }

    char getChar() {
        shared_lock<shared_mutex> guard(mu_b);
        return this->b;
    }

    bool setInt(int value = 1) {
        lock_guard<shared_mutex> guard(mu_a);
        this->a = value;
        return true;
    }

    bool setChar(char value = 'a') {
        lock_guard<shared_mutex> guard(mu_b);
        this->b = value;
        return true;
    }

    operator string() {
        shared_lock<shared_mutex> guard_a(mu_a);
        shared_lock<shared_mutex> guard_b(mu_b);
        return format("{} {}", this->a, this->b);
    }
};


bool generateFiles() {
    vector<vector<int>> percentages = {{10, 5,  50, 10},
                                       {20, 20, 20, 20},
                                       {5,  25, 5,  20}};
    int multiplyer = 1000;
    random_device rd;
    mt19937 gen(rd());
    for (int i = 0; i < 3; ++i) {
        set<int> generatedNumbers;
        ofstream file(format("file{}.txt", i));
        if (file.is_open()) {
            for (int j = 0; j < 100 * multiplyer; ++j) {
                uniform_int_distribution<> distribution(1, 100 * multiplyer);
                int operationType;
                do {
                    operationType = distribution(gen);
                } while (generatedNumbers.count(operationType) > 0);

                generatedNumbers.insert(operationType);

                if (operationType <= percentages[i][0] * multiplyer) {
                    file << "read 0" << endl;
                } else if (operationType <=
                           percentages[i][0] * multiplyer + percentages[i][1] * multiplyer) {
                    file << "write 0 1" << endl;
                } else if (operationType <=
                           percentages[i][0] * multiplyer + percentages[i][1] * multiplyer +
                           percentages[i][2] * multiplyer) {
                    file << "read 1" << endl;
                } else if (operationType <=
                           percentages[i][0] * multiplyer + percentages[i][1] * multiplyer +
                           percentages[i][2] * multiplyer + percentages[i][3] * multiplyer) {
                    file << "write 1 a" << endl;
                } else {
                    file << "string" << endl;
                }
            }
            file.close();
        } else {
            cerr << "Unable to open " << format("file{}.txt", i) << endl;
            return false;
        }
    }
    return true;
}


int cutFiles(int numOfThreads, string fileName) {
    ifstream file_input(fileName);
    if (!file_input.is_open()) {
        cerr << "Unable to open file\n";
        return 1;
    }
    vector<string> instructions;
    vector<string> files_instructions;
    string line;
    while (getline(file_input, line)) {
        instructions.push_back(line);
    }
    for (int i = 0; i < numOfThreads; ++i) {
        ofstream file_output(format("stream{}file.txt", i));
        int start = i * (instructions.size() / numOfThreads);
        int end = start + (instructions.size() / numOfThreads);
        for (int j = start; j < end; ++j) {
            file_output << instructions[j] << endl;
        }
        file_output.close();
    }
    if (instructions.size() % numOfThreads != 0) {
        int left = instructions.size() - (numOfThreads) * (instructions.size() / numOfThreads);
        ofstream file_output(format("stream{}file.txt", numOfThreads - 1), ios::app);
        for (int i = 0; i < left; i++) {
            file_output << instructions[instructions.size() - 1 - i] << endl;
        }
    }
    file_input.close();
}

vector<function<void()>> parse(DS &ds, string fileName) {
    vector<function<void()>> calls;
    ifstream file(fileName);
    if (file.is_open()) {
        string line;
        vector<string> lines;
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
        for (const auto &storedLine: lines) {
            if (storedLine == "read 0") {
                calls.emplace_back([&ds]() { ds.getInt(); });
            } else if (storedLine == "write 0 1") {
                calls.emplace_back([&ds]() { ds.setInt(); });
            } else if (storedLine == "read 1") {
                calls.emplace_back([&ds]() { ds.getChar(); });
            } else if (storedLine == "write 1 a") {
                calls.emplace_back([&ds]() { ds.setChar(); });
            } else if (storedLine == "string") {
                calls.emplace_back([&ds]() { static_cast<string>(ds); });
            }
        }
    } else {
        cerr << "Unable to open file" << endl;
    }
    return calls;
}

template<class F>
void timeit(F &&f) {
    auto time1 = chrono::system_clock::now();
    f();
    auto time2 = chrono::system_clock::now();
    auto tdiff = time2 - time1;
    if (tdiff > 100000ns)
        cout << "Time: " << format("{:%S}", tdiff) << endl;
    else
        cout << tdiff << endl;
}

void f1(vector<function<void()>> &calls1) {

    thread t1([&calls1] {
        for (const auto &call: calls1) {
            call();
        }
    });
    t1.join();
}

void f2(vector<function<void()>> &calls1, vector<function<void()>> &calls2) {

    thread t1([&calls1] {
        for (const auto &call: calls1) {
            call();
        }
    });
    thread t2([&calls2] {
        for (const auto &call: calls2) {
            call();
        }
    });
    t1.join();
    t2.join();
}

void f3(vector<function<void()>> &calls1, vector<function<void()>> &calls2, vector<function<void()>> &calls3) {

    thread t1([&calls1] {
        for (const auto &call: calls1) {
            call();
        }
    });
    thread t2([&calls2] {
        for (const auto &call: calls2) {
            call();
        }
    });
    thread t3([&calls3] {
        for (const auto &call: calls3) {
            call();
        }
    });
    t1.join();
    t2.join();
    t3.join();
}

int main() {
    DS ds;
    vector<function<void()>> calls1;
    vector<function<void()>> calls2;
    vector<function<void()>> calls3;

    generateFiles();


    synchronizedOut("1 thread, normal percentage");
    cutFiles(1, "file0.txt");
    calls1 = parse(ds, "stream0file.txt");
    timeit([&calls1]() { f1(calls1); });

    synchronizedOut("1 thread, equal percentage");
    cutFiles(1, "file1.txt");
    calls1 = parse(ds, "stream0file.txt");
    timeit([&calls1]() { f1(calls1); });

    synchronizedOut("1 thread, weird percentage");
    cutFiles(1, "file2.txt");
    calls1 = parse(ds, "stream0file.txt");
    timeit([&calls1]() { f1(calls1); });


    synchronizedOut("2 threads, normal percentage");
    cutFiles(2, "file0.txt");
    calls1 = parse(ds, "stream0file.txt");
    calls2 = parse(ds, "stream1file.txt");
    timeit([&calls1, &calls2]() { f2(calls1, calls2); });

    synchronizedOut("2 threads, equal percentage");
    cutFiles(2, "file1.txt");
    calls1 = parse(ds, "stream0file.txt");
    calls2 = parse(ds, "stream1file.txt");
    timeit([&calls1, &calls2]() { f2(calls1, calls2); });

    synchronizedOut("2 threads, weird percentage");
    cutFiles(2, "file2.txt");
    calls1 = parse(ds, "stream0file.txt");
    calls2 = parse(ds, "stream1file.txt");
    timeit([&calls1, &calls2]() { f2(calls1, calls2); });


    synchronizedOut("3 threads, normal percentage");
    cutFiles(3, "file0.txt");
    calls1 = parse(ds, "stream0file.txt");
    calls2 = parse(ds, "stream1file.txt");
    calls3 = parse(ds, "stream2file.txt");
    timeit([&calls1, &calls2, &calls3]() { f3(calls1, calls2, calls3); });

    synchronizedOut("3 threads, equal percentage");
    cutFiles(3, "file1.txt");
    calls1 = parse(ds, "stream0file.txt");
    calls2 = parse(ds, "stream1file.txt");
    calls3 = parse(ds, "stream2file.txt");
    timeit([&calls1, &calls2, &calls3]() { f3(calls1, calls2, calls3); });

    synchronizedOut("3 threads, weird percentage");
    cutFiles(3, "file2.txt");
    calls1 = parse(ds, "stream0file.txt");
    calls2 = parse(ds, "stream1file.txt");
    calls3 = parse(ds, "stream2file.txt");
    timeit([&calls1, &calls2, &calls3]() { f3(calls1, calls2, calls3); });

    return 0;
}
