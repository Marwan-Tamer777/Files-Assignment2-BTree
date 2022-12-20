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

//Functions Definitions
void updateParents(BTreeNode);
int splitNode(BTreeNode,BTreeNodeUnit);

//Basic values for testing gets overridden in run time.
const int DEL_FLAG = -1;
const int PARENT_FLAG = 1;
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

//Reads a specific number of bytes. used in more complex read functions.
string readBytes(fstream &f,int byteCount){
    char temp = ' ';
    string value= "";
    for(int i=0;i<byteCount;i++){
        temp = f.get();
        value.push_back(temp);
    }
    return value;
};

//Writes a specific number of bytes. used in more complex write functions.
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
    btn.byteOffset = -1;
    btnu.reference = -1;
    btnu.value = -1;

    for(int i=0;i<M_SIZE;i++){
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

//Reads a while Node from the data file (size is Dependant on the M size values the users inputs.
//And returns  a BTreeNode object.
//Navigate first to the Node you want to read in the file.
BTreeNode readTreeNode(){

    BTreeNode btn;
    btn.byteOffset = fBTree.tellg();
    btn.stateFlag = stoi(readBytes(fBTree, 4));
    btn.parentOrNextDel = stoi(readBytes(fBTree, 4));
    for(int i=0;i<M_SIZE;i++){
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
    for(int i=0;i<M_SIZE;i++){
        BTreeNodeUnit btnu;
        btnu.value = stoi(s.substr(pos+=FIELD_SIZE,FIELD_SIZE));
        btnu.reference = stoi(s.substr(pos+=FIELD_SIZE,FIELD_SIZE));
        btn.nodes.push_back(btnu);

    }*/
    return btn;
};

//Returns the first deleted node from the AvailList, returns an empty node if none are available.
//You can Error check the Returned node by seeing if it's ByteOffset is -1 (An empty not real node)
//Or an actual value (A node that exists in the Index file.
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

//Writes a BTreeNode object into the Index file.
//Please navigate to the Node you want to write in the file before using this.
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

//Writes the first Deleted Node in the Index Files and updates the Avail List Header.
//Needs both the new node to be written and it's parent index.
//Does not require you to navigate first before using this function.
void writeFirstDelTreeNode(BTreeNode btn,int newParentIndex){
    BTreeNode delBtn = readFirstDelTreeNode();
    int nextDeletedNode = delBtn.parentOrNextDel;
    delBtn.parentOrNextDel = newParentIndex;
    fBTree.seekg(delBtn.byteOffset,ios::beg);

    writeTreeNode(btn);

    //Overwrites the next item on the avail list in the header node.
    fBTree.seekg(FIELD_SIZE,ios::beg);
    writeBytes(fBTree,FIELD_SIZE,to_string(nextDeletedNode));
    fBTree.seekg(0,ios::beg);
};

//Gets the biggest value from the vector of any node to use in the Update Parent function.
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

//Inserts a specific record in a specific node.
//Used in other insertions Functions to allow Recursion as when splitting you will
//Need to insert new values in the parent and maybe split again.
int insertRecordInNode(BTreeNode btn, BTreeNodeUnit btnu){
    vector<BTreeNodeUnit> v = btn.nodes;
    for(int i = 0;i<v.size();i++){
        //if there was empty slots in the leaf node we will add to the vector and sort
        //then write it in it's place and move to the parent
        if(v[i].value == DEL_FLAG && v[i].reference == DEL_FLAG){
            v[i] = btnu;
            sort(v.begin(),v.end(),&NodesSorterAscending);
            btn.nodes = v;

            fBTree.seekg(btn.byteOffset,ios::beg);
            writeTreeNode(btn);
            //then we seek the parents until we reach the root to check on the values if it needs updating
            updateParents(btn);
            //The new record been inserted successfully in a node with empty data and the node index is returned.
            return getNodeRRN(btn);
        }
    }
    return splitNode(btn,btnu);
};


//Navigates the Tree upwards till the leaf and updates each parent with it's children Biggest value.
void updateParents(BTreeNode btn){
    vector<BTreeNodeUnit> v = btn.nodes;
    int childNodeReference = getNodeRRN(btn);

    while(childNodeReference != 1/*btn.parentOrNextDel == DEL_FLAG*/){

        //we start at the end of the child node to get some values;
        int biggest = getBiggestNum(v);
        fBTree.seekg(btn.parentOrNextDel*NODE_SIZE,ios::beg);

        //get parent node details.
        btn = readTreeNode();
        v = btn.nodes;
        //loop on parent references till we get the child's reference we just came from.
        for(int x = 0;x<v.size();x++){
            if(v[x].reference == childNodeReference){
                v[x].value = biggest;
                break;
            }
        }

        btn.nodes = v;
        //Overwrite parent node in case the values should be updated;
        fBTree.seekg(btn.byteOffset,ios::beg);
        writeTreeNode(btn);
        childNodeReference = getNodeRRN(btn);
    }
};

//Navigates down from the node to all of it's children updating their's parent's node value
//only used when splitting root(AKA Adding a new level to the tree.
void updateChildren(BTreeNode btn){

    vector<BTreeNodeUnit> v = btn.nodes;
    for(int i=0;i<v.size();i++){
        if(v[i].value!= -1 && btn.stateFlag == PARENT_FLAG){
            fBTree.seekg(v[i].reference*NODE_SIZE,ios::beg);
            BTreeNode childBtn =readTreeNode();
            childBtn.parentOrNextDel = getNodeRRN(btn);
            fBTree.seekg(-NODE_SIZE,ios::cur);
            writeTreeNode(childBtn);
            updateChildren(childBtn);
        }
    }

};

//Takes the node you want to split and the new Record that is to be added into it.
//It creates 2 nodes, splits the vector of the original node.
//Write the 2 nodes again then updates parents and adds new value to the immediate of the new nodes.
//Has a special case if the node to split is root as then it requires 3 nodes (The root and 2 empty nodes)
int splitNode(BTreeNode btn,BTreeNodeUnit btnu){
    //we do a split here and check if you are splitting the root as it is a special case.
    if(btn.byteOffset/NODE_SIZE == 1 ){
        BTreeNode btn2 = readFirstDelTreeNode();
        fBTree.seekg(btn2.parentOrNextDel*NODE_SIZE,ios::beg);
        BTreeNode btn3 = readTreeNode();
        vector<BTreeNodeUnit> v;
        vector<BTreeNodeUnit> newV1;
        vector<BTreeNodeUnit> newV2;
        vector<BTreeNodeUnit> newV3;

        //Gets the Records in the Full leaf nodes and adds the new record then sorts them.
        v = btn.nodes;
        v.push_back(btnu);
        sort(v.begin(),v.end(),&NodesSorterAscending);

        //Splits the Records vector into 2 new vectors and reassigns them into the 2 nodes
        for(int i=0;i<v.size();i++){
            if(i<v.size()/2){
                newV2.push_back(v[i]);
            }else{
                newV3.push_back(v[i]);
            }
        }

        btnu.value = getBiggestNum(newV2);
        btnu.reference = getNodeRRN(btn2);
        newV1.push_back(btnu);
        btnu.value = getBiggestNum(newV3);
        btnu.reference = getNodeRRN(btn3);
        newV1.push_back(btnu);

        while(newV1.size()<M_SIZE){
            newV1.push_back(getEmptyNodeUnit());
        }
        while(newV2.size()<M_SIZE){
            newV2.push_back(getEmptyNodeUnit());
        }
        while(newV3.size()<M_SIZE){
            newV3.push_back(getEmptyNodeUnit());
        }
        //Add the 2 new split node then rewrite the root node.

        btn.nodes = newV1;
        btn2.nodes = newV2;
        btn3.nodes = newV3;
        btn2.stateFlag = btn.stateFlag == LEAF_FLAG ? LEAF_FLAG : PARENT_FLAG;
        btn3.stateFlag = btn.stateFlag == LEAF_FLAG ? LEAF_FLAG : PARENT_FLAG;
        btn.stateFlag = PARENT_FLAG;
        btn2.parentOrNextDel = 1;
        btn3.parentOrNextDel = 1;
        fBTree.seekg(NODE_SIZE,ios::beg);
        writeTreeNode(btn);
        writeFirstDelTreeNode(btn2,1);
        writeFirstDelTreeNode(btn3,1);
        updateChildren(btn);
        return getNodeRRN(btn3);
    } else {
        BTreeNode btn2 = readFirstDelTreeNode();
        vector<BTreeNodeUnit> v;
        vector<BTreeNodeUnit> newV1;
        vector<BTreeNodeUnit> newV2;
        if(btn2.byteOffset == -1){cout<<"No Available Empty Nodes for Splitting."<<endl;return -1;}

        //Gets the Records in the Full leaf nodes and adds the new record then sorts them.
        v = btn.nodes;
        v.push_back(btnu);
        sort(v.begin(),v.end(),&NodesSorterAscending);

        //Splits the Records vector into 2 new vectors and reassigns them into the 2 nodes
        for(int i=0;i<v.size();i++){
            if(i<v.size()/2){
                newV1.push_back(v[i]);
            }else{
                newV2.push_back(v[i]);
            }
        }
        while(newV1.size()<M_SIZE){
            newV1.push_back(getEmptyNodeUnit());
        }
        while(newV2.size()<M_SIZE){
            newV2.push_back(getEmptyNodeUnit());
        }
        btn.nodes = newV1;
        btn2.nodes = newV2;

        //Writes the 2 split nodes in the files
        fBTree.seekg(btn.byteOffset,ios::beg);
        writeTreeNode(btn);
        btn2.stateFlag = btn.stateFlag;
        btn2.parentOrNextDel = btn.parentOrNextDel;
        writeFirstDelTreeNode(btn2,btn.parentOrNextDel);

        //Update parent value in respect to the old node then add the new split node reference.
        updateParents(btn);
        fBTree.seekg(btn.parentOrNextDel*NODE_SIZE,ios::beg);
        btn = readTreeNode();
        btnu.value = getBiggestNum(btn2.nodes);
        btnu.reference = btn2.byteOffset/NODE_SIZE;
        insertRecordInNode(btn,btnu);
        return getNodeRRN(btn);
    }
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


//Navigates the tree from root to find the leaf that 'should' contain the given value.
//Does not check if that node actually holds the value or not.
BTreeNode searchTillLeaf(int RecordID){
    fBTree.seekg(NODE_SIZE,ios::beg);
    BTreeNode btn= readTreeNode();
    vector<BTreeNodeUnit> v;
    //Navigate till the leaf node that might hold the ID value.
    while(btn.stateFlag != LEAF_FLAG){

        v = btn.nodes;
        int nextRRN;
        for(int i = 0;i<v.size();i++){
            //Takes the next Node reference until it either reaches
            // end of vector or finds a node it's value is less than it.
            if(v[i].value == DEL_FLAG){break;}
            nextRRN = v[i].reference;
            if(RecordID<= v[i].value){break;}
        }
        fBTree.seekg(nextRRN*NODE_SIZE,ios::beg);
        btn = readTreeNode();
    }

    return btn;
};


int checkUnderFlow(BTreeNode btn){
    //Loop though the Node's Records to see how many doesn't equal -1
    //and returns the amount;
    int numOfNonEmptyRecords = 0;
    vector<BTreeNodeUnit> v = btn.nodes;
    for(int i=0;i<v.size();i++){
        if(v[i].value != DEL_FLAG &&  v[i].reference != DEL_FLAG){
            numOfNonEmptyRecords++;
        }
    }

    return numOfNonEmptyRecords;
};

int redistributeNode(BTreeNode btn){
    //Go to parent node using parameter node and navigate to it and read it;
    int parentIndex = btn.parentOrNextDel;
    int childRRNorIndex = getNodeRRN(btn);
    fBTree.seekg(parentIndex*NODE_SIZE,ios::beg);
    BTreeNode parentBtn = readTreeNode();
    vector<BTreeNodeUnit> parentRecords = parentBtn.nodes;

    //Now check this node sibling's if they are underflow or not.
    //We store the Sibling nodes and indexes in an array to know which is right and left.
    //And to hold 0, 1,2 node depending on the amount of sibling.
    vector<pair<BTreeNode,int>> siblings;

    for(int i=0;i<parentRecords.size();i++){
        if(parentRecords[i].reference == childRRNorIndex){
            childRRNorIndex = i;
            if(i-1>=0 && parentRecords[i-1].reference!= DEL_FLAG){
                fBTree.seekg(parentRecords[i-1].reference*NODE_SIZE,ios::beg);
                pair<BTreeNode,int> p;
                p.first =readTreeNode();
                p.second = i-1;
                siblings.push_back(p);
            }

            if(i+1<parentRecords.size() && parentRecords[i+1].reference!= DEL_FLAG){
                fBTree.seekg(parentRecords[i+1].reference*NODE_SIZE,ios::beg);
                pair<BTreeNode,int> p;
                p.first =readTreeNode();
                p.second = i+1;
                siblings.push_back(p);
            }
            break;
        }
    }

    //if one of the right and left sibling is not underflow, then you will take either the smallest or
    //biggest record in it and add it to the Original node and overwrite all the affected nods.
    for(int i=0;i<siblings.size();i++){
        int numberOfNonEmptyRecord = checkUnderFlow(siblings[i].first);
        BTreeNodeUnit btnu;
        if(numberOfNonEmptyRecord>=M_SIZE/2){
            //A sibling is not underflow so we take a Record from it and return it's RRN

            if(siblings[i].second = childRRNorIndex-1){
                //The non-underflow sibling if the left so we use the biggest value in it.
                int index;
                for(int x=0;x<siblings[i].first.nodes.size();x++){
                    if(siblings[i].first.nodes[x].reference!= DEL_FLAG){
                        btnu = siblings[i].first.nodes[x];
                        index = x;
                    }
                }
                btn.nodes[M_SIZE/2] = btnu;
                siblings[i].first.nodes[index].value = DEL_FLAG;
                siblings[i].first.nodes[index].reference = DEL_FLAG;

                sort(siblings[i].first.nodes.begin(),siblings[i].first.nodes.end(),&NodesSorterAscending);
                sort(btn.nodes.begin(),btn.nodes.end(),&NodesSorterAscending);

                fBTree.seekg(btn.byteOffset,ios::beg);
                writeTreeNode(btn);
                fBTree.seekg(siblings[i].first.byteOffset,ios::beg);
                writeTreeNode(siblings[i].first);

                updateParents(btn);
                updateParents(siblings[i].first);
            } else if(siblings[i].second = childRRNorIndex+1){
                //The non-underflow sibling if the left so we use the smallest value in it.
                btnu = siblings[i].first.nodes[0];
                btn.nodes[M_SIZE/2] = btnu;
                siblings[i].first.nodes[0].value = DEL_FLAG;
                siblings[i].first.nodes[0].reference = DEL_FLAG;

                sort(siblings[i].first.nodes.begin(),siblings[i].first.nodes.end(),&NodesSorterAscending);
                sort(btn.nodes.begin(),btn.nodes.end(),&NodesSorterAscending);

                fBTree.seekg(btn.byteOffset,ios::beg);
                writeTreeNode(btn);
                fBTree.seekg(siblings[i].first.byteOffset,ios::beg);
                writeTreeNode(siblings[i].first);

                updateParents(btn);
                updateParents(siblings[i].first);
            }

            return getNodeRRN(siblings[i].first);
        }
    }

    //if both sibling will be underflow, return -1;
    return -1;
};

void merge(BTreeNode btn){
    //this functions happens when redistribution isn't possible
    //It will take sibling's node and merge them into a new Node before
    //OverWriting one of them and deleting the other.
    int parentIndex = btn.parentOrNextDel;
    int childIndex = getNodeRRN(btn);
    fBTree.seekg(parentIndex*NODE_SIZE,ios::beg);
    BTreeNode parentBtn = readTreeNode();
};

//takes a node you just deleted from and check if it needs redistributing or merging.
void fixNodeAfteDelete(BTreeNode btn){

    //if number of used records is lower than or equal underflow M_SIZE/2
    int nonEmptyRecords = checkUnderFlow(btn);
    if(nonEmptyRecords<M_SIZE/2){
        //then redistribute
        int redistributed = redistributeNode(btn);
        //if redistribution wasn't possible return -1 if it was possible return;
        if(redistributed == -1){
            //redistribution wasn't possible then merge;
            merge(btn);
        }
    }


}
