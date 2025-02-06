// Please note this is a C program
// It compiles without warnings with gcc
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

std::vector<char> message;
/*
Implement a function to read the compressed file and store the information in a
suitable data structure. You could use a vector of structs, where each struct
stores the binary code and a vector of positions.
*/

struct code {
    std::string binaryCode;
    std::vector<int> pos;
    char ch;
};

void readCOMP(code* nodes, int &nThreads, int &count) {
    std::string line;
    int num;

    while(getline(std::cin, line)) 
    {
        std::istringstream iss(line);
        code temp;

        iss >> num;
        temp.binaryCode = std::to_string(num);

        while(iss >> num) 
        {
            temp.pos.push_back(num);
            count++;
        }

        temp.ch = '\0';

        nodes[nThreads] = (temp);

        nThreads++;
    }
}

char createSocket(std::string binCode, int argc, char *argv[]) {
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3)
    {
        std::cerr << "usage " << argv[0] << "hostname port\n";
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        std::cerr << "ERROR opening socket";
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting";
        exit(1);
    }

    const int len = binCode.length();

    char message[len + 1] = {'\0'};

    strcpy(message, binCode.c_str());

    int sMessage = strlen(message);
    n = write(sockfd, &sMessage, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    n = write(sockfd, message, sMessage);
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    int size;
    n = read(sockfd, &size, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    n = read(sockfd, buffer, size);
    std::cout << buffer << std::endl;

    char c = *buffer;

    delete [] buffer;
    close(sockfd);

    return c;
}

/*
Implement a function to decompress the message using POSIX threads.
You would create one thread for each symbol in the alphabet.
Each thread would receive the Huffman tree and the information about the symbol to
decompress from the main thread, traverse the Huffman tree to determine the character
from the binary code, and store the decompressed character (as many times as needed based
on the list of positions) on a memory location accessible by the main thread.
*/

void* decode(void* void_ptr) {
    code* arg_ptr = (struct code *) void_ptr;
        
    while(!arg_ptr->pos.empty())
    {
        message.at(arg_ptr->pos.front()) = arg_ptr->ch;
        arg_ptr->pos.erase(arg_ptr->pos.begin());
    }
        
    return nullptr;
}

/*
The user will execute this program using the following syntax:

./exec_filename hostname port_no < compressed_filename

where exec_filename is the name of your executable file, hostname is the
address where the server program is located, port_no is the port number used
by the server program, and compressed_filename is the name of the compressed file.
The hostname and the port number will be available to the client as command-line
arguments.

After reading the information from STDIN, this program creates m child threads
(where m is the size of the alphabet). Each child thread executes the following tasks: 

    1. Receives the information about the symbol to decompress (binary code and
       list of positions) from the main thread.

    2. Create a socket to communicate with the server program.

    3. Send the binary code to the server program using sockets. 

    4. Wait for the decoded representation of the binary code (character) from the
       server program.

    5. Write the received information into a memory location accessible by the main
       thread.
*/

int main(int argc, char *argv[])
{
    int nThreads = 0;
    int count = 0;

    pthread_t* tid = new pthread_t[nThreads];
    code* nodes = new code[nThreads];

    readCOMP(nodes, nThreads, count);

    for(int j = 0; j < count; j++)
    {
        message.push_back('\0');
    }

    std::cout << "Original message: ";

    for(int i = 0; i < nThreads; i++)
    {
        //std::cout << nodes[i].binaryCode << std::endl;
        // get char
        nodes[i].ch = createSocket(nodes[i].binaryCode, argc, argv);

        //create the thread
        if(pthread_create(&tid[i], NULL, decode, &nodes[i]))
        {
            fprintf(stderr, "Error creating thread\n");
		    return 1;
        }
    }

    // call pthread_join here
    for(int i = 0; i < nThreads; i++)
    {
        pthread_join(tid[i], NULL);
    }
    
    delete[] tid;
    delete[] nodes;
    return 0;
}

// Credit to Dr. Rinon's Socket Practice Example