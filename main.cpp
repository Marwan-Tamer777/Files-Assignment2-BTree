#include <iostream>
#include "utils.cpp"


using namespace std;

int main()
{
    cout << "Hello world!" << endl;
    char* f ="Test.txt";
    CreateIndexFile(f,10,10);
    InsertNewRecordAtIndex(f,2,987);
    InsertNewRecordAtIndex(f,5,127);
    InsertNewRecordAtIndex(f,1,47);
    InsertNewRecordAtIndex(f,7,3457);
    InsertNewRecordAtIndex(f,19,454);
    return 0;
}
