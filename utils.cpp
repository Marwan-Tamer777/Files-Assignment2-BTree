#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

struct BTreeNodeUnit{
    int value = -1;
    int reference = -1;
};

struct BTreeNode{
    int stateFlag;
    int parentOrNextDel;
    vector<BTreeNodeUnit> nodes;
};

const int DEL_FLAG = -1;
const int NODE_FLAG = 0;
const int LEAF_FLAG = 1;
const int FIELD_SIZE = 4;
const int M_SIZE = 8;
const int NODE_SIZE = FIELD_SIZE*(2+M_SIZE/2);
fstream fBTree("btree.txt", ios::in | ios::out);
//Record example:
//   1   2   A   3   B   7   C   3   Y   9

//Deleted Record Example
//  -1   2  -1  -1  -1  -1  -1  -1  -1  -1

string readBytes(fstream &f,int byteCount){
    char temp = ' ';
    string value= "";
    for(int i=0;i<byteCount;i++){
        temp = f.get();
        value.push_back(temp);
    }
    return value;
};


void writeBytes(fstream& f,int fSize,string s){
    int actualSize = s.length();
    int diff = fSize-actualSize;
    int writtenChars = 0;

    for(int i=diff;i>0;i--){
        f.put(' ');
        writtenChars++;
    }
    for(int i=0;writtenChars!=fSize;i++,writtenChars++){
        f.put(s[i]);
    }


};

int getFileSize(fstream& f){
    int current,full;
    current = f.tellg();
    f.seekg(0,ios::end);
    full = f.tellg();
    f.seekg(current,ios::beg);
    return full;
};

BTreeNode readTreeNode(){
    BTreeNode btn;
    btn.stateFlag = stoi (readBytes(fBTree, 4));
    btn.parentOrNextDel = stoi (readBytes(fBTree, 4));
    for(int i=0;i<M_SIZE/2;i++){
        BTreeNodeUnit btnu;
        btnu.value = stoi (readBytes(fBTree, 4));
        btnu.reference = stoi (readBytes(fBTree, 4));
        btn.nodes.push_back(btnu);
    }
    return btn;
};
void writeTreeNode(BTreeNode btn){
    writeBytes(fBTree,FIELD_SIZE,btn.stateFlag);
    writeBytes(fBTree,FIELD_SIZE,btn.parentOrNextDel);
    for(int i=0;i<btn.nodes.size();i++){
        writeBytes(fBTree,FIELD_SIZE,btn.nodes[i].value);
        writeBytes(fBTree,FIELD_SIZE,btn.nodes[i].reference);
    }
};

int initlaise(){

    fBTree.seekg(0,ios::end);
    if(fBTree.tellg() == 0){
        BTreeNode btn;
        BTreeNodeUnit btnu;
        vector<BTreeNodeUnit> v;
        btnu.reference = -1;
        btnu.value = -1;

        for(int i=0;i<M_SIZE/2;i++){
            v.push_back(btnu);
        }
        btn.stateFlag = DEL_FLAG;
        btn.parentOrNextDel = 1;
        btn.nodes = v;

        writeTreeNode(btn);
        btn.parentOrNextDel = -1;
        writeTreeNode(btn);
    }
}
