#include <iostream>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <iterator>
#include <bits/stdc++.h>
#include <map>


/*
Define a struct to represent a node in the Huffman tree.
Each node should have pointers to its left and right child, 
a character (if it is a leaf node), a frequency, and a binary code (if it is not a leaf node).
*/

struct node {
    node *left, *right;
    char ch;
    int freq;
    std::string code;
};

struct inputCode {
    std::string binCode;
    std::vector<std::string> pos;
    node *root;
    char character;
};

static pthread_mutex_t bsem;
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
static int turn;
static inputCode *arr;
static std::map<int, char> messageMAP;
// static std::vector<node *> filledNodes;

/*
Implement a function to sort the alphabet information based on the frequencies
and ASCII values, as described in the problem statement. You could use the sort
function from the algorithm library.
*/

bool comp(node* l, node* r) {

    if(l->freq == r->freq)
    {
        return l->ch < r->ch;
    }

	return l->freq < r->freq;
}

bool mapCOMP(std::pair<int, char>& a, std::pair<int, char>& b) {
    return a.first < b.first;
}

/*
Implement a function to generate the Huffman tree based on the sorted alphabet information.
You could use a priority queue of Huffman tree nodes, where the nodes are sorted based on
their frequencies. In each iteration, you would dequeue the two nodes with the lowest
frequency, create a new node as their parent, and enqueue it back into the priority queue.
Repeat this process until there is only one node left in the priority queue, which represents
the root of the Huffman tree.
*/

node* getNode(char c, int f, std::string s, node *l, node *r) {
    node* temp = new node();
    
    temp->ch = c;
    temp->freq = f;
    temp->code = s;
    temp->left = l;
    temp->right = r;
    
    return temp;
}


void buildHuffmanTree(std::vector<node*> &HNodes) {
    while(HNodes.size() != 1) {
        sort(HNodes.begin(), HNodes.end(), comp);

        node *l = HNodes.front();
        HNodes.erase(HNodes.begin());
        node *r = HNodes.front();
        HNodes.erase(HNodes.begin());

        int sum = l->freq + r->freq;

        HNodes.push_back(getNode('\0', sum, "", l, r));
    }
}

/*
Implement a function to print the Huffman codes from the Huffman tree, as described in the
problem statement. You could traverse the Huffman tree using a depth-first search algorithm
and store the binary codes in a suitable data structure. Then, you could iterate over the
alphabet information and print the code for each symbol.
*/

void encode(node* rt, std::string str, std::map<std::string, char> &huffmanCode) {
    	if (rt == nullptr)
    		return;
    
        // found a huffman leaf node
    	if (rt->left == nullptr && rt->right == nullptr) {
            huffmanCode[str] = rt->ch;
            rt->code = str;
        }
        
        // traverse the tree
    	encode(rt->left, str + "0", huffmanCode);
        encode(rt->right, str + "1", huffmanCode);
}

void printHuffmanCodes(node* rt, std::string c, inputCode &input) {    
    if(rt == nullptr)
        return;

    if((!rt->left && !rt->right) && (rt->code == c))
    {
        std::cout << "Symbol: " << rt->ch << ", Frequency: " << rt->freq
            << ", Code: " << rt->code << std::endl;

        input.character = rt->ch;
    }
    	
    printHuffmanCodes(rt->left, c, input);
    printHuffmanCodes(rt->right, c, input);
}

void traverseTree(node *rt, std::vector<node *>& filledNodes) {
    if(rt == nullptr)
        return;

    if((!rt->left && !rt->right)) {
        filledNodes.push_back(rt);
    }

    traverseTree(rt->left, filledNodes);
    traverseTree(rt->right, filledNodes);
}

/*
Implement a function to decompress the message using POSIX threads.
You would create one thread for each symbol in the alphabet.
Each thread would receive the Huffman tree and the information about the symbol to
decompress from the main thread, traverse the Huffman tree to determine the character
from the binary code, and store the decompressed character (as many times as needed based
on the list of positions) on a memory location accessible by the main thread.
*/

void createMessage(inputCode *input) {
    node *tree = input->root;
    std::vector<std::string> positions = input->pos;
    std::string findCode = input->binCode;
    char letter = input->character;

    int position;

    for(int i = 0; i < (positions).size(); i++) 
    {
        std::stringstream s(positions[i]);
        while(s >> position) {
            messageMAP.insert( {position, input->character} );
        }
    }
}

void *printHuffman(void *void_ptr_argv) {
    std::string myTurn = *(std::string *) void_ptr_argv;

    pthread_mutex_lock(&bsem);

    while(myTurn != arr[turn].binCode) {
        pthread_cond_wait(&waitTurn, &bsem);
    }

    pthread_mutex_unlock(&bsem);

    pthread_mutex_lock(&bsem);

    printHuffmanCodes(arr[turn].root, arr[turn].binCode, arr[turn]);

    createMessage(&arr[turn]);

    turn++;

    pthread_cond_broadcast(&waitTurn);

    pthread_mutex_unlock(&bsem);

    return nullptr;
}


void sortingMap()
{
    std::vector<std::pair<int, char> > temp;
 
    // Copy key-value pair from Map
    // to vector of pairs
    for (auto& it : messageMAP) {
        temp.push_back(it);
    }
 
    // Sort using comparator function
    sort(temp.begin(), temp.end(), mapCOMP);
 
    // Print the sorted value
    for (auto& it : temp) {
        std::cout << it.second;
    }
}


/*
Main Function
*/

int main() {

    // initialize variables for the tree
    std::map<std::string, char> huffmanCodes;
    std::vector<node*> HNodes;
    int nThreads, num;
    int count = 0;
    std::string line;
    
    // get info for the Huffman Tree
    std::cin >> nThreads;
    std::cin.ignore();

    pthread_mutex_init(&bsem, NULL);
    pthread_t *tid = new pthread_t[nThreads];
    std::string *threadLetter = new std::string[nThreads];
    turn = 0;
    arr = new inputCode[nThreads];

    for(int i = 0; i < nThreads; i++)
    {
        getline(std::cin, line);

        node *temp = new node();

        temp->ch = line[0];
        temp->freq = stoi(line.substr(2,1));
        temp->right = temp->left = nullptr;
        temp->code = "";

        HNodes.push_back(temp);
    }

    for(int i = 0; i < nThreads; i++) 
    {
        getline(std::cin, line);

        std::istringstream iss(line);

        iss >> num;
        arr[i].binCode = std::to_string(num);

        while(iss >> num) 
        {
            arr[i].pos.push_back(std::to_string(num));
            count++;
        }

    }

    buildHuffmanTree(HNodes);

    encode(HNodes.front(), "", huffmanCodes);

    for(int i = 0; i < nThreads; i++) {

        arr[i].root = HNodes.front();
        threadLetter[i] = arr[i].binCode;
        if(pthread_create(&tid[i], nullptr, printHuffman, &threadLetter[i])) {
            std::cout << "Failed to create thread" << std::endl;
            return 1;
        }
    }

    for(int i = 0; i < nThreads; i++) {
        pthread_join(tid[i], NULL);
    }

    std::cout << "Original message: ";

    sortingMap();

    std::cout << std::endl;
    
    delete[] threadLetter;
    delete[] tid;
    return 0;
}