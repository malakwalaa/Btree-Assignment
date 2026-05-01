# Btree-Assignment Malak Elsayed 900223286
Part 7:
Question 1:
The code does not allow duplicates:
```cpp
if (root != nullptr && root->search(x)) { 
    cout << "Duplicate " << x << " ignored." << endl;
    outputFile << "Duplicate " << x << " ignored." << endl;
    continue;  
}
```
 if the key already exists in the tree it prints a message and skips the insertion entirely using continue.
 I choose to deal with duplicates to avoid ambiguity when dealing with spliting or deletion. Splits become unpredictable because a duplicate could end up as a separator key in an internal node causing ambiguity about which child to descend into. As for deletion, when we say delete 10 which 10 should we delete? 

Question 2:
Yes the tree is balanced because every insertion goes down to a leaf, splits propagate upward, also, the tree only grows in height when the root splits, meaning all leaves are always at the same level.

Question 3:
When there is an over flow at the root, a new empty root is created, the old root is split the middle key 20 is promoted up to the new root. This is the only moment the tree grows in height.
```cpp
if (root->n == 2 * t - 1) {                  // root has 3 keys, it's full → true
    BTreeNode* newRoot = new BTreeNode(t, false); // create new empty root
    newRoot->children[0] = root;                  // old full root [10 20 30] becomes first child
    newRoot->splitChild(0, root);                 // split it:
                                                  //   left  → [10]
                                                  //   right → [30]
                                                  //   20 promoted to newRoot
    int i = 0;
    if (newRoot->keys[0] < x) i++;               // 20 < 40 → true, so i=1 (go right)
    newRoot->children[i]->insertNonFull(x);       // insert 40 into right child [30]
    root = newRoot;                               // new root is now [20]
}
```
Part 10:
Question 1:
A 2-3-4 tree is simply a B-Tree with t=2. The name comes from the fact that nodes can have 2, 3, or 4 children.

Question 2:
With t=2, a node holds at most 2t-1 = 3 keys, which means at most 2t = 4 children.

Question 3:
When a node is full (has 3 keys), the middle key is promoted up to the parent and the node is split into two nodes, left gets the smaller key, right gets the larger key. If the root splits, a new root is created and the tree grows in height by 1.

Question 4:
O(log n) 

Part 11:
Question 1:
Because the tree structure matters, not just the values. If saved only keys, we lose which keys belong to which node, which nodes are leaves, and the parent-child relationships. When rebuilding we wouldn't know how to rebuild the hierarchy there will just be a flat list of numbers with no way to reconstruct the correct tree shape.

Question 2:
serialize function writes the leaf flag alongside n for every node
```cpp
out << node->n << " " << node->leaf << " ";
```




 
