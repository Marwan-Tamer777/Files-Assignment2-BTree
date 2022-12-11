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
int M_SIZE;// = 8;
int NODE_SIZE;// = FIELD_SIZE*(2+M_SIZE/2);
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


//Parameter function to sort the btree node units vector.
bool NodesSorterAscending(BTreeNodeUnit const& lbtnu, BTreeNodeUnit const& rbtnu) {
    if(rbtnu.value == DEL_FLAG){return true;}
    if(lbtnu.value == DEL_FLAG){return false;}
    return lbtnu.value < rbtnu.value;
};

void CreateIndexFile(char* fileName,int numberOfRecord, int m){
    fBTree.open(fileName, ios::in | ios::out | ios::trunc);
    M_SIZE = m;
    NODE_SIZE = FIELD_SIZE*(2+M_SIZE);

    fBTree.seekg(0,ios::beg);
    BTreeNode btn;
    BTreeNodeUnit btnu;
    vector<BTreeNodeUnit> v;
    btnu.reference = -1;
    btnu.value = -1;

    for(int i=0;i<M_SIZE/2;i++){
        v.push_back(btnu);
    }

    btn.stateFlag = DEL_FLAG;
    btn.nodes = v;

    for(int i=1;i<=numberOfRecord;i++){
        btn.parentOrNextDel = i;
        if(i==numberOfRecord){btn.parentOrNextDel = -1;}
        writeTreeNode(btn);
    }

};


int InsertNewRecordAtIndex(char* filename, int RecordID, int Reference) {
    //we start with the root node and check if it's empty or not.
    fBTree.seekg(NODE_SIZE,ios::beg);
    BTreeNode btn= readTreeNode();
    vector<BTreeNodeUnit> v;

    if(btn.stateFlag == -1){
        //Root is empty so we overwrite it into a leaf
        BTreeNodeUnit btnu;
        btnu.value = RecordID;
        btnu.reference = Reference;
        btn.stateFlag = LEAF_FLAG;
        btn.parentOrNextDel = DEL_FLAG;
        btn.nodes[0] = btnu;
        writeFirstDelTreeNode(btn);
        return 1;
    }

    //Root is not empty, we will traverse to the correct leaf node;

    while(btn.stateFlag != LEAF_FLAG){

        v = btn.nodes;
        int nextRRN;

        for(int i = 0;i<v.size();i++){
            //Takes the next Node reference until it either reaches
            // end of vector or finds a node it's value is less than it;
            if(v[i].value == DEL_FLAG){break;}
            nextRRN = v[i].reference;
            if(RecordID<= v[i].value){break;}
        }

        fBTree.seekg(nextRRN*NODE_SIZE,ios::beg);
        btn = readTreeNode();
    }

    v = btn.nodes;
    for(int i = 0;i<v.size();i++){
        //if there was empty slots in the leaf node we will add to the vector and sort
        //then write it in it's place and move to the parent
        if(v[i].value == DEL_FLAG && v[i].reference == DEL_FLAG){
            BTreeNodeUnit btnu;
            btnu.value = RecordID;
            btnu.reference = Reference;
            v[i] = btnu;
            sort(v.begin(),v.end(),&NodesSorterAscending);
            btn.nodes = v;

            fBTree.seekg(btn.byteOffset,ios::beg);
            writeTreeNode(btn);

            //then we seek the parents until we reach the root to check on the values if it needs updating
            int childNodeReference = fBTree.tellg()/NODE_SIZE -1;
            while(childNodeReference != 1/*btn.parentOrNextDel == DEL_FLAG*/){

                //we start at the end of the child node to get some values;
                int biggest = getBiggestNum(v);
                fBTree.seekg(btn.parentOrNextDel*FIELD_SIZE,ios::beg);

                //get parent node details.
                btn = readTreeNode();
                v = btn.nodes;
                //loop on parent references till we get the child's we came from reference
                for(int x = 0;x<v.size();x++){
                    if(v[x].reference = childNodeReference){
                        v[x].value = biggest;
                    }
                }

                //Overwrite parent node in case the values should be updates;
                fBTree.seekg(btn.byteOffset,ios::beg);
                writeTreeNode(btn);
                childNodeReference = fBTree.tellg()/NODE_SIZE -1;
            }
            return 1;
        } else {
            //we do a split here and check if you are splitting the root as it is a special case.
        }
    }


};

void DeleteRecordFromIndex(char* filename, int RecordID) {

};

void DisplayIndexFileContent(char* filename) {

};

int SearchARecord(char* filename, int RecordID) {

};
