#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "manager.h"

void addCustomerByEmployee(int clientSocket);
void modifyCustomerByEmployee(int clientSocket);
void processLoanApplication(int clientSocket);
void approveOrRejectLoan(int clientSocket);
void viewAssignedLoans(int clientSocket);
void viewCustomerTransaction(int clientSocket);
void changePasswordByEmployee(int clientSocket);


bool employee_authenticate(int clientSocket){
   
   printf("Customer Authenticate\n");
   return true;
}

bool employeeHandle(int clientSocket) {
    if (employee_authenticate(clientSocket)) {
    char readbuff[1024];
        send(clientSocket, "Logged in Successfully\n", strlen("Logged in Successfully\n"), 0);
       while(true){
      char managerPrompt[] = "\t\tWelcome to Employee Menu\n"
                     "---------------------------------\n"
                     " 1. Add Customer\n"
                     " 2. Modify Customer\n"
                     " 3. Process Loan Application\n"
                     " 4. Approve or Reject\n"
                     " 5. View Assigned Loans\n"
                     " 6. View Customer Transaction\n"
                     " 7. Change Password\n"
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
          case 1:
	        addCustomer(clientSocket);
                break;
          case 2:   
                modifyCustomerByEmployee(clientSocket);
                break;
          case 3:
          	processLoanApplication(clientSocket);
	        break;
          case 4:
          	 approveOrRejectLoan(clientSocket);
	         break;
          case 5:
          	viewAssignedLoans(clientSocket);	
          	break;
          case 6:
          	viewCustomerTransaction(clientSocket);
          	break;
          case 7:
          	changePasswordByEmployee(clientSocket);	
	        break;
       
           default:
            return true; 
                break;
        }
      }
    }
    return false;
}




void addCustomerByEmployee(int clientSocket){
  //printf("You are in add customer\n");
  addCustomer(clientSocket);

   
};
void modifyCustomerByEmployee(int clientSocket){
   modifyEmployee(clientSocket);
  //printf("Modify customer\n");

};

void processLoanApplication(int clientSocket){

   printf("Process Loan application\n");
   
};
void approveOrRejectLoan(int clientSocket){
  
  printf("Approve or reject loan\n");

};
void viewAssignedLoans(int clientSocket){

   printf("Assigned Loans\n");
};

void viewCustomerTransaction(int clientSocket){
   printf("view customer trans\n");
};
void changePasswordByEmployee(int clientSocket){

   printf("change password\n");
};


