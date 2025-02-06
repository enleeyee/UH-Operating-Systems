#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
    int pid;
    std::cout << "I am the parent process " << std::endl;
    for(int i = 0; i < 4; i++)
    {
        pid = fork();
        if(pid == 0)
        {
            std::cout << "I am child process " << i << std::endl;
            
            switch(i) {
                case 0:
                        for(int j = 0; j < 1; j++)
                        {
                            pid = fork();
                            if(pid == 0)
                            {
                                std::cout << "I am a grandchild process from child process " << i << std::endl;
                                _exit(0);
                            }
                            wait(nullptr);
                        }
                        break;
                
                case 1:
                        for(int j = 0; j < 2; j++)
                        {
                            pid = fork();
                            if(pid == 0)
                            {
                                std::cout << "I am a grandchild process from child process " << i << std::endl;
                                _exit(0);
                            }
                            wait(nullptr);
                        }
                        break;
                
                case 2:
                        for(int j = 0; j < 3; j++)
                        {
                            pid = fork();
                            if(pid == 0)
                            {
                                std::cout << "I am a grandchild process from child process " << i << std::endl;
                                _exit(0);
                            }
                            wait(nullptr);
                        }
                        break;
                
                case 3:
                        for(int j = 0; j < 4; j++)
                        {
                            pid = fork();
                            if(pid == 0)
                            {
                                std::cout << "I am a grandchild process from child process " << i << std::endl;
                                _exit(0);
                            }
                            wait(nullptr);
                        }
                        break;
                        
            }
            _exit(0);
        }
        wait(nullptr);
    }
    return 0;
}

/*  
            FORK EXAMPLE CODE WE WROTE DURING THE PREVIOUS LECTURE
        YOU CAN USE THIS CODE TO WRITE YOUR SOLUTION FOR THIS QUESTION

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int pid;
  std::cout << "I am the parent process " << std::endl;
  for (int i = 0; i < 3; i++)
  {
    pid = fork();
    if (pid == 0)
    {
      std::cout << "I am child process " << i << std::endl;
      if (i == 1)
      {

        pid = fork();
        if (pid == 0)
        {
          std::cout << "I am a grandchild process from child process " << i << std::endl;
          _exit(0);
        }
        wait(nullptr);
      }
      _exit(0);
    }
    wait(nullptr);
  }
  //  for (int i = 0; i < 3; i++)
  //      wait(nullptr);
  return 0;
}


*/