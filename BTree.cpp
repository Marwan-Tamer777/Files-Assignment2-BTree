//#include "utils.cpp"

void CreateIndexFile(char* fileName,int numberOfRecord, int m){
    fBTree.open(fileName, ios::in | ios::out | ios::trunc);
    NUM_OF_RECORDS = numberOfRecord;
    M_SIZE = m;
    NODE_SIZE = FIELD_SIZE*(2+M_SIZE) + NODE_DELIMITER.length();

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
    BTreeNodeUnit btnu;
    vector<BTreeNodeUnit> v;

    if(btn.stateFlag == -1){
        //Root is completely empty so we overwrite it into a leaf
        btnu.value = RecordID;
        btnu.reference = Reference;
        btn.stateFlag = LEAF_FLAG;
        btn.parentOrNextDel = DEL_FLAG;
        btn.nodes[0] = btnu;
        writeFirstDelTreeNode(btn, DEL_FLAG);
        return 1;
    }
    //Root is not empty, we will traverse to the correct leaf node;
    btn = searchTillLeaf(RecordID);
    //btn now have the leaf node that the new value should be inserted at.
    btnu.value = RecordID;
    btnu.reference = Reference;
    //If it cannot be inserted in that leaf node, the insert function will split.
    return insertRecordInNode(btn,btnu);

};

void DisplayIndexFileContent(char* filename) {
    BTreeNode btn;
    fBTree.seekg(0,ios::beg);

    for(int i=0;i<NUM_OF_RECORDS;i++){
         btn = readTreeNode();
         printBTreeNode(btn);
    }
    cout<<endl;
};

int SearchARecord(char* filename, int RecordID) {

    BTreeNode btn= searchTillLeaf(RecordID);
    vector<BTreeNodeUnit> v;
    //Iterate over the Leaf's data to see if the RecordId exists.
    v = btn.nodes;
    for(int i = 0;i<v.size();i++){
        if(v[i].value == RecordID){
            return v[i].reference;
        }
    }

    return -1;
};

void DeleteRecordFromIndex(char* filename, int RecordID) {
    if(SearchARecord(filename, RecordID) != -1){
        BTreeNode btn= searchTillLeaf(RecordID);
        vector<BTreeNodeUnit> v;
        //Iterate over the Leaf's data to see if the RecordId exists.
        v = btn.nodes;
        for(int i = 0;i<v.size();i++){
            if(v[i].value == RecordID){
                v[i].reference = -1;
                v[i].value = -1;
                btn.nodes = v;
                fBTree.seekg(btn.byteOffset,ios::beg);
                writeTreeNode(btn);
                updateParents(btn);

                //check if underflow;
                //verifyDeletedNode(btn);
                break;
            }
        }
    } else cout << "Not found!";

};