#include <iostream>                    // for cout
#include <unistd.h>                    // for fork()
#include <sys/types.h>                 // for pid_t type

using namespace std;

bool trow_the_coin(){         // trow_the_coin function
    if (rand()%2+1 == 2)
        return true;          //will increment the counter
    return false;
}

int main(){

    srand(time(NULL));      // restarts the seeds
    int i = 0, j = 0, cp = 0, cc = 0;   // counter, counter for parent and counter for child
    int readmessage, writemessage; //messages in/out pipes
    int pipeparent[2], pipechild[2];
    int statuspipeparent, statuspipechild;
    statuspipeparent = pipe(pipeparent);
    statuspipechild = pipe(pipechild);
    if (statuspipeparent == -1 || statuspipechild == -1) {
        cout << "Unable to create pipes" << endl;
        return 1;
    }

    writemessage = i;
    write(pipechild[1], &writemessage, sizeof(writemessage));


    //Starts one child process and continue the main
    pid_t pid1 = fork();


    //Trow the coin until shared memory counter reach 1000
    while (i < 999 ) {
        if (pid1 > 0) {          // parent process
            cp++;   // increment the counter for parent process
            if(trow_the_coin()){
                    close(pipeparent[0]); // Close parentpipe for read
                    close(pipechild[1]); // Close childpipe for write
                    read(pipechild[0], &readmessage, sizeof(readmessage));
                    i = readmessage;
                    i++;
                    writemessage = i;
                    write(pipeparent[1], &writemessage, sizeof(writemessage));
                    cout << "Parent process counter = " << cp << ", Pipe = " << i << endl;
            }
        } else {            // child process
            cc++;   // increment the counter for child process
            if(trow_the_coin()){
                    close(pipeparent[1]); // Close pipeparent for write
                    close(pipechild[0]); // Close pipechild for read
                    read(pipeparent[0], &readmessage, sizeof(readmessage));
                    i = readmessage;
                    i++;
                    writemessage = i;
                    write(pipechild[1], &writemessage, sizeof(writemessage));
                    cout << "Child process counter = " << cc << ", Pipe = " << i << endl;
            }
        }
    }


    return 0;
}
