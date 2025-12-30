#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "linxer/linxer.h"

using namespace std;

void GetLines(linxer::IAccessor* accessor, int from, int to, bool update=false);
void GetLines(linxer::IAccessor* accessor, int from, int to, bool update)
{
    cout << "From " << from << " to " << to << (update?" (update)":"") << " got line:" << endl;
    auto lines = accessor->get_lines(from, to, update);
    for (auto& line : lines)
    {
        cout << "  --> " << line << endl;
    }
}

void Test_1()
{
    cout << __func__ << " =====================" << endl;

    string file_path = "./test_input.txt";

    ofstream stream{file_path};
    auto accessor = linxer::new_accessor(file_path, 0, 2);
    accessor->update_index();
    
    stream << "one";
    stream.flush();
    accessor->update_index();
    
    stream << "ONE";
    stream.flush();
    accessor->update_index();

    stream << endl;
    accessor->update_index();

    stream << "two";
    stream.flush();
    accessor->update_index();
    accessor->update_index();

    stream << endl;
    accessor->update_index();
    accessor->update_index();



    stream << "three" << endl;
    stream << "four" << endl;

    GetLines(accessor.get(), 0, -1);
    GetLines(accessor.get(), 0, -1, true);
    GetLines(accessor.get(), 0, 1);
    GetLines(accessor.get(), 0, 4);
    GetLines(accessor.get(), 2, 2);
    GetLines(accessor.get(), -1, 0);
}

void Test_2()
{
    cout << __func__ << " =====================" << endl;

    string file_path = "./test_input.txt";

    ofstream stream{file_path};
    
    auto accessor = linxer::new_accessor(file_path, 1000, 2);
    this_thread::sleep_for(chrono::milliseconds{200});

    stream << "one" << endl;
    stream << "two" << endl;
    stream << "three" << endl;
    stream << "four" << endl;

    GetLines(accessor.get(), 0, -1);

    this_thread::sleep_for(chrono::milliseconds{600});

    GetLines(accessor.get(), 0, -1);

    this_thread::sleep_for(chrono::milliseconds{400});

    GetLines(accessor.get(), 0, -1);
}

void Test_3()
{
    cout << __func__ << " =====================" << endl;

    string file_path = "./test_input.txt";

    ofstream stream{file_path};
    
    for (auto i=0 ; i<9 ; ++i)
    {
        stream << "L0" << i+1 << endl;
    }
    stream << "L10" << endl;
    
    auto accessor = linxer::new_accessor(file_path);

    GetLines(accessor.get(), 0, 3);
    GetLines(accessor.get(), -4, -1);
    GetLines(accessor.get(), 6, -1);
    GetLines(accessor.get(), 3, 0);
    GetLines(accessor.get(), -1, -4);
    GetLines(accessor.get(), -1, 6);
    GetLines(accessor.get(), 5, 5);

    try
    {
        (void)accessor->get_line(11);
    }
    catch (exception& e)
    {
        cout << "Caught: " << e.what() << endl;
    }

    try
    {
        (void)accessor->get_line(-12);
    }
    catch (exception& e)
    {
        cout << "Caught: " << e.what() << endl;
    }

    try
    {
        (void)GetLines(accessor.get(), 0, 11);
    }
    catch (exception& e)
    {
        cout << "Caught: " << e.what() << endl;
    }

    try
    {
        (void)GetLines(accessor.get(), 0, -12);
    }
    catch (exception& e)
    {
        cout << "Caught: " << e.what() << endl;
    }

    try
    {
        (void)GetLines(accessor.get(), 11, 0);
    }
    catch (exception& e)
    {
        cout << "Caught: " << e.what() << endl;
    }

    try
    {
        (void)GetLines(accessor.get(), -12, 0);
    }
    catch (exception& e)
    {
        cout << "Caught: " << e.what() << endl;
    }
}

int main()
{
    Test_1();
    Test_2();
    Test_3();
    return 0;
}
