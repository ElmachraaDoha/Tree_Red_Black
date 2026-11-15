#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

enum Color {
    RED,
    BLACK
};

struct Node {

    int value;
    Color color;

    Node* left;
    Node* right;

    Node* parent;
};

#endif // NODE_H_INCLUDED
