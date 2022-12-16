#include <iostream>
#include "utils.cpp"
#include "BTree.cpp"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;
    char* f ="Test.txt";
    CreateIndexFile(f,10,10);
    InsertNewRecordAtIndex(f,3,12);
    InsertNewRecordAtIndex(f,7,24);
    InsertNewRecordAtIndex(f,10,48);
    InsertNewRecordAtIndex(f,14,72);
    InsertNewRecordAtIndex(f,24,60);
    DisplayIndexFileContent(f);

    int test;
    cout<<endl;
    test = SearchARecord(f,3);
    cout<<test<< " : ";
    test = SearchARecord(f,14);
    cout<<test<< " : ";
    test = SearchARecord(f,24);
    cout<<test<< " : ";
    test = SearchARecord(f,10);
    cout<<test<< " : ";
    test = SearchARecord(f,95);
    cout<<test<< " : ";
    test = SearchARecord(f,7);
    cout<<test<< " : ";
    return 0;
}
