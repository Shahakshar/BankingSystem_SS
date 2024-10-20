//#include "../database/structure.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "admin.h"

void activateAccountByManager(int clientSocket);
void deactivateAccountByManager(int clientSocket);
void loanApplicationHandle(int clientSocket);
void reviewCustomerFeedback(int clientSocket);
void changePasswordManager(int clientSocket);
void changePasswordEmployee(int clientSocket);
void changePasswordCustomer(int clientSocket);


bool manager_authenticate(int clientSocket){
return true;
}

bool managerHandle(int clientSocket) {
    if (manager_authenticate(clientSocket)) {
    char readbuff[1024];
        send(clientSocket, "Logged in Successfully\n", strlen("Logged in Successfully\n"), 0);
       while(true){
      char managerPrompt[] = "\t\tWelcome to Manager Menu\n"
                     "---------------------------------\n"
                     " 1. Activate Customer Account\n"
                     " 2. Deactivate Customer Account\n"
                     " 3. Assign Loan Application Processes to Employee\n"
                     " 4. Review Customer Feedback\n"
                     " 5. Change Manager Password\n"
                     " 6. Change Employee Password\n"
                     " 7. Change Customer Passowrd\n"
                     " 8. Logout and Exit\n"
                     "---------------------------------\n"
                     "Please select an option: ";
                     
        write(clientSocket, managerPrompt, strlen(managerPrompt));

        // readBytes stores the number of bytes read from the client by the server
        ssize_t readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
        if (readBytes == -1) {
            perror("Error in the choice you provided");
            return false;
        }
        
        int choice = atoi(readbuff);
         //send(clientSocket, readbuff, sizeof(readbuff), 0);

        switch (choice) {
            case 1:activateAccountByManager(clientSocket);
                 ;
                break;
             case 2:   
                deactivateAccountByManager(clientSocket);
                break;
          case 3:
          	loanApplicationHandle(clientSocket);
          break;
          case 4:
          	 reviewCustomerFeedback(clientSocket);
          break;
          case 5:
          	changePasswordManager(clientSocket);
          	//printf("activated\n");
          	
          break;
          case 6:
          	changePasswordEmployee(clientSocket);
          	//printf("activated\n");
          	
          break;
          case 7:
          	changePasswordCustomer(clientSocket);
          	//printf("activated\n");
            break;
          
           default:
            return true; 
                break;
        }
      }
    }
    return false;
}




void activateAccountByManager(int clientSocket){//
    activateAccount(clientSocket);
};
void deactivateAccountByManager(int clientSocket){//
    deactivateAccount(clientSocket);
};
void loanApplicationHandle(int clientSocket){//
 
};
void reviewCustomerFeedback(int clientSocket){//
};
void changePasswordManager(int clientSocket){//
};
void changePasswordEmployee(int clientSocket){//
};
void changePasswordCustomer(int clientSocket){//
};
