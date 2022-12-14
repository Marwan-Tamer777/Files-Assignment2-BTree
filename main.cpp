#include <iostream>
#include "utils.cpp"
#include "BTree.cpp"

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
    DisplayIndexFileContent(f);

    int test;
    cout<<endl;
    test = SearchARecord(f,2);
    cout<<test<< " : ";
    test = SearchARecord(f,1);
    cout<<test<< " : ";
    test = SearchARecord(f,7);
    cout<<test<< " : ";
    test = SearchARecord(f,19);
    cout<<test<< " : ";
    test = SearchARecord(f,95);
    cout<<test<< " : ";
    test = SearchARecord(f,5);
    cout<<test<< " : ";
    return 0;
}
