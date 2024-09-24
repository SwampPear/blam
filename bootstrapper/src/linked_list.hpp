#pragma once

#include <stdexcept>
#include <iostream>

template <typename T>
struct LLNode {
    T *data;
    LLNode<T> *next;
    LLNode<T> *prev;
};

template <typename T>
void replace(LLNode<T> *replaced, LLNode<T> *replacement) {
    if (replaced == nullptr) {
        throw std::runtime_error("The replaced node cannot be null.");
    }

    if (replacement == nullptr) {
        throw std::runtime_error("The replacement node cannot be null.");
    }

    // record old state
    LLNode<T> *oldNext = replaced->next;

    // replace previous
    replacement->prev = replaced->prev;

    // replace next
    LLNode<T> *currNode = replacement;

    while (currNode->next != nullptr) {
        currNode = currNode->next;
    }

    currNode->next = oldNext;

    if (oldNext != nullptr) {
        oldNext->prev = currNode;
    }

    // connect to replacement
    *replaced = *replacement;
}

template <typename T>
void print(LLNode<T> *head) {
    LLNode<T> *current = head;

    while (current != nullptr) {
        if (current->data != nullptr) {
            std::cout << *(current->data);
            if (current->prev != nullptr || current->next != nullptr) {
                std::cout << " - ";
            } else {
                std::cout << " - ";
            }

            if (current->prev != nullptr) {
                std::cout << *(current->prev->data);
            } else {
                std::cout << "n";
            }

            std::cout << ", ";

            if (current->next != nullptr) {
                std::cout << *(current->next->data);
            } else {
                std::cout << "n";
            }

            std::cout << "\n";
        } else {
            std::cout << "null\n";
        }

        current = current->next;
    }

    std::cout << std::endl;
}

LLNode<int> *testList(int min, int max) {
    // node 1
    LLNode<int> *root = new LLNode<int>();
    int *data = new int(min);
    root->data = data;
    root->next = nullptr;
    root->prev = nullptr;

    // test list 1
    LLNode<int> *prevNode = root;

    for (int i = min + 1; i < max; i++) {
        LLNode<int> *newNode = new LLNode<int>();

        int *newData = new int(i);
        newNode->data = newData;

        newNode->next = nullptr;
        newNode->prev = prevNode;

        prevNode->next = newNode;
        prevNode = newNode;
    }

    return root;
}
