#include <iostream>
#include "utils.cpp"
#include "BTree.cpp"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;
    char* f ="Test.txt";
    CreateIndexFile(f,10,5);
    InsertNewRecordAtIndex(f,3,12);
    InsertNewRecordAtIndex(f,7,24);
    InsertNewRecordAtIndex(f,10,48);
    InsertNewRecordAtIndex(f,14,72);
    InsertNewRecordAtIndex(f,24,60);
    InsertNewRecordAtIndex(f,19,84);
    InsertNewRecordAtIndex(f,30,96);
    InsertNewRecordAtIndex(f,15,108);
    InsertNewRecordAtIndex(f,1,120);
    InsertNewRecordAtIndex(f,5,132);
    InsertNewRecordAtIndex(f,2,144);
    InsertNewRecordAtIndex(f,8,156);
    InsertNewRecordAtIndex(f,9,168);
    InsertNewRecordAtIndex(f,6,180);
    InsertNewRecordAtIndex(f,11,192);
    InsertNewRecordAtIndex(f,12,204);
    InsertNewRecordAtIndex(f,17,216);
    InsertNewRecordAtIndex(f,18,228);
    InsertNewRecordAtIndex(f,32,240);
    DeleteRecordFromIndex(f,10);
    DeleteRecordFromIndex(f,9);
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
