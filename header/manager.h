//#include "../database/structure.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "customer.h"
struct userCred temporary2;


void activateAccountByManager(int clientSocket);//done
void deactivateAccountByManager(int clientSocket);//done
void loanApplicationHandle(int clientSocket);
void reviewCustomerFeedback(int clientSocket);
void changePasswordManager(int clientSocket);
void changePasswordEmployee(int clientSocket);
void changePasswordCustomer(int clientSocket);
bool manager_authenticate(int clientSocket){


struct employee mng, temp;
int openFD = open("database/employee_database.txt", O_RDWR, 0774); // Open in read-only mode
int recordoff=0;

    if (openFD == -1) {
        perror("Error opening file");
        return false;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(buffer, '\0', sizeof(buffer));
    
    
    
    
/* 
The below is taking login id and password from the client
*/    

    send(clientSocket, "Enter Manager ID: ", strlen("Enter Manager ID: "), 0);
    int readResult = read(clientSocket, mng.loginId, sizeof(mng.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Admin ID from server", strlen("Error receiving Admin ID from server"), 0);
        return false;
    }
    mng.loginId[readResult] = '\0';
    
    
    
    
    send(clientSocket, "Password\n", strlen("Password\n"), 0);
     readResult = read(clientSocket, mng.password, sizeof(mng.password) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Admin Password from server", strlen("Error receiving Admin Password from server"), 0);
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

        if(  strcmp(temp.position,"mng") != 0 ) continue;
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
               memset(temporary2.loginId,'\0',sizeof(temporary2.loginId));
               memset(temporary2.password,'\0',sizeof(temporary2.password));        
    
   	   strcpy(temporary2.loginId, temp.loginId);
   	   strcpy(temporary2.password, temp.password);
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













void calling_to_make_applied_for_loan_to_y_on_employee(int clientSocket,char *s){


    
  struct employee c1,temp;
  memset(&c1,'\0',sizeof(c1));
  memset(&temp,'\0', sizeof(temp));
  
  int recordoff = 0, readResult=-1;
  strcpy(c1.loginId, s);
  int openFD = open("database/employee_database.txt", O_RDWR, 0774); // Open the file in append mode

    if (openFD == -1) {
        perror("open");
        send(clientSocket, "Database is not exist\n", sizeof("Database is not exist\n"),0);
        close(clientSocket);
        close(openFD);
        return;
    }
    
    
    
    struct flock mylock;
    while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct employee);
         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        
        //This checks whther my login id is matching or not
        
    if (strcmp(c1.loginId, temp.loginId) == 0) 
       {            
         strcpy(temp.assigned_for_loan,"y");  
    lseek(openFD, -sizeof(struct employee), SEEK_CUR);  
    
    
    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
    if (bytes_written == -1) {
        perror("write");
        send(clientSocket,"Applied loan attribute err\n", strlen("Applied loan attribute err\n"),0);
        close(openFD);
        close(clientSocket);
        return ;
         }
    
         
        }
        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }
       
    
    recordoff += sizeof(struct employee);
}// end of while loop  

close(openFD);

}








// assigns the loa application to the employee
void loanApplicationHandle(int clientSocket){ //
//printf("Loan application \n");
/*
Steps: 
1. Get the name and employee id of eveyone in the employee except manager.
2. The store it in the buffer as it is shown in view employee details of the admin
3. Get the emp id from the client side. 
*/

struct employee my_emp, temp;
    int openFD = open("database/employee_database.txt", O_RDWR, 0774); // Open in read-only mode

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
    	char tempbuf[1024]="";
    	memset(tempbuf,'\0',sizeof(tempbuf));
    	
    
    	
        if(strcmp(temp.position,"mng")==0) continue;  	
    		
        sprintf(tempbuf, "\nName: %s\nLogin ID: %s\n\n",
                temp.name, temp.loginId);
                
                
                strcat(buffer,tempbuf);
        }
     
    strcat(buffer,"Press Enter to move on\n");    
    send(clientSocket,buffer,sizeof(buffer),0);
    
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);
    close(openFD); // Close the file after use
    //printf("printed all the employee list\n");
    
    
    
    
    // step2: Now the information regarding the employees are already given, it is now time to assign the employee employee for the loan
      
    
    
    
    //printf("loan database\n");
    struct loan temp1;
    struct userCred t;
    openFD = open("database/loan_database.txt", O_RDWR, 0774);

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    //bool found = false;

    

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    //struct flock mylock;
        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
    
       fcntl(openFD, F_SETLKW, &mylock);

    while (read(openFD, &temp1, sizeof(temp1)) > 0) 
    
 {
    
    //this will check whether there is any loan application or not
    // When there is any loan application pending, just print and get the id from the client, and assign that to the temp.empId
    
    if(strcmp(temp1.empId,"null")==0){
    
    memset(buffer,'\0',sizeof(buffer));
    snprintf(buffer, sizeof(buffer), 
             "User Id: %s\nDescription: %s\n"
             "Assign employee (Be careful, check twice before pressing enter, else you will give the wrong employee)\n\n"
             "Employee ID: ", 
             temp1.userId, temp1.description_to_grant_loan);
             
               
    send(clientSocket,buffer,sizeof(buffer),0);
    memset(&t,'\0',sizeof(t));
     int readResult = read(clientSocket, t.loginId, sizeof(t.loginId) - 1);
     

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Employee ID from server", strlen("Error receiving Employee ID from server"), 0);
        return;
    }
    t.loginId[readResult] = '\0';
    //printf("Employee Id: %s\n",t.loginId);
    
    
    strcpy(temp1.empId,t.loginId);
    
    lseek(openFD, -sizeof(struct loan), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &temp1, sizeof(temp1));
	
	
	
	 
    if (bytes_written == -1) {
        perror("write"); 
        close(openFD);
        close(clientSocket);
        return ;

       }
		
    
    

    
    }//nul condition
       
  }//while loop condition
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);
    
    
    
    close(openFD);
    // calling function to make the employee assigned for loan to y
    calling_to_make_applied_for_loan_to_y_on_employee(clientSocket,t.loginId);
    
    
    

};





























void remove_rejected_loan_application(int clientSocket){


   struct loan temp;
   int bytes_written=-1;
    memset(&temp,'\0', sizeof(temp));
    int openFD = open("database/loan_database.txt", O_RDWR, 0774);
    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    char yn[2];
    strcpy(yn,"n");

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
    
       fcntl(openFD, F_SETLKW, &mylock);

    while (read(openFD, &temp, sizeof(temp)) > 0) {
        if ((strcmp(temp.status,"rej") == 0) && (strcmp(temp.read_by_manager,"y") == 0)) {
                      
            
            lseek(openFD, -sizeof(struct loan), SEEK_CUR);
            memset(&temp,'\0',sizeof(temp));
	bytes_written = write(openFD, &temp, sizeof(temp));
	    
	    if (bytes_written == -1) {
	        perror("write");
	        close(openFD);
	        close(clientSocket);
	        return ;
	    }
	    
      //
        }
        

    
    
    
    }
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);
    close(openFD); 
   

}














void reviewCustomerFeedback(int clientSocket){




    printf("\n");
    struct loan cust, temp;
    memset(&temp,'\0', sizeof(temp));
    int openFD = open("database/loan_database.txt", O_RDWR, 0774);
    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false;
    char buffer[1024];


    send(clientSocket, "Enter Customer ID: ", strlen("Enter Customer ID: "), 0);
    int readResult = read(clientSocket, cust.userId, sizeof(cust.userId) - 1);
    if (readResult <= 0) {
        send(clientSocket, "Error receiving Customer ID from server", strlen("Error receiving Customer ID from server"), 0);
        return;
    }
    cust.userId[readResult] = '\0';
    
    
    

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
    
       fcntl(openFD, F_SETLKW, &mylock);

    while (read(openFD, &temp, sizeof(temp)) > 0) {
        if (strcmp(cust.userId, temp.userId) == 0) {
        
        
        if(strcmp(temp.feedback_from_customer_about_employee,"null") == 0) continue;
        
            found = true;
            strcpy(temp.read_by_manager,"y");
            lseek(openFD, -sizeof(struct loan), SEEK_CUR);
	    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
	    

	    if (bytes_written == -1) {
	        perror("write");
	        close(openFD);
	        close(clientSocket);
	        return ;
	    }
	    
            break;
        }
    }
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);



    if (found && strcmp(temp.status,"pen")!=0) {
        // Construct the details message
        sprintf(buffer, "...................Customer Feedback ..................\nCustomer Id: %s\nEmployee Id: %s\nFeedback:%s\nStatus: %s\n.............................................\nPress Enter to Move on\n",
                temp.userId, temp.empId, temp.feedback_from_customer_about_employee,temp.status);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
    } 
    else if (found && strcmp(temp.status,"pen")==0) {
       
  send(clientSocket, "Customer found\nBut the Loan Application is still pending\nNo feedback about employee yet\nPress ENTER  to move on\n", strlen( "Customer found\nBut the Loan Application is still pending\nNo feedback about employee yet\nPress ENTER  to move on\n"), 0);
    
    }
    
    else {
        send(clientSocket, "Customer not found\nIf found, but no feedbacks yet\n", strlen("Customer not found\nIf found, but no feedbacks yet\n"), 0);
    }

    close(openFD); 
    
    // remove the rejected part
    remove_rejected_loan_application(clientSocket);
    
    
};



























void changePasswordManager(int clientSocket){

    printf("\n");
    struct employee emp,temp;
    memset(emp.loginId,'\0', sizeof(emp.loginId));
    memset(emp.password,'\0', sizeof(emp.password));
    
    strcpy(emp.loginId, temporary2.loginId);
    send(clientSocket, "Enter New Admin Passowrd: ", strlen("Enter New Admin Passowrd: "), 0);
     int readResult = read(clientSocket, emp.password, sizeof(emp.password) - 1);

    if (readResult <= 0) 
    {
        send(clientSocket, "Error receiving Manager Password from server", strlen("Error receiving Manager Password from server"), 0);
        return;
    }
    emp.password[readResult] = '\0';
    // now the data is ready, but we shall change it. 
    
    
    int openFD = open("database/employee_database.txt", O_RDWR, 0774); // Open in read-only mode
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

        if (   strcmp(temp.position, "mng") != 0) continue;
        //This checks whther my login id is matching or not
        if (   strcmp(emp.loginId, temp.loginId) == 0) 
        {
        
         // if the login id matched but the password is not matched, then
         found = true;
       
        
           
             if (found) 
             {
               // then change the changed password to be written at this place
                strcpy(temp.password, emp.password);
                lseek(openFD, -sizeof(struct employee), SEEK_CUR);
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
     if(found==false) 
    {
send(clientSocket, "No account found\n", strlen("No account found\n"),0);
    }
    

    close(openFD); // Close the file after use
    return;
       
}













void changePasswordEmployee(int clientSocket){

   printf("\n");
    struct employee emp,temp;
    send(clientSocket, "\nEnter Employee ID: ", strlen("\nEnter Employee ID: "), 0);
     int readResult = read(clientSocket, temp.loginId, sizeof(temp.loginId) - 1);
    if (readResult <= 0) 
    {
    send(clientSocket, "Error receiving Employee Id in Manger from server\n", strlen("Error receiving Employee Id in Manger from server\n"), 0);
        return;
    }
    temp.loginId[readResult] = '\0';
    
    // now the data is ready, but we shall change it. 
    
    
      
    
    int openFD = open("database/employee_database.txt", O_RDWR, 0774); // Open in read-only mode
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
    
    
    
    
    
    while (read(openFD, &emp, sizeof(emp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct employee);

        if(  strcmp(emp.position,"mng") == 0 ) continue;
        //This checks whther my login id is matching or not
        if (   strcmp(emp.loginId, temp.loginId) == 0) 
        {
        
         // if the login id matched but the password is not matched, then
         found = true;
       
        
           
             if (found) 
             {
               // then change the changed password to be written at this place
send(clientSocket, "\nEmployee ID found\nEnter New password for Employee ID: ", strlen("\nEmployee ID found\nEnter New password for Employee ID: "), 0);
readResult = read(clientSocket, temp.password, sizeof(temp.password) - 1);

    if (readResult <= 0) 
    {
send(clientSocket, "Error receiving Password Employee Id in Manger from server\n", strlen("Error receiving Password Employee Id in Manger from server\n"), 0);
  return;
    }
    
    temp.password[readResult] = '\0';
    strcpy(emp.password, temp.password);
    
    
    lseek(openFD, -sizeof(emp), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &emp, sizeof(emp));
	    

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

    if(found==true) 
    {
send(clientSocket, "Password Of Employee  Changed Successfully\n", strlen("Password Of Employee  Changed Successfully\n"),0);
    }
     if(found==false) 
    {
send(clientSocket, "No account found\n", strlen("No account found\n"),0);
    }
    

    close(openFD); // Close the file after use
    return;
       
}































void changePasswordCustomer(int clientSocket){
    printf("\n");
    struct customer cust,temp;
    send(clientSocket, "\nEnter Customer ID: ", strlen("\nEnter Customer ID: "), 0);
     int readResult = read(clientSocket, temp.loginId, sizeof(temp.loginId) - 1);
    if (readResult <= 0) 
    {
    send(clientSocket, "Error receiving Employee Id in Manger from server\n", strlen("Error receiving Employee Id in Manger from server\n"), 0);
        return;
    }
    temp.loginId[readResult] = '\0';
    
    // now the data is ready, but we shall change it. 
    
    
      
    
    int openFD = open("database/customer_database.txt", O_RDWR, 0774); // Open in read-only mode
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
    
    
    
    
    
    while (read(openFD, &cust, sizeof(cust)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct customer);

        
        //This checks whther my login id is matching or not
        if (   strcmp(cust.loginId, temp.loginId) == 0) 
        {
        
         // if the login id matched but the password is not matched, then
         found = true;
       
        
           
             if (found) 
             {
               // then change the changed password to be written at this place
send(clientSocket, "\nCustomer ID found\nEnter New password for Customer ID: ", strlen("\nCustomer ID found\nEnter New password for Customer ID: "), 0);
readResult = read(clientSocket, temp.password, sizeof(temp.password) - 1);

    if (readResult <= 0) 
    {
send(clientSocket, "Error receiving Password Customer ID in Manger from server\n", strlen("Error receiving Password Customer ID in Manger from server\n"), 0);
  return;
    }
    
    temp.password[readResult] = '\0';
    strcpy(cust.password, temp.password);
    
    
    lseek(openFD, -sizeof(struct customer), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &cust, sizeof(cust));
	    

	    if (bytes_written == -1) {
	        perror("write");
	        send(clientSocket, "Cannot update Password Of the Customer \n", strlen("Cannot update Password Of the Customer \n"),0);
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

        recordoff += sizeof(struct customer);
    }

    if(found==true) 
    {
send(clientSocket, "Password Of Customer  Changed Successfully\n", strlen("Password Of Customer  Changed Successfully\n"),0);
    }
     if(found==false) 
    {
send(clientSocket, "No account found\n", strlen("No account found\n"),0);
    }
    
    

    close(openFD); // Close the file after use
    return;
       
}
