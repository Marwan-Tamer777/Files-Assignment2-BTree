
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
    BTreeNode btn;
    fBTree.seekg(0,ios::beg);

    for(int i=0;i<NUM_OF_RECORDS;i++){
         btn = readTreeNode();
         printBTreeNode(btn);
    }
    cout<<endl;
};

int SearchARecord(char* filename, int RecordID) {

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

    //Iterate over the Leaf's data to see if the RecordId exists.
    v = btn.nodes;
    for(int i = 0;i<v.size();i++){
        if(v[i].value == RecordID){
            return v[i].reference;
        }
    }

    return -1;
};
