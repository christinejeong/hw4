#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    // void rebalance(AVLNode<Key, Value>* node);
    // void updateBalanceAfterInsert(AVLNode<Key, Value>* node);
    // void updateBalanceAfterRemove(AVLNode<Key, Value>* node);
    // AVLNode<Key, Value>* getChildForRemoval(AVLNode<Key, Value>* node);
    // int getHeight(AVLNode<Key, Value>* node);
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    // AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* node) const;
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    // if tree is empty insert root node
    if(!this->root_) {
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      return;
    }

    // start at root and search correct insertion point
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while(current != nullptr) {
      parent = current;
      if(new_item.first < current->getKey()) {
        current = current->getLeft();
      }
      else if(new_item.first > current->getKey()) {
        current = current->getRight();
      }
      else {
        // key already exists, so update value
        current->setValue(new_item.second);
        return;
      }
    }

    // create new node and attach it to parent
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if(new_item.first < parent->getKey()) {
      parent->setLeft(newNode);
      parent->setBalance(parent->getBalance() - 1);
    }
    else {
      parent->setRight(newNode);
      parent->setBalance(parent->getBalance() + 1);
    }

    if (parent->getBalance() != 0) {
      insertFix(parent, newNode);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key, Value>* node)
{
  // base case: stop if reached root
  if(parent == nullptr || parent->getParent() == nullptr) {
    return;
  }

  // grandparent node for rebalancing
  AVLNode<Key, Value>* grandparent = parent->getParent();

  // case 1: parent is the left child of grandparent
  if(parent == grandparent->getLeft()) {
    // insertion into left tree
    grandparent->setBalance(grandparent->getBalance() - 1);

    // balance is now 0 (no height change)
    if(grandparent->getBalance() == 0) {
      return;
    }
    // balance is -1 (height increased)
    else if(grandparent->getBalance() == -1) {
      insertFix(grandparent, parent);
    }
    // balance is -2 (unbalanced)
    else if(grandparent->getBalance() == -2) {
      // left-left case ZIG ZIG
      if(node == parent->getLeft()) {
        rotateRight(grandparent);
        parent->setBalance(0);
        grandparent->setBalance(0);
      }
      // left-right case ZIG ZAG
      else {
        rotateLeft(parent);
        rotateRight(grandparent);

        int balance = node->getBalance();
        if(balance == -1) {
          parent->setBalance(0);
          grandparent->setBalance(1);
          node->setBalance(0);
        }
        else if(balance == 0) {
          parent->setBalance(0);
          grandparent->setBalance(0);
          node->setBalance(0);
        }
        else if(balance == 1) {
          parent->setBalance(-1);
          grandparent->setBalance(0);
          node->setBalance(0);
        }
      }
    }
  }
  // case 2: parent is the right child of grandparent
  else {
    // insertion into right tree
    grandparent->setBalance(grandparent->getBalance() + 1);

    // balance is now 0 (no height change)
    if(grandparent->getBalance() == 0) {
      return;
    }
    // balance is 1 (height increased)
    else if(grandparent->getBalance() == 1) {
      insertFix(grandparent, parent);
    }
    // balance is 2 (unbalanced)
    else if(grandparent->getBalance() == 2) {
      // right-right case ZIG ZIG
      if(node == parent->getRight()) {
        rotateLeft(grandparent);
        parent->setBalance(0);
        grandparent->setBalance(0);
      }
      // right-left case ZIG ZAG
      else {
        rotateRight(parent);
        rotateLeft(grandparent);

        int balance = node->getBalance();
        if(balance == 1) {
          parent->setBalance(0);
          grandparent->setBalance(-1);
          node->setBalance(0);
        }
        else if(balance == 0) {
          parent->setBalance(0);
          grandparent->setBalance(0);
          node->setBalance(0);
        }
        else if(balance == -1) {
          parent->setBalance(1);
          grandparent->setBalance(0);
          node->setBalance(0);
        }
      }
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
  Node<Key, Value>* base = this->internalFind(key);
  if(base == nullptr) {
    return;
  }

  AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(base);
  AVLNode<Key, Value>* swap;

  if (node->getLeft() != nullptr && node->getRight() != nullptr) {
    swap = static_cast<AVLNode<Key,Value>*>(this->predecessor(base));
    nodeSwap(node, swap);
  }

  AVLNode<Key, Value>* parent = node->getParent();
  int diff = 0;
  if (parent != nullptr) {
    // Calculate diff
    if (parent->getLeft() == node) {
      diff = 1;
    } 
    else {
      diff = -1;
    }
  }

  // Update pointers
  AVLNode<Key, Value>* newChild = nullptr;
  if (node->getLeft() != nullptr) {
    newChild = node->getLeft();
  }
  else if (node->getRight() != nullptr) {
    newChild = node->getRight();
  }

  if (parent == nullptr) {
    this->root_ = newChild;
    if (newChild != nullptr) {
      newChild->setParent(nullptr);
    }
  }
  else if (parent->getLeft() == node) {
    parent->setLeft(newChild);
    if (newChild != nullptr) {
      newChild->setParent(parent);
    }
  }
  else if (parent->getRight() == node) {
    parent->setRight(newChild);
    if (newChild != nullptr) {
      newChild->setParent(parent);
    }
  }

  delete node;
  
  removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
  if(node == nullptr) {
    return;
  }

  AVLNode<Key, Value>* parent = node->getParent();
  int ndiff = 0;
  if(parent != nullptr) {
    if(node == parent->getLeft()) {
      ndiff = 1;
    }
    else {
      ndiff = -1;
    }
  }
  
  if(diff == -1) {
    if(node->getBalance() + diff == -1) {
      node->setBalance(-1);
      return;
    }
    else if(node->getBalance() + diff == 0) {
      node->setBalance(0);
      removeFix(parent, ndiff);
    }
    else if(node->getBalance() + diff == -2) {
      AVLNode<Key, Value>* left = node->getLeft();
      if(left == nullptr) return;

      int balance = left->getBalance();
      if(balance == 0) {
        rotateRight(node);
        node->setBalance(-1);
        left->setBalance(1);

      }
      else if(balance == -1) {
        rotateRight(node);
        node->setBalance(0);
        left->setBalance(0);
        removeFix(parent, ndiff);
      }
      else if(balance == 1) {
        AVLNode<Key, Value>* g = left->getRight();
        if(g == nullptr)return;
        rotateLeft(left);
        rotateRight(node);

        int bal = g->getBalance();
        if(bal == 1) {
          node->setBalance(0);
          left->setBalance(-1);
        }
        else if(bal == 0) {
          node->setBalance(0);
          left->setBalance(0);
        }
        else if (bal == -1) {
          node->setBalance(1);
          left->setBalance(0);
        }
        g->setBalance(0);
        removeFix(parent, ndiff);
      }
    }
  }
  // MIRROR
  else if(diff == 1) {
    if(node->getBalance() + diff == 1) {
      node->setBalance(1);
      return;
    }
    else if(node->getBalance() + diff == 0) {
      node->setBalance(0);
      removeFix(parent, ndiff);
      return;
    }
    else if(node->getBalance() + diff == 2) {
      AVLNode<Key, Value>* right = node->getRight();
      if(right == nullptr) return;

      int balance = right->getBalance();
      if(balance == 0) {
        rotateLeft(node);
        node->setBalance(1);
        right->setBalance(-1);
      }
      else if(balance == 1) {
        rotateLeft(node);
        node->setBalance(0);
        right->setBalance(0);
        removeFix(parent, ndiff);
      }
      else if(balance == -1) {
        AVLNode<Key, Value>* g = right->getLeft();
        if(g == nullptr)return;
        rotateRight(right);
        rotateLeft(node);

        int bal = g->getBalance();
        if(bal == -1) {
          node->setBalance(0);
          right->setBalance(1);
        }
        else if(bal == 0) {
          node->setBalance(0);
          right->setBalance(0);
        }
        else if (bal == 1) {
          node->setBalance(-1);
          right->setBalance(0);
        }
        g->setBalance(0);
        removeFix(parent, ndiff);
      }
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* node)
{
  if (node == nullptr) {
    return;
  }

  AVLNode<Key, Value>* pivot = node->getRight();

  if(pivot == nullptr) {
    return;
  }

  pivot->setParent(node->getParent());
  if(node->getParent() == nullptr) {
    this->root_ = pivot;
  }
  else if(node == node->getParent()->getLeft()) {
    node->getParent()->setLeft(pivot);
  }
  else {
    node->getParent()->setRight(pivot);
  }    

  node->setRight(pivot->getLeft());
  if(pivot->getLeft() != nullptr) {
    pivot->getLeft()->setParent(node);
  }

  pivot->setLeft(node);
  node->setParent(pivot);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* node)
{
  if (node == nullptr) {
    return;
  }

  AVLNode<Key, Value>* pivot = node->getLeft();

  if(pivot == nullptr) {
    return;
  }
  pivot->setParent(node->getParent());
  if(node->getParent() == nullptr) {
    this->root_ = pivot;
  }
  else if(node == node->getParent()->getLeft()) {
    node->getParent()->setLeft(pivot);
  }
  else {
    node->getParent()->setRight(pivot);
  }

  node->setLeft(pivot->getRight());
  if(pivot->getRight() != nullptr) {
    pivot->getRight()->setParent(node);
  }

  pivot->setRight(node);
  node->setParent(pivot);
}

#endif
