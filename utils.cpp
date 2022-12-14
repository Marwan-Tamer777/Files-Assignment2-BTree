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
    int byteOffset;
    vector<BTreeNodeUnit> nodes;
};

//Basic values for testing gets overridden in run time.
const int DEL_FLAG = -1;
const int NODE_FLAG = 1;
const int LEAF_FLAG = 0;
const int FIELD_SIZE = 4;
const string NODE_DELIMITER = "|-|";
//User Inputted values;
int NUM_OF_RECORDS;
int M_SIZE;// = 8;
int NODE_SIZE;// = FIELD_SIZE*(2+M_SIZE) + NODE_DELIMITER.length());
fstream fBTree;

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

BTreeNode getEmptyNode(){
    BTreeNode btn;
    BTreeNodeUnit btnu;
    vector<BTreeNodeUnit> v;

    btn.stateFlag = DEL_FLAG;
    btn.parentOrNextDel = -1;
    btnu.reference = -1;
    btnu.value = -1;

    for(int i=0;i<M_SIZE/2;i++){
        v.push_back(btnu);
    }
    btn.nodes = v;

    return btn;
};

BTreeNodeUnit getEmptyNodeUnit(){
    BTreeNodeUnit btnu;
    btnu.reference = -1;
    btnu.value = -1;
    return btnu;
};

BTreeNode readTreeNode(){

    BTreeNode btn;
    btn.byteOffset = fBTree.tellg();
    btn.stateFlag = stoi(readBytes(fBTree, 4));
    btn.parentOrNextDel = stoi(readBytes(fBTree, 4));
    for(int i=0;i<M_SIZE/2;i++){
        BTreeNodeUnit btnu;
        btnu.value = stoi(readBytes(fBTree, 4));
        btnu.reference = stoi(readBytes(fBTree, 4));
        btn.nodes.push_back(btnu);
    }
    readBytes(fBTree, NODE_DELIMITER.length());
    /*
    string s;
    readline(fBTree,s);
    BTreeNode btn;
    int pos = 0-FIELD_SIZE;
    btn.byteOffset = fBTree.tellg();
    btn.stateFlag = stoi(s.substr(pos+=FIELD_SIZE,FIELD_SIZE));
    btn.parentOrNextDel = stoi(s.substr(pos+=FIELD_SIZE,FIELD_SIZE));
    for(int i=0;i<M_SIZE/2;i++){
        BTreeNodeUnit btnu;
        btnu.value = stoi(s.substr(pos+=FIELD_SIZE,FIELD_SIZE));
        btnu.reference = stoi(s.substr(pos+=FIELD_SIZE,FIELD_SIZE));
        btn.nodes.push_back(btnu);

    }*/
    return btn;
};

BTreeNode readFirstDelTreeNode(){
    int currentPos = fBTree.tellg();
    int nextDeletedIndex;

    fBTree.seekg(0,ios::beg);
    BTreeNode btn;
    btn = readTreeNode();
    nextDeletedIndex = btn.parentOrNextDel;

    if(nextDeletedIndex == -1){cout<<"No Available Empty Nodes"<<endl;return getEmptyNode();}
    fBTree.seekg(nextDeletedIndex*NODE_SIZE,ios::beg);
    btn = readTreeNode();

    fBTree.seekg(currentPos,ios::beg);
    return btn;
};

void writeTreeNode(BTreeNode btn){
    writeBytes(fBTree,FIELD_SIZE,to_string(btn.stateFlag));
    writeBytes(fBTree,FIELD_SIZE,to_string(btn.parentOrNextDel));
    for(int i=0;i<btn.nodes.size();i++){
        writeBytes(fBTree,FIELD_SIZE,to_string(btn.nodes[i].value));
        writeBytes(fBTree,FIELD_SIZE,to_string(btn.nodes[i].reference));
    }
    fBTree<<NODE_DELIMITER;
    //fBTree<<endl;
};

void writeFirstDelTreeNode(BTreeNode btn){
    BTreeNode delBtn = readFirstDelTreeNode();
    int nextDeletedNode = delBtn.parentOrNextDel;
    fBTree.seekg(delBtn.byteOffset,ios::beg);

    writeTreeNode(btn);

    fBTree.seekg(FIELD_SIZE,ios::beg);
    writeBytes(fBTree,FIELD_SIZE,to_string(nextDeletedNode));
};


int getBiggestNum(vector<BTreeNodeUnit> v){
    int biggest = -1;

    for(int i=0;i<v.size();i++){
        if(v[i].value>biggest){
            biggest = v[i].value;
        }
    }

    return biggest;
};

int getNodeRRN(BTreeNode btn){
    return btn.byteOffset/NODE_SIZE;
};
//Parameter function to sort the btree node units vector.
bool NodesSorterAscending(BTreeNodeUnit const& lbtnu, BTreeNodeUnit const& rbtnu) {
    if(rbtnu.value == DEL_FLAG){return true;}
    if(lbtnu.value == DEL_FLAG){return false;}
    return lbtnu.value < rbtnu.value;
};

void printFixedLengthField(int length,int value){
    int valueLength = to_string(value).length();
    while(length>valueLength){
        cout<<" ";
        valueLength++;
    }
    cout<<value;
}

void printBTreeNode(BTreeNode btn){
    printFixedLengthField(FIELD_SIZE,btn.stateFlag);
    printFixedLengthField(FIELD_SIZE,btn.parentOrNextDel);
    for(int i=0;i<btn.nodes.size();i++){
        printFixedLengthField(FIELD_SIZE,btn.nodes[i].value);
    printFixedLengthField(FIELD_SIZE,btn.nodes[i].reference);
    }
    cout<<endl;
}
