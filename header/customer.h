#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

void viewAccountBalance(int clientSocket);
void depositMoney(int clientSocket);
void withdrawMoney(int clientSocket);
void transferFunds(int clientSocket);
void applyForLoan(int clientSocket);
void changePassword(int clientSocket);
void addFeedback(int clientSocket);
void viewTransactionHistory(int clientSocket);


bool customer_authenticate(int clientSocket){
  printf("auththentication of customer is done\n");
  return true;
}

bool customerHandle(int clientSocket) {
    if (customer_authenticate(clientSocket)) {
    char readbuff[1024];
        send(clientSocket, "Logged in Successfully\n", strlen("Logged in Successfully\n"), 0);
       while(true){
      char managerPrompt[] = "\t\tWelcome to Customer Menu\n"
                     "---------------------------------\n"
                     " 1. View Account Balance\n"
                     " 2. Deposit Money\n"
                     " 3. Withdraw Money\n"
                     " 4. Transfer Funds\n"
                     " 5. Apply for a Loan\n"
                     " 6. Change Password\n"
                     " 7. Adding Feedback\n"
                     " 8. View Transaction History\n"
                     " 9. Logout and Exit\n"
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
            case 1:viewAccountBalance(clientSocket);
                 ;
                break;
             case 2:   
                depositMoney(clientSocket);
                break;
          case 3:
          	withdrawMoney(clientSocket);
          break;
          case 4:
          	 transferFunds(clientSocket);
          break;
          case 5:
          	applyForLoan(clientSocket);
          	//printf("activated\n");
          	
          break;
          case 6:
          	changePassword(clientSocket);
          	//printf("activated\n");
          	
          break;
          case 7:
          	addFeedback(clientSocket);
          	//printf("activated\n");
            break;
            
          case 8:
          	viewTransactionHistory(clientSocket);
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












void viewAccountBalance(int clientSocket){

   printf("Hi you are in view balance\n");

};


void depositMoney(int clientSocket){

  printf("Hi you are depositing money\n");

};

void withdrawMoney(int clientSocket){
 
  printf("Hi you are Withdraw money\n");
  
};
void transferFunds(int clientSocket){

 printf("Hi you are Transfer Funds\n");

};

void applyForLoan(int clientSocket){

 printf("Hi you are Apply For Loans\n");

};
void changePassword(int clientSocket){

 printf("Hi you are change password \n");
 
};
void addFeedback(int clientSocket){

  printf("Hi you are add Fedback\n");
};
void viewTransactionHistory(int clientSocket){

  printf("Hi you are Transaction History\n");
  
};



