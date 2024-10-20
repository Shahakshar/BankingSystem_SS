#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "manager.h"

struct userCred temporary1;
void addCustomerByEmployee(int clientSocket);
void modifyCustomerByEmployee(int clientSocket);
void processLoanApplication(int clientSocket);
void approveOrRejectLoan(int clientSocket);
void viewAssignedLoans(int clientSocket);
void viewCustomerTransaction(int clientSocket);
void viewCustomerTransaction1(int clientSocket);
void changePasswordByEmployee(int clientSocket);
bool employee_authenticate(int clientSocket){

struct employee mng, temp;
int openFD = open("database/employee_database.txt", O_RDWR, 0744); // Open in read-only mode
	int recordoff=0;
    if (openFD == -1) {
        perror("Error opening file");
        return false;
    }
    bool found = false; // Initialize found to false
    char buffer[1024]; // Declare buffer for sending data
    memset(&buffer, '\0', sizeof(buffer));
    
    // taking loginId and password
    send(clientSocket, "Enter Employee ID: ", strlen("Enter Employee ID: "), 0);
    int readResult = read(clientSocket, mng.loginId, sizeof(mng.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error", strlen("Error"), 0);
        return false;
    }
    mng.loginId[readResult] = '\0';
    
    
    
    send(clientSocket, "Password\n", strlen("Password\n"), 0);
    readResult = read(clientSocket, mng.password, sizeof(mng.password) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error", strlen("Error"), 0);
        return false;
    }
    mng.password[readResult] = '\0';
        
   
   
    

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct employee);

        if(  strcmp(temp.position,"mng") == 0 ) continue;
        //This checks whther my login id is matching or not
        if (   strcmp(mng.loginId, temp.loginId) == 0) 
        {
        
        
         if (   strcmp(mng.password, temp.password) != 0)
          {
         	 found=true;// vague logic to not to stucj after break
         	 close(openFD); 
             send(clientSocket, "Login Id was correct\nPassword error\nTry Logging Again\n\n", strlen("Login Id was correct\nPassword error\nTry Logging Again\n\n"),0);
             break; 
         
         }
        // if the login id matched but the password is not matched, then
         found = true;
       
        
           
             if (found) 
             {
               memset(&temporary1,'\0',sizeof(temporary1));
                     
    
   	   strcpy(temporary1.loginId, temp.loginId);
   	   strcpy(temporary1.password, temp.password);
   	   close(openFD);
   	   return true;
   	       
    	  
             }

            break;
        }

        // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }

        recordoff += sizeof(struct customer);
    }

    if(found==false) {
        send(clientSocket, "Account not found\n", strlen("Account not found\n"),0);
    }

    close(openFD); // Close the file after use
    return false;
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
                     " 8. Change Password of Customer\n"
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
            case 1:
            //printf("Insize case 1\n");
            addCustomer(clientSocket);
            //struct customer cust;
                 
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
          	//printf("activated\n");
          	
          break;
          case 6:
          	viewCustomerTransaction1(clientSocket);
          	//printf("activated\n");
          	
          break;
          case 7:
          	changePasswordByEmployee(clientSocket);
          	//printf("activated\n");
          	
          break;
          
          case 8:
          	changePasswordCustomer(clientSocket);
          	
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
   modifyCustomer(clientSocket);
  //printf("Modify customer\n");

};























void processLoanApplication(int clientSocket){

    
    
    int readResult=0;
    ssize_t bytes_written=-1;
    struct loan emp, temp;
int openFD = open("database/loan_database.txt", O_RDWR, 0744); // Open in read-only mode
int recordoff=0;
    if (openFD == -1) {
        perror("Error opening file");
        return ;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(&buffer, '\0', sizeof(buffer));
    
    
    
    
/* 
The below is taking login id and password from the client
*/    

    strcpy(emp.empId, temporary1.loginId);
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct loan);
        
        // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }

        
        //This checks whther my login id is matching or not
        if (   strcmp(emp.empId, temp.empId) == 0) 
        {
        
         // if the login id matched but the password is not matched, then
         found = true;
         
        
           
 if (found) 
    {
       memset(&buffer, '\0', sizeof(buffer));
       snprintf(buffer,sizeof(buffer), "\n***Record***\nUser Id: %s\nDescription:%s\nProcess whether the description is Valid or not and assign the process(o- not valid and verified, y-valid and verified, n-not processed\n)",temp.userId,temp.description_to_grant_loan);
   send(clientSocket,buffer,sizeof(buffer),0); 
   readResult = read(clientSocket, temp.processed, sizeof(temp.processed) - 1);

    if (readResult <= 0) {
  send(clientSocket, "Error in Process LA\n", strlen("Error in Process LA\n"), 0);
        return ;
    }
    temp.processed[readResult] = '\0';
        
                         
    // now it is time to change the records of the database
    
    lseek(openFD, -sizeof(temp), SEEK_CUR); 
    bytes_written = write(openFD, &temp, sizeof(temp));
	    

	    if (bytes_written == -1) {
	        perror("write");
	        close(openFD);
	        close(clientSocket);
	        return ;
     
      
             }

        
        }

        

        

        
    }
    
    // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }
    recordoff += sizeof(struct loan);
}
    if(found==false) {
        send(clientSocket, "Loan Application is Not Assigned\nPress Enter to move on\n", strlen("Loan Application is Not Assigned\nPress Enter to move on\n"),0);
    }

    close(openFD); // Close the file after use
    return; 
    
   


};



















void approveOrRejectLoan(int clientSocket){
  
  /*
  1. Take the records which are asigned to this employee.
  2. Check with the status and processed. 
  3. If the status is pending and processed is o then make the status to rej
  4. If the status is pending and processed is y then make the status to acc
  5. If the status is rejected and read_by_manager is yes, then remove it, make the whole content to \0, or just 0. 
  6. The accept and reject to be asked with the in while loop, then do it for every record.  
  */
  
 
      
    int readResult=0;
    ssize_t bytes_written=-1;
    struct loan emp, temp;
int openFD = open("database/loan_database.txt", O_RDWR, 0744); // Open in read-only mode
int recordoff=0;
    if (openFD == -1) {
        perror("Error opening file");
        return ;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(buffer, '\0', sizeof(buffer));
    
    
    
    
/* 
The below is taking login id and password from the client
*/    

    strcpy(emp.empId, temporary1.loginId);
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct loan);

        
        //This checks whther my login id is matching or not
   if (strcmp(emp.empId, temp.empId) == 0) 
       {
         if(strcmp(temp.processed,"n") == 0) continue;
         // if the login id matched but the password is not matched, then
         found = true;
         
        
           
 if (found) 
    {
       memset(buffer, '\0', sizeof(buffer));
       snprintf(buffer,sizeof(buffer), "\n***Record***\nUser Id: %s\nDescription:%s\nTell whether to accept(acc) or reject(rej)\n",temp.userId,temp.description_to_grant_loan);
   send(clientSocket,buffer,sizeof(buffer),0); 
   readResult = read(clientSocket, temp.status, sizeof(temp.status) - 1);

    if (readResult <= 0) {
  send(clientSocket, "Error in Process LA\n", strlen("Error in Process LA\n"), 0);
        return ;
    }
    temp.status[readResult] = '\0';
        
                         
    // now it is time to change the records of the database
    
    lseek(openFD, -sizeof(temp), SEEK_CUR); 
    bytes_written = write(openFD, &temp, sizeof(temp));
	    

	    if (bytes_written == -1) {
	        perror("write");
	        close(openFD);
	        close(clientSocket);
	        return ;
     
      
             }

        
        }

        // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }

        
    }
    recordoff += sizeof(struct loan);
}
    if(found==false) {
        send(clientSocket, "Loan Application is Not Assigned or not processed still\nPress Enter to move on\n", strlen("Loan Application is Not Assigned or not processed still\nPress Enter to move on\n"),0);
    }

    close(openFD); // Close the file after use
    return; 
    

};

























void viewAssignedLoans(int clientSocket){

  
   struct loan  temp;
    int openFD = open("database/loan_database.txt", O_RDONLY, 0744); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }
    
   
    
    //bool found = false; // Initialize found to false
    

    char buffer[1024]=""; // Declare buffer for sending data
    memset(buffer,'\0',sizeof(buffer));
    
    
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    
    struct flock mylock;
        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
    
       fcntl(openFD, F_SETLKW, &mylock);
    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0) {
    
        if(strcmp(temp.empId,temporary1.loginId)==0){
    	
    	char tempbuf[1024]="";
    	memset(tempbuf,'\0',sizeof(tempbuf));
    	
    		
        sprintf(tempbuf, "\nUser Id :%s\nStatus of Loan: %s\nProcessed?(y-yes, n-no, o- not valid): %s\n\n",
                temp.userId, temp.status, temp.processed);
               
                
                strcat(buffer,tempbuf);
                }
        }
        
    strcat(buffer,"\nPress Enter to move on\n");    
    send(clientSocket,buffer,sizeof(buffer),0);
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);
    close(openFD); // Close the file after use
    
    
};




















void viewCustomerTransaction(int clientSocket){
   //printf("view customer trans\n");
   
   /*
   Get the customer id/ account id, then start to print the things

   */
   struct  transaction my_trans, temp;
    int openFD = open("database/transaction_database.txt", O_RDWR, 0744); // Open in read-only mode
    int recordoff = 0;
    if (openFD == -1) {
        perror("Error opening file");
        return;
    }
    
   send(clientSocket, "Enter customer ID: ", strlen("Enter customer ID: "), 0);
    int readResult = read(clientSocket, my_trans.from_customer_id, sizeof(my_trans.from_customer_id) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Admin ID from server", strlen("Error receiving Admin ID from server"), 0);
        return;
    }
    my_trans.from_customer_id[readResult] = '\0';
    
    
    //bool found = false; // Initialize found to false
    

    char buffer[1024]=""; // Declare buffer for sending data
    memset(buffer,'\0',sizeof(buffer));
    
    
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    
    struct flock mylock;
        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
    
       fcntl(openFD, F_SETLKW, &mylock);
    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0) {
    
        if(strcmp(temp.from_customer_id,my_trans.from_customer_id)==0){
    	char tempbuf[1024]="";
    	memset(tempbuf,'\0',sizeof(tempbuf));
    	
    	
    		
        sprintf(tempbuf, "\nTransaction ID: %s\nFrom Account number: %s\n To ccount Number%s\ndate_time: %s\nTransaction Amount: %s\nDecsription To Transaction: %s\nBalance: %s\n",
                temp.transaction_id, temp.from_account_no, temp.to_account_no, temp.date_time, temp.transaction_amount, temp.description_to_transaction, temp.balance_from_account_after_transaction);
                
                
                strcat(buffer,tempbuf);
                }
        }
        
    strcat(buffer,"Press Enter to move on\n");    
    send(clientSocket,buffer,sizeof(buffer),0);
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);
    close(openFD); // Close the file after use
    
    
   
   
};






void viewCustomerTransaction1(int clientSocket) {
    struct transaction my_trans, temp;
    int openFD = open("database/transaction_database.txt", O_RDWR, 0744); // Open in read-only mode
    int recordoff = 0;
    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    // Prompt for customer ID
    send(clientSocket, "Enter customer ID: ", strlen("Enter customer ID: "), 0);
    int readResult = read(clientSocket, my_trans.from_customer_id, sizeof(my_trans.from_customer_id) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Customer ID from server", strlen("Error receiving Customer ID from server"), 0);
        close(openFD);
        return;
    }
    my_trans.from_customer_id[readResult] = '\0';

    char buffer[4096] = ""; // Increase buffer size for large data
    memset(buffer, '\0', sizeof(buffer));

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);

    struct flock mylock;
    mylock.l_type = F_RDLCK;
    mylock.l_whence = SEEK_SET;
    mylock.l_start = 0;
    mylock.l_len = 0;
    mylock.l_pid = getpid();

    // Lock the file
    fcntl(openFD, F_SETLKW, &mylock);

    // Add table headers to the buffer
    snprintf(buffer, sizeof(buffer), "%-15s %-20s %-20s %-20s %-15s %-30s %-15s\n",
             "Transaction ID", "From Account", "To Account", "Date Time", "Amount", "Description", "Balance");
    strcat(buffer, "-----------------------------------------------------------------------------------------------------------\n");

    // Loop to search for the transaction in the file
    bool found = false;
    while (read(openFD, &temp, sizeof(temp)) > 0) {
        if (strcmp(temp.from_customer_id, my_trans.from_customer_id) == 0) {
            found = true;
            char tempbuf[1024] = "";
            memset(tempbuf, '\0', sizeof(tempbuf));

            // Format the output for tabular representation
            snprintf(tempbuf, sizeof(tempbuf), "%-15s %-20s %-20s %-20s %-15s %-30s %-15s\n",
                     temp.transaction_id, temp.from_account_no, temp.to_account_no, 
                     temp.date_time, temp.transaction_amount, 
                     temp.description_to_transaction, temp.balance_from_account_after_transaction);

            strcat(buffer, tempbuf);
        }
    }

    // Append message if no transactions were found
    if (!found) {
        strcat(buffer, "No transactions found for the given customer ID.\n");
    }

    strcat(buffer, "Press Enter to move on\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    // Unlock the file
    mylock.l_type = F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);
    close(openFD); // Close the file after use
}




























void changePasswordByEmployee(int clientSocket){


    struct employee emp,temp;
    memset(emp.loginId,'\0', sizeof(emp.loginId));
    memset(emp.password,'\0', sizeof(emp.password));
    
    strcpy(emp.loginId, temporary1.loginId);
    send(clientSocket, "Passowrd\n", strlen("Passowrd\n"), 0);
     int readResult = read(clientSocket, emp.password, sizeof(emp.password) - 1);

    if (readResult <= 0) 
    {
        send(clientSocket, "Error receiving Password from server\n", strlen("Error receiving Password from server\n"), 0);
        return;
    }
    emp.password[readResult] = '\0';
    // now the data is ready, but we shall change it. 
    
    
    int openFD = open("database/employee_database.txt", O_RDWR, 0744); // Open in read-only mode
    int recordoff=0;
    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false
    char buffer[1024]; // Declare buffer for sending data
    memset(buffer, '\0', sizeof(buffer));
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
    
    
    
    while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct employee);

        if (   strcmp(temp.position, "mng") == 0) continue;
        //This checks whther my login id is matching or not
        if (   strcmp(emp.loginId, temp.loginId) == 0) 
        {
        
         // if the login id matched but the password is not matched, then
         found = true;
       
        
           
             if (found) 
             {
               // then change the changed password to be written at this place
                strcpy(temp.password, emp.password);
                lseek(openFD, -sizeof(emp), SEEK_CUR);
	    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
	    

	    if (bytes_written == -1) {
	        perror("write");
	        close(openFD);
	        close(clientSocket);
	        return ;
	    }
		   
    	  
             }

            break;
        }

        // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }

        recordoff += sizeof(struct employee);
    }

    if(found==true) {
        send(clientSocket, "Password Changed Successfully\n", strlen("Password Changed Successfully\n"),0);
    }
    

    close(openFD); // Close the file after use
    return;
       
}



























