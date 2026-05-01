# Btree-Assignment Malak Elsayed 900223286
Part 7:
Question 1:
The code does not allow duplicates:
if (root != nullptr && root->search(x)) { 
    cout << "Duplicate " << x << " ignored." << endl;
    outputFile << "Duplicate " << x << " ignored." << endl;
    continue;  
}
 if the key already exists in the tree it prints a message and skips the insertion entirely using continue.
 I choose to deal with duplicates to avoid ambiguity when dealing with spliting or deletion. Splits become unpredictable because a duplicate could end up as a separator key in an internal node causing ambiguity about which child to descend into. As for deletion, when we say delete 10 which 10 should we delete? 

Question 2

 
