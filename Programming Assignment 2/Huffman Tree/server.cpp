// A simple server in the internet domain using TCP
// The port nu1mber is passed as an argument

// Please note this is a C program
// It compiles without warnings with gcc

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <pthread.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <bits/stdc++.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

using namespace std;

/*
Define a struct to represent a node in the Huffman tree.
Each node should have pointers to its left and right child, 
a character (if it is a leaf node), a frequency, and a binary code (if it is not a leaf node).
*/

struct node {
    node *left, *right;
    char ch;
    int freq;
    string code;
};

/*
Implement a function to read the input file and store the alphabet information
in a suitable data structure. You could use a vector of structs, where each struct
stores the character, frequency, and a pointer to its corresponding Huffman tree node.
*/

void readSTDIN(vector<node*> &HNodes, int &nThreads) {
    string line;

    while(getline(std::cin, line))
    {
        node *temp = new node();

        temp->ch = line[0];
        temp->freq = stoi(line.substr(2,1));
        temp->right = temp->left = nullptr;
        temp->code = "";

        HNodes.push_back(temp);

        nThreads++;
    }
}

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


void buildHuffmanTree(vector<node*> &HNodes) {
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

void encode(node* rt, string str, map<string, char> &huffmanCode) {
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


void printHuffmanCodes(node* rt) {    
    if(rt == nullptr)
        return;

    if(!rt->left && !rt->right)
    {
        cout << "Symbol: " << rt->ch << ", Frequency: " << rt->freq
            << ", Code: " << rt->code << endl;
    }
    	
    printHuffmanCodes(rt->left);
    printHuffmanCodes(rt->right);
}

/*
Fireman Function and Main Function
*/

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman); 
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding";
        exit(1);
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Huffman Code
    map<string, char> huffmanCodes;
    vector<node*> HNodes;
    int nThreads = 0;

    readSTDIN(HNodes, nThreads);

    buildHuffmanTree(HNodes);

    encode(HNodes.front(), "", huffmanCodes);

    printHuffmanCodes(HNodes.front());

    cout << endl;

    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            if (newsockfd < 0)
            {
                std::cerr << "ERROR on accept";
                exit(1);
            }
            int size;
            n = read(newsockfd, &size, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            char *buffer = new char[size+1];
            bzero(buffer, size+1);
            n = read(newsockfd, buffer, size);
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }

            std::string s;
            s.push_back(huffmanCodes.find(buffer)->second);

            const int len = s.length();

            char message[len + 1] = {'\0'};

            strcpy(message, s.c_str());

            int sMessage = strlen(message);
            n = write(newsockfd, &sMessage, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, message, sMessage);
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            close(newsockfd);
            delete[] buffer;
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}

// Credit to Dr. Rinon's Socket Practice Example