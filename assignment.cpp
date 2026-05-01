#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int splitCount = 0;
class BTreeNode {
public:
    int * keys;
    int t;
    BTreeNode ** children;
    int n;
    bool leaf;
    
    BTreeNode(int _t, bool _leaf);
    void insertNonFull(int k);
    void splitChild(int i, BTreeNode * y);
    void traverse();
    void levelOrder();
    bool search(int k);
    void remove(int k);
    int height();
    
};

void BTreeNode::insertNonFull(int k) {                    // Insert a key into a node where we already know that its not full
    int i = n - 1;                                        // Start from the rightmost existing key

    if (leaf) {                                           // If this node is a leaf, then insert key
        while (i >= 0 && keys[i] > k) {                   // Shift larger keys one position to the right
            keys[i + 1] = keys[i];                        // Move key to make room for the new key
            i--;                                          // Move to the previous key
        }

        keys[i + 1] = k;                                  // Insert the new key in its correct sorted position
        n++;                                              // Increase the number of keys in that node
    }
    else {                                                // Like we took in class, if it was an internal node
        while (i >= 0 && keys[i] > k) {                   // Find the child that should contain the new key
            i--;                                          // Move left until the correct position is found
        }

        i++;                                              // Thats why we convert key index to child index

        if (children[i]->n == 2 * t - 1) {                // If the target child is full
            splitChild(i, children[i]);                   // Split the full child into two nodes

            if (keys[i] < k) {                            // Decide whether to go to the left or right split child
                i++;                                      // Move to the right child if k is greater than the promoted key
            }
        }

        children[i]->insertNonFull(k);                    // Recursively insert the key into the appropriate child
    }
}
void BTreeNode::splitChild(int i, BTreeNode *y) { 
    splitCount++;                // Split the full child y at index i
    BTreeNode *z = new BTreeNode(y->t, y->leaf);                  // Create a new node to store half of y's keys
    z->n = t - 1;                                                 // z will contain the last (t-1) keys of y

    for (int j = 0; j < t - 1; j++)                               // Copy the last (t-1) keys from y to z
        z->keys[j] = y->keys[j + t];                              // Transfer keys from the right half of y

    if (!y->leaf) {                                               // If y is not a leaf, copy its child pointers too
        for (int j = 0; j < t; j++)                               // Copy the last t children of y to z
            z->children[j] = y->children[j + t];                  // Transfer child pointers
    }

    y->n = t - 1;                                                 // Reduce the number of keys in y

    for (int j = n; j >= i + 1; j--)                              // Shift this node's children to make room for z
        children[j + 1] = children[j];                            // Move child pointers one position right

    children[i + 1] = z;                                          // Link z as the new child after y

    for (int j = n - 1; j >= i; j--)                              // Shift this node's keys to make room for y's middle key
        keys[j + 1] = keys[j];                                    // Move keys one position right

    keys[i] = y->keys[t - 1];                                     // Promote y's middle key to this node

    n++;                                                          // Increase the number of keys in this node
}

void BTreeNode::traverse() {                                  // Traverse all nodes in the subtree rooted at this node
    int i;                                                    // Index variable

    for (i = 0; i < n; i++) {                                 // Visit all keys and their left children
        if (!leaf)                                             // If this is not a leaf
            children[i]->traverse();                           // Recursively traverse the i-th child

        cout << " " << keys[i];                                // Print the current key
    }

    if (!leaf)                                                 // After all keys, visit the rightmost child
        children[i]->traverse();                               // Recursively traverse the last child
}
//Queue is required for level order traversal and since we can't use STL containers we will implement it
class NodeQueue {
private:
    BTreeNode** arr;                          // Dynamic array to store queue elements
    int front;                                // Index of the front element
    int rear;                                 // Index where the next element will be inserted
    int capacity;                             // Maximum number of elements the queue can hold

public:
    NodeQueue(int size = 1000) {              // Constructor
        capacity = size;                      // Set queue capacity
        arr = new BTreeNode*[capacity];       // Allocate memory for the array
        front = 0;                            // Initialize front
        rear = 0;                             // Initialize rear
    }

    ~NodeQueue() {                            // Destructor
        delete[] arr;                         // Free dynamically allocated memory
    }

    bool isEmpty() const {                    // Check whether the queue is empty
        return front == rear;
    }

    bool isFull() const {                     // Check whether the queue is full
        return rear == capacity;
    }

    void enqueue(BTreeNode* node) {           // Insert an element at the rear
        if (isFull()) {
            cout << "Queue overflow!" << endl;
            return;
        }
        arr[rear++] = node;
    }

    BTreeNode* dequeue() {                    // Remove and return the front element
        if (isEmpty()) {
            cout << "Queue underflow!" << endl;
            return nullptr;
        }
        return arr[front++];
    }

    int size() const {                        // Return the number of elements currently in the queue
        return rear - front;
    }
};

void BTreeNode::levelOrder() {
    
    NodeQueue q;                                    // Create queue object for level order traversal
    
    q.enqueue(this);                                // Start by adding the root node to the queue
    int level = 0;
    while (!q.isEmpty()) {                          // Continue until queue becomes empty
        int levelSize = q.size();                   // Get number of nodes at current level
        
        for (int i = 0; i < levelSize; i++) {       // Process all nodes at current level
            BTreeNode* current = q.dequeue();       // Remove and get the front node from queue
            if (i == 0) cout << "Level " << level << ": ";
            cout << "[ ";                           
            
            for (int j = 0; j < current->n; j++) {  // Loop through all keys in current node
                cout << current->keys[j];
                if (j < current->n - 1)
                cout << " ";
            }
            
            cout << "]";
            if (i < levelSize - 1)
            cout << " ";                          
            
            if (!current->leaf) {                   // If current node is not a leaf
                for (int j = 0; j <= current->n; j++) {  // Loop through all children (n+1 children)
                    q.enqueue(current->children[j]);     // Add each child to the queue for next level
                }
            }
        }
        
        cout << endl;
        level++;                               
    }
}
bool BTreeNode::search(int k) {                     
    int i = 0;                                       

    while (i < n && k > keys[i])                   // Find first key >= k
        i++;

    if (i < n && keys[i] == k)                     // If found in this node
        return true;

    if (leaf)                                      // If leaf and not found
        return false;

    return children[i]->search(k);                 
}
int BTreeNode::height() {
    if (leaf) return 0;                              // Leaf nodes are at height 0
    return 1 + children[0]->height();               // Add 1 for each level going down
}
void BTreeNode::remove(int k) {
    int i = 0;
    int height();

    // Find the first key >= k in this node
    while (i < n && keys[i] < k)
        i++;

    // Key is in this node 
    if (i < n && keys[i] == k) {

        if (leaf) {
            // ── CASE 1: Node is a leaf so just delete the key 
            for (int j = i; j < n - 1; j++)
                keys[j] = keys[j + 1];               // Shift keys left to fill the gap
            n--;

        } else {
            // ── CASE 2a: Left child has at least t keys
            if (children[i]->n >= t) {
                // Find the in-order predecessor (rightmost key in left subtree)
                BTreeNode* curr = children[i];
                while (!curr->leaf)
                    curr = curr->children[curr->n];
                int pred = curr->keys[curr->n - 1];   // Predecessor key

                keys[i] = pred;                       // Replace k with predecessor
                children[i]->remove(pred);            // Delete predecessor from left subtree

            // Right child has at least t keys
            } else if (children[i + 1]->n >= t) {
                // Find the in-order successor (leftmost key in right subtree)
                BTreeNode* curr = children[i + 1];
                while (!curr->leaf)
                    curr = curr->children[0];
                int succ = curr->keys[0];             // Successor key

                keys[i] = succ;                       // Replace k with successor
                children[i + 1]->remove(succ);       // Delete successor from right subtree

            // Both children have t-1 keys so merge 
            } else {
                // Merge children[i+1] and keys[i] into children[i]
                BTreeNode* leftChild  = children[i];
                BTreeNode* rightChild = children[i + 1];

                // Pull down the key being deleted into the left child
                leftChild->keys[t - 1] = k;
                leftChild->n = t - 1;

                // Copy all keys from the right child
                for (int j = 0; j < rightChild->n; j++)
                    leftChild->keys[t + j] = rightChild->keys[j];

                // Copy all children from the right child (if not a leaf)
                if (!leftChild->leaf) {
                    for (int j = 0; j <= rightChild->n; j++)
                        leftChild->children[t + j] = rightChild->children[j];
                }

                leftChild->n = 2 * t - 1;            // Merged node now has 2t-1 keys

                // Remove k and children[i+1] from this node
                for (int j = i; j < n - 1; j++)
                    keys[j] = keys[j + 1];
                for (int j = i + 1; j < n; j++)
                    children[j] = children[j + 1];
                n--;

                delete rightChild;                    // Free the merged child
                leftChild->remove(k);                 // Now delete k from the merged node
            }
        }

    // Key is NOT in this node so recurse into appropriate child 
    } else {
        if (leaf) {
            // k is not in the tree at all
            cout << "Key " << k << " not found in the tree." << endl;
            return;
        }

        // i is the index of the child we need to descend into.
        // Check if that child has the minimum number of keys (t-1).
        bool lastChild = (i == n);                    

        if (children[i]->n < t) {                    // Child has only t-1 keys so fix it first

            // Borrow from left sibling
            if (i > 0 && children[i - 1]->n >= t) {
                BTreeNode* child    = children[i];
                BTreeNode* leftSib  = children[i - 1];

                // Shift child's keys right to make room at index 0
                for (int j = child->n - 1; j >= 0; j--)
                    child->keys[j + 1] = child->keys[j];

                // Shift child's children right if not a leaf
                if (!child->leaf) {
                    for (int j = child->n; j >= 0; j--)
                        child->children[j + 1] = child->children[j];
                }

                // Move parent's separator key down into child
                child->keys[0] = keys[i - 1];

                // Move sibling's last child pointer to child (if internal)
                if (!child->leaf)
                    child->children[0] = leftSib->children[leftSib->n];

                // Move sibling's last key up to parent
                keys[i - 1] = leftSib->keys[leftSib->n - 1];

                child->n++;
                leftSib->n--;

            // Borrow from right sibling 
            } else if (i < n && children[i + 1]->n >= t) {
                BTreeNode* child    = children[i];
                BTreeNode* rightSib = children[i + 1];

                // Move parent's separator key down into child's last position
                child->keys[child->n] = keys[i];

                // Move right sibling's first child pointer to child (if internal)
                if (!child->leaf)
                    child->children[child->n + 1] = rightSib->children[0];

                // Move right sibling's first key up to parent
                keys[i] = rightSib->keys[0];

                // Shift right sibling's keys left
                for (int j = 1; j < rightSib->n; j++)
                    rightSib->keys[j - 1] = rightSib->keys[j];

                // Shift right sibling's children left
                if (!rightSib->leaf) {
                    for (int j = 1; j <= rightSib->n; j++)
                        rightSib->children[j - 1] = rightSib->children[j];
                }

                child->n++;
                rightSib->n--;

            // Neither sibling has extra keys so merge 
            } else {
                // Merge children[i] with its sibling
                // If i is the last child, merge with left sibling instead
                if (i == n)
                    i--;                              // Adjust i to merge left sibling into children[i-1]

                BTreeNode* leftChild  = children[i];
                BTreeNode* rightChild = children[i + 1];

                // Pull the parent separator key down into the left child
                leftChild->keys[t - 1] = keys[i];

                // Copy all keys from right child into left child
                for (int j = 0; j < rightChild->n; j++)
                    leftChild->keys[t + j] = rightChild->keys[j];

                // Copy all children pointers from right child
                if (!leftChild->leaf) {
                    for (int j = 0; j <= rightChild->n; j++)
                        leftChild->children[t + j] = rightChild->children[j];
                }

                leftChild->n = 2 * t - 1;

                // Remove separator key and right child pointer from this node
                for (int j = i; j < n - 1; j++)
                    keys[j] = keys[j + 1];
                for (int j = i + 1; j < n; j++)
                    children[j] = children[j + 1];
                n--;

                delete rightChild;
            }
        }

        
        if (lastChild && i > n)
            children[i - 1]->remove(k);
        else
            children[i]->remove(k);
    }
}
BTreeNode::BTreeNode(int _t, bool _leaf) {
    t = _t; // Store the minimum degree of the B-Tree
    leaf = _leaf; // Mark whether this node is a leaf or internal node
    keys = new int[2 * t - 1]; // Allocate array for max (2t-1) keys a node can hold
    children = new BTreeNode*[2 * t]; // Allocate array for max (2t) child pointers
    n = 0; // Node starts with 0 keys
}

class IntVector {
private:
    int* arr;                                        // Dynamic array to store integers
    int capacity;                                    // Maximum capacity
    int size;                                        // Number of elements

public:
    IntVector() {                                    // Constructor
        capacity = 10;                               // Start with capacity of 10
        arr = new int[capacity];                     // Allocate memory
        size = 0;                                    // Start with no elements
    }

    ~IntVector() {                                   // Destructor
        delete[] arr;                                
    }

    void push_back(int x) {                          // Add element to the end
        if (size == capacity) {                      // If array is full
            capacity *= 2;                           // Double the capacity
            int* newArr = new int[capacity];         // Allocate new larger array
            for (int i = 0; i < size; i++)           // Copy old elements
                newArr[i] = arr[i];                  // Copy each element
            delete[] arr;                            // Free old array
            arr = newArr;                            // Point to new array
        }
        arr[size++] = x;                             // Insert element and increment size
    }

    int get(int i) const {                           // Get element at index
        return arr[i];                               // Return element at position i
    }

    int getSize() const {                            // Return number of elements
        return size;                                 // Return current size
    }
};

void serialize(BTreeNode* node, ofstream &out) {
    if (node == nullptr) return;

    // Write node info
    out << node->n << " " << node->leaf << " ";

    // Write all keys in this node
    for (int i = 0; i < node->n; i++)
        out << node->keys[i] << " ";

    out << "\n";

    // Recursively write children
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++)
            serialize(node->children[i], out);
    }
}
void saveTree(BTreeNode* root) {
    ofstream out("snapshot.dat");

    if (!out) {
        cout << "Error opening snapshot.dat\n";
        return;
    }

    serialize(root, out);

    out.close();
    cout << "Tree saved to snapshot.dat\n";
}


BTreeNode* deserialize(ifstream &in, int t) {
    int n;
    bool leaf;

    if (!(in >> n >> leaf))
        return nullptr;

    BTreeNode* node = new BTreeNode(t, leaf);   
    node->n = n;

    for (int i = 0; i < n; i++)
        in >> node->keys[i];

    if (!leaf) {
        for (int i = 0; i <= n; i++)
            node->children[i] = deserialize(in, t);
    }

    return node;
}

BTreeNode* loadTree(int t) {
    ifstream in("snapshot.dat");

    if (!in) {
        cout << "Cannot open snapshot.dat\n";
        return nullptr;
    }

    BTreeNode* root = deserialize(in, t);

    in.close();
    cout << "Tree restored from snapshot.dat\n";

    return root;
}



int main() {
    int t = 2;                             // Set minimum degree t=2 
    BTreeNode* root = nullptr;             // Start with an empty tree, root is null

    ofstream logFile("log.txt");                 
    if (!logFile) {                               
       cout << "Could not create log.txt" << endl; 
       return 1;                                 
    } 

    ofstream outputFile("output.txt");            
    if (!outputFile) {                            
        cout << "Could not create output.txt" << endl; 
        return 1;                                 
    } 

    IntVector insertedKeys;                                              

    ifstream file("input.txt");            // Open the input.txt file for reading
    if (!file) {                           // Check if the file failed to open
        cout << "Could not open input.txt" << endl;  // If file wasn't found
        outputFile<< "Could not open input.txt" << endl;
        return 1;                          
    }

    string line;                           // String to hold each line read from the file
    while (getline(file, line)) {          // Read the file line by line until end 
        if (line.empty()) continue;        // Skip blank lines without doing anything

        char op;                           // Variable to hold the operation character (I, D, S)
        int x;                             // Variable to hold the integer value from the line

        stringstream ss(line);             // Wrap the line in a stream for easy parsing
        if (!(ss >> op >> x)) {            // Try to parse an operation and a number from the line
            cout << "Invalid line ignored: \"" << line << "\"" << endl; 
            outputFile<< "Invalid line ignored: \"" << line << "\"" << endl; 
            continue;                      // Skip this line and move to the next one
        }

        if (op == 'I') {
            
            try {
                cout << "Insert " << x << ":" << endl;  // Print which value is being inserted
                outputFile<< "Insert " << x << ":" << endl;

                if (root != nullptr && root->search(x)) { 
                             
                    cout << "Duplicate " << x << " ignored." << endl;
                    outputFile<< "Duplicate " << x << " ignored." << endl;
                    continue;  
                } 

                insertedKeys.push_back(x);                                   


                if (root == nullptr) {                   // Check if the tree is currently empty
                    root = new BTreeNode(t, true);       // Create the very first node as a leaf
                    root->keys[0] = x;                   // Place the key as the only key in root
                    root->n = 1;                         // Update key count to 1
                } else {
                    if (root->n == 2 * t - 1) {          // Check if the root is completely full
                        BTreeNode* newRoot = new BTreeNode(t, false);   // Create a new empty internal root
                        newRoot->children[0] = root;                    // Old root becomes first child of new root
                        newRoot->splitChild(0, root);                   // Split the old full root into two children

                        int i = 0;                       // Start at the first child of the new root
                        if (newRoot->keys[0] < x) i++;  // Move to right child if key belongs there
                        newRoot->children[i]->insertNonFull(x);         // Insert key into the correct child

                        root = newRoot;                  // Update root pointer to the new root
                    } else {
                        root->insertNonFull(x);          // Root has space so insert directly
                    }
                }
            } catch (const bad_alloc& e) {               // Catch memory allocation 
                cout << "Memory allocation failed during Insert " << x << ": " << e.what() << endl;
                outputFile<< "Memory allocation failed during Insert " << x << ": " << e.what() << endl;
                continue;                                 // Skip to next line, tree state unchanged
            } catch (const exception& e) {               
                cout << "Error during Insert " << x << ": " << e.what() << endl;
                outputFile<< "Error during Insert " << x << ": " << e.what() << endl;
                continue;                                 // Skip to next line safely
            } catch (...) {                              // In case for unknown or unexpected errors
                cout << "Unknown error during Insert " << x << endl;
                outputFile<< "Unknown error during Insert " << x << endl;
                continue;                                 // Skip to next line safely
            }

        } else if (op == 'D') {
            
            try {
                cout << "Delete " << x << ":" << endl;  // Print which value is being deleted
                outputFile << "Delete " << x << ":" << endl;

                if (root == nullptr) {                   // Check if tree is empty before deleting
                    cout << "Tree is empty, cannot delete " << x << endl;  // Warn user nothing to delete
                    outputFile<< "Tree is empty, cannot delete " << x << endl;
                } else {
                    root->remove(x);                     // Call remove on the root to delete the key

                    if (root->n == 0 && !root->leaf) {   // Check if root became empty after deletion
                        BTreeNode* oldRoot = root;       // Save pointer to the now-empty root
                        root = root->children[0];        // Promote the only child to become the new root
                        delete oldRoot;                  // Free the memory of the old empty root
                    }
                }
            } catch (const exception& e) {               // Catch any standard exceptions during delete
                cout << "Error during Delete " << x << ": " << e.what() << endl;
                outputFile<< "Error during Delete " << x << ": " << e.what() << endl;
                continue;                                 // Skip to next line safely
            } catch (...) {                              // Catch anything unexpected/unknown
                cout << "Unknown error during Delete " << x << endl;
                outputFile<< "Unknown error during Delete " << x << endl;
                continue;                                 // Skip to next line safely
            }

        } else if (op == 'S') {
            
            try {
                cout << "Search " << x << ":" << endl;  // Print which value is being searched
                outputFile<< "Search " << x << ":" << endl;

                if (root == nullptr) {                   // Check if tree is empty before searching
                    cout << x << " not found (tree is empty)." << endl;  // Tree has no elements
                    outputFile<< x << " not found (tree is empty)." << endl;
                } else {
                    bool found = root->search(x);        // Search for the key starting from root
                    cout << x << (found ? " found." : " not found.") << endl;  // Print result of the search
                    outputFile<< x << (found ? " found." : " not found.") << endl;
                }
            } catch (const exception& e) {               // Catch any errors during search
                cout << "Error during Search " << x << ": " << e.what() << endl;
                outputFile<< "Error during Search " << x << ": " << e.what() << endl;
                continue;                                 // Skip to next line safely
            } catch (...) {                              // Catch anything unexecpted/unkonw 
                cout << "Unknown error during Search " << x << endl;
                outputFile<< "Unknown error during Search " << x << endl;
                continue;                                 // Skip to next line safely
            }

        } else {
            cout << "Invalid operation ignored: \"" << line << "\"" << endl;  // Operation letter not I, D, or S
            outputFile<< "Invalid operation ignored: \"" << line << "\"" << endl;
            continue;                                     // Skip printing the tree for unrecognized operations
        }

        //  Print tree state after each valid operation 
        try {
            if (root == nullptr) {
                cout << "Tree is empty." << endl;        
                outputFile<< "Tree is empty." << endl;
            
            } else                       
                root->levelOrder();                      
        } catch (const exception& e) {    //// Catch any errors that occur during printing               
            cout << "Error printing tree: " << e.what() << endl;
            outputFile<< "Error printing tree: " << e.what() << endl;
        } catch (...) {                                  // Catch anything unexpected/unkown
            cout << "Unknown error printing tree." << endl;
            outputFile<< "Unknown error printing tree." << endl;
        }
        cout << "Total splits so far: " << splitCount << endl;
        outputFile<< "Total splits so far: " << splitCount << endl; 
        logFile << "Total splits so far: " << splitCount << endl; 

        cout << endl;
        outputFile<< endl;                                  
    }

    file.close(); 
    
    logFile << "Total splits performed: " << splitCount << endl;
    outputFile<< "Total splits performed: " << splitCount << endl;  

    // To compare record original tree
    int originalHeight = (root == nullptr) ? 0 : root->height();
    int originalSplits = splitCount;
    int totalSplits = splitCount; 

    cout << "Original Tree:" << endl;
    outputFile<< "Original Tree:" << endl;
    cout << "Height: " << originalHeight << endl;
    outputFile<< "Height: " << originalHeight << endl;
    cout << "Splits: " << originalSplits << endl;
    outputFile<< "Splits: " << originalSplits << endl;
    cout << "Structure:" << endl;
    outputFile<< "Structure:" << endl;

    // logFile << "Original Tree:" << endl;
    // logFile << "Height: " << originalHeight << endl;
    // logFile << "Splits: " << originalSplits << endl;
    // logFile << "Structure:" << endl;

    if (root != nullptr) {                                  // Check if the B-tree is not empty
        ostringstream s1;                                   // Create a string stream to capture output instead of printing directly
        streambuf* b1 = cout.rdbuf(s1.rdbuf());            

        root->levelOrder();                                 // Perform level order traversal (output goes into s1)

        cout.rdbuf(b1);                                     // Restore original cout 

        string ts1 = s1.str();                              // Extract the captured traversal output as a string

        cout << ts1;                                        // Print the stored traversal output to the console
       logFile << ts1;                                     // Also write the same output into the log file
    }
    // Clear tree
    root = nullptr;
    splitCount = 0;                                  // Reset split counter

    cout << "Rebuilding in reverse order..." << endl;
   outputFile<< "Rebuilding in reverse order..." << endl;

    // Rebuild in reverse order
    for (int i = insertedKeys.getSize() - 1; i >= 0; i--) {
        int x = insertedKeys.get(i);

        try {
            if (root == nullptr) {
                root = new BTreeNode(t, true);
                root->keys[0] = x;
                root->n = 1;
            } else {
                if (root->n == 2 * t - 1) {
                    BTreeNode* newRoot = new BTreeNode(t, false);
                    newRoot->children[0] = root;
                    newRoot->splitChild(0, root);
                    int j = 0;
                    if (newRoot->keys[0] < x) j++;
                    newRoot->children[j]->insertNonFull(x);
                    root = newRoot;
                } else {
                    root->insertNonFull(x);
                }
            }

            cout << "Insert " << x << ":" << endl;
            outputFile<< "Insert " << x << ":" << endl;

            if (root != nullptr) {
                ostringstream treeStream;
                streambuf* coutBuf = cout.rdbuf(treeStream.rdbuf());
                root->levelOrder();
                cout.rdbuf(coutBuf);
                string treeState = treeStream.str();
                cout << treeState;
                logFile << treeState;
            }

        } catch (const exception& e) {
            cout << "Error during rebuild insert " << x << ": " << e.what() << endl;
            outputFile << "Error during rebuild insert " << x << ": " << e.what() << endl;
        }
    }
    //Compare Original and Reversed Tree
    int reversedHeight = (root == nullptr) ? 0 : root->height();
    int reversedSplits = splitCount;
    

    cout << "Reversed Tree:" << endl;
    cout << "Height: " << reversedHeight << endl;
    cout << "Splits: " << reversedSplits << endl;
    cout << "Structure:" << endl;

    outputFile << "Reversed Tree:" << endl;
    outputFile << "Height: " << reversedHeight << endl;
    outputFile << "Splits: " << reversedSplits << endl;
    outputFile << "Structure:" << endl;
    if (root != nullptr) {
        ostringstream s2;
        streambuf* b2 = cout.rdbuf(s2.rdbuf());
        root->levelOrder();
        cout.rdbuf(b2);
        string ts2 = s2.str();
        cout << ts2;
        outputFile << ts2;
    }

    cout << "\nTotal splits = " << totalSplits + reversedSplits << endl;
    outputFile << "\nTotal splits = " << totalSplits + reversedSplits << endl;

    cout << "Comparison:" << endl;
    cout << "Height - Original: " << originalHeight << " | Reversed: " << reversedHeight << (originalHeight == reversedHeight ? " : Same" : " : Different") << endl;
    cout << "Splits - Original: " << originalSplits << " | Reversed: " << reversedSplits << (originalSplits == reversedSplits ? " : Same" : " : Different") << endl;

    outputFile << "Comparison:" << endl;
    outputFile << "Height - Original: " << originalHeight << " | Reversed: " << reversedHeight << (originalHeight == reversedHeight ? " : Same" : " : Different") << endl;
    outputFile << "Splits - Original: " << originalSplits << " | Reversed: " << reversedSplits << (originalSplits == reversedSplits ? " : Same" : " : Different") << endl;

    logFile.close();
    
    saveTree(root);
    root = loadTree(t);
    if (root){
        root->levelOrder();
    }
    cout << "Extra Challenge Question" << endl;
    outputFile<< "Extra Challenge Question" << endl;
    root = nullptr;
    splitCount = 0;
    int prevHeight = 0;

    int descending[] = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10};  

    for (int i = 0; i < 10; i++) {
        int x = descending[i];

        try {
            if (root == nullptr) {
                root = new BTreeNode(t, true);
                root->keys[0] = x;
                root->n = 1;
            } else {
                if (root->n == 2 * t - 1) {
                    BTreeNode* newRoot = new BTreeNode(t, false);
                    newRoot->children[0] = root;
                    newRoot->splitChild(0, root);
                    int j = 0;
                    if (newRoot->keys[0] < x) j++;
                    newRoot->children[j]->insertNonFull(x);
                    root = newRoot;
                } else {
                    root->insertNonFull(x);
                }
            }

            int currentHeight = root->height();                      // Get height after this insert
            bool heightGrew = currentHeight > prevHeight;            // Check if height increased
            prevHeight = currentHeight;                              // Update previous height

            cout << "Insert " << x << ":" << endl;
            outputFile << "Insert " << x << ":" << endl;
            

            // Print tree
            ostringstream treeStream;
            streambuf* coutBuf = cout.rdbuf(treeStream.rdbuf());
            root->levelOrder();
            cout.rdbuf(coutBuf);
            string treeState = treeStream.str();
            cout << treeState;
            outputFile << treeState;
            

            // Print analysis after each insert
            cout << "Height: " << currentHeight;
            outputFile << "Height: " << currentHeight;
            
            if (heightGrew) {
                cout << " (grew!)";                                  // Flag when height increases
                outputFile << " (grew!)";
                
            }
            cout << " | Splits so far: " << splitCount << endl;
            outputFile << " | Splits so far: " << splitCount << endl;
            
            cout << endl;
            outputFile << endl;

        } catch (const exception& e) {
            cout << "Error inserting " << x << ": " << e.what() << endl;
            outputFile << "Error inserting " << x << ": " << e.what() << endl;
        }
    }
    // Final analysis
    
    cout << "Final Height : " << root->height() << endl;
    cout << "Total Splits : " << splitCount << endl;
    
    outputFile << "Final Height : " << root->height() << endl;
    outputFile << "Total Splits : " << splitCount << endl;
    
    outputFile.close();
    return 0;                                            
}


