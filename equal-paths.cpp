#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// find depth of leaves
int getLeafDepth(Node* root) {
    // base case: if root is null, return -1
    if(!root) {
        return -1;
    }
    // if leaf node, return 0
    if(!root->left && !root->right) {
        return 0;
    }

    int leftDepth = getLeafDepth(root->left);
    int rightDepth = getLeafDepth(root->right);

    // when one child is missing
    if(leftDepth == -1) {
        return rightDepth + 1;
    }
    if(rightDepth == -1) {
        return leftDepth + 1;
    }
    if(leftDepth != rightDepth) {
        return -2;
    }
    return leftDepth + 1;
}


bool equalPaths(Node * root)
{
    if(!root) {
        return true;
    }

    return getLeafDepth(root) >= 0;
}

