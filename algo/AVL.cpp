#include "AVL.h"
#include <iostream>
#include <stdlib.h>

AVL::AVL(){
    root = 0;
}

AVL::~AVL(){
    if (root != 0)
        delete root;
}

AVL::Node::Node(int Key){
    key = Key;
    left = 0;
    right = 0;
    height = 1;
    size = 1;
}

AVL::Node* AVL::find(int Key){
    Node * cur_node = root;
    while(cur_node != 0){
        if (cur_node->key == Key) return cur_node;
        if (Key > cur_node->key){
            if (cur_node->right == 0) return cur_node;
            cur_node = cur_node->right;
            continue;
        }
        if (Key < cur_node->key){
            if (cur_node->left == 0) return cur_node;
            cur_node = cur_node->left;
            continue;
        }
    }
    //return cur_node;
}


//такой же как в двоичном дереве поиска
bool AVL::Find(int Key){
    if (find(Key) == 0 || find(Key)->key != Key)
        return false;
    else
        return true;
}

bool AVL::Insert(int Key){

    if (root == 0){
        root = new Node(Key);
        return true;
    }
    else{
        Node* node = insert(Key, root);
        if (node == 0)
            return 0;
        else{
            root = node;
            return true;
        }
    }
}

AVL::Node* AVL::insert(int Key, Node*& node){
    if (Key == node->key) return 0;
    if (Key > node->key){
        if (node->right == 0){
            node->right = new Node(Key);
            //node->size++;
            //root = balance(root);
        }
        else{
            node->right = insert(Key, node->right);
        }
    }
    if (Key < node->key){
        if (node->left == 0) {
            node->left = new Node(Key);
            //node->size++;
        }
        else{
            node->left = insert(Key, node->left);
        }
    }
    return balance(node);
}

bool AVL::removeNode(Node*& cur_node, Node*& par){
    if (cur_node == 0) return false;

    //найденный узел не имеет поддеревьев
    if(cur_node->left == 0 && cur_node->right == 0) {
        cur_node == par->left ? par->left = 0 : par->right = 0;
        delete cur_node;
        par->size--;
        par->height--;
        root = balance(root);

        return true;
    }
    //найденный узел имеет одно поддерево
    if(cur_node->left == 0 || cur_node->right == 0){
        if(cur_node->left != 0){
            (par->left == cur_node ? par->left : par->right) = cur_node->left;
        }
        else{
            (par->left == cur_node ? par->left : par->right) = cur_node->right;
        }
        root = balance(root);
        return true;
    }
    //найденный имеет два поддерева
    Node* find_node = cur_node->left;//наибольший из наим
    Node* fpar = par;
    //пока не найдем max из min
    while(find_node->right != 0){
        fpar = find_node;
        find_node = find_node->right;
    }
    //нет правого
    if (find_node->left != 0){
        cur_node->key = find_node->key;//заменяем удаляемый на след. за ним
        //fpar = find_node->left;//текущий узел без левого подерева заменяем на правый узел
        find_node = find_node->left;
        fpar != par ? fpar->right = find_node : fpar->left = find_node;
    }
    //нет правого, левого
    else{
        fpar != par ? fpar->right = 0 : fpar->left = 0;
        cur_node->key = find_node->key;//заменяем удаляемый на след. за ним
        delete find_node;
    }
    root = balance(root);
    return true;
}

bool AVL::Delete(int Key){
    Node* cur_node = root;
    Node* par = root;
    while(cur_node != 0){
        if (cur_node->key == Key) break;
        if (Key > cur_node->key){
            par = cur_node;
            cur_node = cur_node->right;
            continue;
        }
        if (Key < cur_node->key){
            par = cur_node;
            cur_node = cur_node->left;
            continue;
        }
    }
    return removeNode(cur_node, par);

}

int AVL::height(Node* node){
    return(node != 0 ? node->height : 0);
}

int AVL::bFactor(Node *node){
    return (height(node->right) - height(node->left));
}

void AVL::fixHeight(Node* node)
{
    int hl = height(node->left);
    int hr = height(node->right);
    node->height = (hl > hr ? hl : hr) + 1;
}

int AVL::getSize(Node *node){
    return (node != 0 ? node->size : 0);
}

void AVL::fixSize(Node* node){
    node->size = getSize(node->left) + getSize(node->right) + 1;
}

//при балансировке размер и высота поддеревьев не менятеся
// правый поворот вокруг node
AVL::Node* AVL::rotateR(Node* node){
    Node* nodeL = node->left;//x за неё подвешиваем
    node->left = nodeL->right;
    nodeL->right = node;

    fixHeight(node);
    fixHeight(nodeL);

    fixSize(node);
    fixSize(nodeL);

    return nodeL;
}

AVL::Node* AVL::rotateL(Node *node){
    Node* nodeR = node->right;
    node->right = nodeR->left;
    nodeR->left = node;

    fixHeight(node);
    fixHeight(nodeR);

    fixSize(node);
    fixSize(nodeR);

    return nodeR;
}
 // балансировка узла p
AVL::Node* AVL::balance(Node* node){
    fixHeight(node);
    fixSize(node);
    if (bFactor(node) == 2){
        if (bFactor(node->right) < 0)
            node->right = rotateR(node->right);
        return rotateL(node);
    }
    if (bFactor(node) == -2 ){
        if (bFactor(node->left) > 0)
            node->left = rotateL(node->left);
        return rotateR(node);
    }
    return node; // балансировка не нужна
}

int AVL::KStatics(int num){
    return kstatics(num + 1, root);//начало с 0, а в дереве с 1
}

int AVL::kstatics(int num, Node *node){
    if (getSize(node->left) + 1 == num) return node->key; // текущий узел удовл условию
    //if (node->left->size == num) return node->left->key;
    if (getSize(node->left) + 1 > num) return kstatics(num, node->left); //искомый слева
    if (getSize(node->left) + 1 < num) return kstatics(num - (getSize(node->left) + 1), node->right);//искомый справа
}

int AVL::GetPos(int Key){

    int pos =  getSize(root->left);//pos of root
    Node * cur_node = root;
    while(cur_node != 0){
        if (cur_node->key == Key){

            return getSize(root) - 1 - pos;
        }
        if (Key > cur_node->key){
            pos += getSize(cur_node->right->left) + 1;//pos of cur_node
            cur_node = cur_node->right;
            //pos += getSize(cur_node->right->left);//pos of cur_node
            continue;
        }
        if (Key < cur_node->key){
            pos -= (getSize(cur_node->left->right) + 1);//pos of cur_node
            cur_node = cur_node->left;

            continue;
        }
    }
    //return cur_node;
}

bool AVL::DeleteByNum(int num){
    Node *node, *par;
    kstaticsNode(num, root, node, par);
    removeNode(node, par);
}

void AVL::kstaticsNode(int num, Node *node, Node*& res, Node*& par){
    if (getSize(node->left) + 1 == num){
        res = node; // текущий узел удовл условию
        return;
    }
    //if (node->left->size == num) return node->left->key;
    if (getSize(node->left) + 1 > num) {
        par = node;
        return kstaticsNode(num, node->left, res, par); //искомый слева
    }
    if (getSize(node->left) + 1 < num){
        par = node;
        return kstaticsNode(num - (getSize(node->left) + 1), node->right, res, par);//искомый справа
    }
}















