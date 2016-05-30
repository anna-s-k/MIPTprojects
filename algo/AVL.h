#ifndef AVL_H
#define AVL_H

class AVL{
public:
    AVL();
    ~AVL();

    bool Insert (int Key);
    bool Delete (int Key);
    bool Find (int Key);
    bool DeleteByNum(int num);

    int KStatics(int num);
    int GetPos(int Key);
private:
    struct Node{
        int key;
        int height;
        int size;
        Node* left;
        Node* right;
        Node(int key);
    };

    Node* root;
    Node* insert(int Key, Node*& node); // ключ и поддерево
    Node* find(int Key);// ближайший узел
    bool removeNode(Node*& node, Node*& par);

    int height(Node* node);
    int bFactor(Node* node);
    void fixHeight(Node* node);
    Node* rotateR(Node* node);
    Node* rotateL(Node* node);
    Node* balance(Node* node);

    int getSize(Node* node);
    void fixSize(Node* node);
    int kstatics(int num, Node* node);

    void kstaticsNode(int num, Node *node, Node*& res, Node*& par);

};

#endif // AVL_H
