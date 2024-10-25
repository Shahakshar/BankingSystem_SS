#include "../database/structure.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

struct userCred temporary;
char acc_name[50];

  
int addEmployee(int clientSocket);
void viewEmployee(int clientSocket);
int addCustomer(int clientSocket);
void viewCustomer(int clientSocket);
void activateAccount(int clientSocket);
void deactivateAccount(int clientSocket);
void modifyEmployee(int clientSocket);
void modifyCustomer(int clientSocket);
void modifyAccount(int clientSocket);
void addAdmin(int clientSocket);
void manage_user_roles(int clientSocket);









void changePasswordAdmin(int clientSocket){


    struct admin adm,temp;
    memset(adm.loginId,'\0', sizeof(adm.loginId));
    memset(adm.password,'\0', sizeof(adm.password));
    
    strcpy(adm.loginId, temporary.loginId);
    send(clientSocket, "Password\n", strlen("Password\n"), 0);
     int readResult = read(clientSocket, adm.password, sizeof(adm.password) - 1);

    if (readResult <= 0) 
    {
        send(clientSocket, "Error receiving Admin Password from server", strlen("Error receiving Admin Password from server"), 0);
        return;
    }
    adm.password[readResult] = '\0';
    // now the data is ready, but we shall change it. 
    
    
    int openFD = open("database/admin_database.txt", O_RDWR, 0744); // Open in read-only mode
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
        mylock.l_len = sizeof(struct admin);
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        //This checks whther my login id is matching or not
        if (   strcmp(adm.loginId, temp.loginId) == 0) 
        {
        
         // if the login id matched but the password is not matched, then
         found = true;
       
        
           
             if (found) 
             {
               // then change the changed password to be written at this place
                lseek(openFD, -sizeof(adm), SEEK_CUR);
	    ssize_t bytes_written = write(openFD, &adm, sizeof(adm));
	    

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
        

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }

        recordoff += sizeof(struct admin);
    }

    if(found==true) {
        send(clientSocket, "Password Changed Successfully\n", strlen("Password Changed Successfully\n"),0);
    }

    close(openFD); // Close the file after use
    return;
       
}

int sendPromptAndReceiveResponse(int , const char *, char *, size_t );

















bool admin_authenticate_1(int clientSocket){
struct admin adm, temp;
int openFD = open("database/admin_database.txt", O_RDWR, 0744); // Open in read-only mode
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

    send(clientSocket, "Enter Admin ID: ", strlen("Enter Admin ID: "), 0);
    int readResult = read(clientSocket, adm.loginId, sizeof(adm.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Admin ID from server", strlen("Error receiving Admin ID from server"), 0);
        return false;
    }
    adm.loginId[readResult] = '\0';
    
    if(isOnline(adm.loginId)){
    
         send(clientSocket, "Admin Already Logged in\n", strlen("Admin Already Logged in\n"), 0);
         return false;
    
    }
    
    
    
    
    send(clientSocket, "Password\n", strlen("Password\n"), 0);
     readResult = read(clientSocket, adm.password, sizeof(adm.password) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Admin Password from server", strlen("Error receiving Admin Password from server"), 0);
        return false;
    }
    adm.password[readResult] = '\0';
    
    
    
    
    
    
    
    
   
   
    

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct admin);
        
          // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }


        
        //This checks whther my login id is matching or not
        if (   strcmp(adm.loginId, temp.loginId) == 0) 
        {
        
         if (   strcmp(adm.password, temp.password) != 0)
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
               memset(temporary.loginId,'\0',sizeof(temporary.loginId));
               memset(temporary.password,'\0',sizeof(temporary.password));        
    
   	   strcpy(temporary.loginId, temp.loginId);
   	   strcpy(temporary.password, temp.password);
   	   add_session(temporary.loginId);
   	   return true;
   	       
    	  
             }

            break;
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







void logout1(int clientSocket){

   remove_session(temporary.loginId);

}























bool adminHandle(int clientSocket) {
    
    if (admin_authenticate_1(clientSocket)) {
    char readbuff[1024];
        send(clientSocket, "Logged in Successfully\n", strlen("Logged in Successfully\n"), 0);
       while(true){
      char adminPrompt[] = "\t\tWelcome to Admin Menu\n"
                     "---------------------------------\n"
                     " 1. Add Employee\n"
                     " 2. View Employee Details\n"
                     " 3. Add Customer\n"
                     " 4. View a Customer Details\n"
                     " 5. Activate Account\n"
                     " 6. Block Account\n"
                     " 7. Update Employee\n"
                     " 8. Update Customer\n"
                     " 9. Update Account\n"
                     " 10. Change Passowrd\n"
                     " 11. Add Admin\n"
                     " 12. Manage User Roles\n"
                     " 13. Logout and Exit\n"
                     "---------------------------------\n"
                     "Please select an option: ";

        write(clientSocket, adminPrompt, strlen(adminPrompt));

        // readBytes stores the number of bytes read from the client by the server
        ssize_t readBytes = recv(clientSocket, readbuff, sizeof(readbuff), 0);
        if (readBytes == -1) {
            perror("Error in the choice you provided");
            return false;
        }
        
        //printf("\nHello I'm in admin\n");
        //printf("\nInformation admin: %s    %s", temporary.loginId, temporary.password);
        

        int choice = atoi(readbuff);
         //send(clientSocket, readbuff, sizeof(readbuff), 0);

        switch (choice) {
            case 1:
                addEmployee(clientSocket) ;
                break;
             case 2:   
                viewEmployee(clientSocket);
                break;
          case 3:
          	addCustomer(clientSocket);
          break;
          case 4:
          	 viewCustomer(clientSocket);
          break;
          case 5:
          	activateAccount(clientSocket);
          	//printf("activated\n");
          	
          break;
          case 6:
          	deactivateAccount(clientSocket);
          	//printf("Blocked\n");
          break;
          case 7:
           modifyEmployee(clientSocket);  
      break;  
          case 8:
           modifyCustomer(clientSocket);  
      break; 
      case 9:
           modifyAccount(clientSocket);  
      break; 
      case 10:
           changePasswordAdmin(clientSocket);  
      break; 
      case 11:
           addAdmin(clientSocket);  
      break; 
      case 12:
           manage_user_roles(clientSocket);  
      break; 
      case 13:
           logout1(clientSocket);  
           return true;
      break; 
            default:
            return true; 
                break;
        }
      }
    }
    return false;
}











int sendPromptAndReceiveResponse(int clientSocket, const char *prompt, char *response, size_t responseSize) {
    send(clientSocket, prompt, strlen(prompt), 0);
	ssize_t bytes_received = read(clientSocket, response, responseSize - 1);


    if (bytes_received <= 0) {
        perror("recv");
        return -1;
    }

    response[bytes_received] = '\0'; // Null-terminate the received string
    if (response[bytes_received - 1] == '\n') {
        response[bytes_received - 1] = '\0';
    }

    return 0; // Success
}



















int addEmployee(int clientSocket) {
    
    struct employee emp;

    memset(emp.name, '\0', sizeof(emp.name));
    memset(emp.loginId, '\0', sizeof(emp.loginId));
    memset(emp.password, '\0', sizeof(emp.password));
    memset(emp.age, '\0', sizeof(emp.age));
    memset(emp.emailAddress, '\0', sizeof(emp.emailAddress));
    memset(emp.mobile, '\0', sizeof(emp.mobile));
    memset(emp.date_of_joining, '\0', sizeof(emp.date_of_joining));
    memset(emp.position, '\0', sizeof(emp.position));
    memset(emp.managerId, '\0', sizeof(emp.managerId));
    memset(emp.assigned_for_loan, '\0', sizeof(emp.assigned_for_loan));
    memset(emp.address, '\0', sizeof(emp.address));
    

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Name: ", emp.name, sizeof(emp.name)) == -1) {
        close(clientSocket);
        return 0;
    }
 
    	
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee LoginId: ", emp.loginId, sizeof(emp.loginId)) == -1) {
        close(clientSocket);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Password\n", emp.password, sizeof(emp.password)) == -1) {
        close(clientSocket);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Age: ", emp.age, sizeof(emp.age)) == -1) {
        close(clientSocket);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Email Address: ", emp.emailAddress, sizeof(emp.emailAddress)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Mobile: ", emp.mobile, sizeof(emp.mobile)) ==-1) {
        close(clientSocket);
        return 0;
    }
     if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Date Of Joining: ", emp.date_of_joining, sizeof(emp.date_of_joining)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
    //sprintf(emp.position,"emp");
    //sprintf(emp.managerId,"MT2024048");
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Position(emp or mng ): ", emp.position, sizeof(emp.position)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
      if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Manger ID: ", emp.managerId, sizeof(emp.managerId)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
      if (sendPromptAndReceiveResponse(clientSocket, "Is Employee Assigned For Loan(y or n): ", emp.assigned_for_loan, sizeof(emp.assigned_for_loan)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
      if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee address: ", emp.address, sizeof(emp.address)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
    
 
    

    int openFD = open("database/employee_database.txt", O_RDWR | O_CREAT | O_APPEND, 0644); // Open the file in append mode

    if (openFD == -1) {
        perror("open");
        close(clientSocket);
        return 0;
    }


struct flock mylock;
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
        
        
        
        
        
    fcntl(openFD, F_SETLKW, &mylock);
    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &emp, sizeof(emp));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return 0;
    }


        mylock.l_type = F_UNLCK;
        fcntl(openFD, F_SETLK, &mylock);
        
        
    close(openFD);

    // Optionally, you can send a success message to the client.
    char successMessage[1024] = "";
    memset(successMessage,'\0',sizeof(successMessage));
    sprintf(successMessage,"\n************ Employee added successfully ***************\n                           Login ID is %s\n******************************************************\nPress Enter to Move on\n",emp.loginId);
    send(clientSocket, successMessage, strlen(successMessage), 0);

    return 1; // Success
}



















void viewEmployee(int clientSocket) {
    struct employee my_emp, temp;
    int openFD = open("database/employee_database.txt", O_RDONLY, 0744); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false

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
    	
    
    	
    	
    		
        sprintf(tempbuf, "...................Employee..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nEmailAddress: %s\nMobile: %s\nDate Of Joining: %s\nPosition: %s\nManager Id: %s\nLoan Assistance: %s\nAddress: %s\n.............................................\nPress Enter to Move on\n",
                temp.name, temp.age, temp.loginId, temp.password,temp.emailAddress, temp.mobile,temp.date_of_joining, temp.position, temp.managerId, temp.assigned_for_loan, temp.address);
                
                
                strcat(buffer,tempbuf);
        }
        
    send(clientSocket,buffer,sizeof(buffer),0);
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);

    

    close(openFD); // Close the file after use
}
















void viewCustomer(int clientSocket) {
    
    struct customer cust, temp;
    int openFD = open("database/customer_database.txt", O_RDONLY, 0744);

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false;

    char buffer[1024];

    send(clientSocket, "Enter Customer ID: ", strlen("Enter Customer ID: "), 0);
    int readResult = read(clientSocket, cust.loginId, sizeof(cust.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Faculty ID from server", strlen("Error receiving student ID from server"), 0);
        return;
    }
    cust.loginId[readResult] = '\0';

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
        if (strcmp(cust.loginId, temp.loginId) == 0) {
            found = true;
            break;
        }
    }
    
    mylock.l_type=F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);



    if (found) {
        // Construct the details message
        sprintf(buffer, "...................Customer..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nMobile Number: %s\n.............................................\n",
                temp.name, temp.age, temp.loginId, temp.password, temp.mobile);

        // Send the details to the client
        send(clientSocket, buffer, strlen(buffer), 0);
    } 
    
    else {
        send(clientSocket, "Customer not found\n", strlen("Customer not found\n"), 0);
    }

    close(openFD);
}











void addAdmin(int clientSocket){

    int nbyt=0;
    struct admin adm;

    memset(adm.loginId, '\0', sizeof(adm.loginId));
    memset(adm.password, '\0', sizeof(adm.password));
    
    //strcpy(adm.loginId, temporary.loginId);
    
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter New Admin Id: \n", adm.loginId, sizeof(adm.loginId)) == -1) {
        close(clientSocket);
        return ;
    }
    if (sendPromptAndReceiveResponse(clientSocket, "Password\n", adm.password, sizeof(adm.password)) == -1) {
        close(clientSocket);
        return ;
    }
    
    printf("Recieved: \n%s\n%s",adm.loginId, adm.password);
    
    
   
    int openFD = open("database/admin_database.txt", O_RDWR | O_CREAT | O_APPEND, 0775); // Open the file in append mode

    if (openFD == -1) {
        perror("open");
        
        return ;
    }


        struct flock mylock;
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
        
        
        
        
        
    fcntl(openFD, F_SETLKW, &mylock);
    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &adm, sizeof(adm));

    if (bytes_written <0) {
        perror("write");
        close(openFD);
        
        return ;
    }


        mylock.l_type = F_UNLCK;
        fcntl(openFD, F_SETLK, &mylock);
        
        
    close(openFD);
    send(clientSocket, "Successfully Added a Admin\nPress Enter to move on\n", strlen("Successfully Added a Admin\nPress Enter to move on\n"), 0);

    

    return ; // Success
}

void create_account_to_customer(int clientSocket ,char *id,  char *name){
 
    struct account acc;
    memset(&acc,'\0', sizeof(acc));
    strcpy(acc.loginId, id);
    strcpy(acc.account_holder_name,name);
    strcpy(acc.balance,"0");
    strcpy(acc.activation, "a");
    
    
    /*if (sendPromptAndReceiveResponse(clientSocket, "Enter Unique Account Number: ", acc.account_number, sizeof(acc.account_number)) == -1) {
        close(clientSocket);
        return ;
    }*/
    
    srand(time(0));
    // Generate a random number between 100000 and 999999
    int random_number = 100000 + rand() % 900000;
    //printf("Account Number: %s\n", acc.account_number);
    char str[20];
    memset(str,'\0',sizeof(str));
    sprintf(str, "%d", random_number);
    strcpy(acc.account_number,  str);
    
    
    
    strcpy(acc_name, acc.account_number);
    printf("Account Number: %s", acc_name);
    
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Bank Name: ", acc.bank_name, sizeof(acc.bank_name)) == -1) {
        close(clientSocket);
        return ;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Branch Name: ", acc.branch_name, sizeof(acc.branch_name)) == -1) {
        close(clientSocket);
        return ;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Date Of Opening: ", acc.date_of_opening, sizeof(acc.date_of_opening)) == -1) {
        close(clientSocket);
        return ;
    }
    
    // The entries are over , start pushing into to database
int openFD = open("database/account_database.txt", O_RDWR | O_CREAT | O_APPEND, 0744); // Open the file in append mode

    if (openFD == -1) {
        perror("open");
        close(clientSocket);
        return ;
    }


struct flock mylock;
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
        
        
        
    fcntl(openFD, F_SETLKW, &mylock);
    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &acc, sizeof(acc));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
    
    mylock.l_type = F_UNLCK;
    fcntl(openFD, F_SETLK, &mylock);

    close(openFD);
    
    return;
  
}



int addCustomer(int clientSocket) {
    struct customer cust;
    memset(cust.name, '\0', sizeof(cust.name));
    memset(cust.password, '\0', sizeof(cust.password));
    memset(cust.loginId, '\0', sizeof(cust.loginId));
    memset(cust.age, '\0', sizeof(cust.age));
    memset(cust.emailAddress, '\0', sizeof(cust.emailAddress));
    memset(cust.mobile, '\0', sizeof(cust.mobile));
    memset(cust.address, '\0', sizeof(cust.address));
    memset(cust.applied_for_loan, '\0', sizeof(cust.applied_for_loan));
    
    
    


    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Name: ", cust.name, sizeof(cust.name)) == -1) {
        close(clientSocket);
        return 0;
    }

     //sprintf(facul.loginId,"PF%d",generateFid());
     if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer LoginId: ", cust.loginId, sizeof(cust.loginId)) == -1) {
        close(clientSocket);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Password\n", cust.password, sizeof(cust.password)) == -1) {
        close(clientSocket);
        return 0;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Age: ", cust.age, sizeof(cust.age)) == -1) {
        close(clientSocket);
        return 0;
    }
    


   
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Mobile Number: ", cust.mobile, sizeof(cust.mobile)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
     if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Email Address: ", cust.emailAddress, sizeof(cust.emailAddress)) ==-1) {
        close(clientSocket);
        return 0;
    }
if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Address: ", cust.address, sizeof(cust.address)) ==-1) {
        close(clientSocket);
        return 0;
    }
    
sprintf(cust.applied_for_loan,"n");
    
    
    int openFD = open("database/customer_database.txt", O_RDWR | O_CREAT | O_APPEND, 0744); // Open the file in append mode

    if (openFD == -1) {
        perror("open");
        close(clientSocket);
        return 0;
    }


struct flock mylock;
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
        
        
        
        
        
    fcntl(openFD, F_SETLKW, &mylock);
    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &cust, sizeof(cust));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return 0;
    }
    
    mylock.l_type = F_UNLCK;
    fcntl(openFD, F_SETLK, &mylock);

    

    // Optionally, you can send a success message to the client.
   create_account_to_customer(clientSocket,cust.loginId, cust.name);
   char successMessage[1024] = "";
    sprintf(successMessage,"\n************ Customer added successfully ***************\n                           Login ID :  %s\nAccount Number: %s\n******************************************************\nPress Enter to Move on\n",cust.loginId,acc_name);
    send(clientSocket, successMessage, strlen(successMessage), 0);
    
   close(openFD);
    return 1; // Success
    
}

void modifyEmployee(int clientSocket) {
    int count=0;
    struct employee my_emp, temp;
    off_t recordoff = 0;
    int openFD = open("database/employee_database.txt", O_RDWR, 0775); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(buffer,'\0',sizeof(buffer));

    send(clientSocket, "Enter Employee ID: ", strlen("Enter Employee ID: "), 0);
    int readResult = read(clientSocket, my_emp.loginId, sizeof(my_emp.loginId) - 1);

    //printf("Employee Id Entered: %s\n", my_emp.loginId);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Employee ID from server", strlen("Error receiving Employee ID from server"), 0);
        return;
    }
    
    struct flock mylock;
    my_emp.loginId[readResult] = '\0';
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
{      
        count++;
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct employee);
        mylock.l_pid = getpid();
        
        
        // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
       
        }



        if (strcmp(my_emp.loginId, temp.loginId) == 0) 
    { 
            // Compare the student IDs
            found = true;
            
            
            
       //printf("SEEK_CUR :%d\n", SEEK_CUR);     
            
            
 if (found) 
 {
    //printf("The number th record: %d\n", count); 
    
    sprintf(buffer, "...................Previous Employee Data..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\n.............................................\n",
                temp.name, temp.age, temp.loginId, temp.password);
   send(clientSocket, buffer, strlen(buffer), 0);
        // Construct the details message
        
        memset(buffer,'\0',sizeof(buffer));
        
    struct employee emp;
	strcpy(emp.loginId,temp.loginId);
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Name: ", emp.name, sizeof(emp.name)) == -1) {
        close(clientSocket);
        return ;
    }


    if (sendPromptAndReceiveResponse(clientSocket, "Password\n", emp.password, sizeof(emp.password)) == -1) {
        close(clientSocket);
        return ;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Age: ", emp.age, sizeof(emp.age)) == -1) {
        close(clientSocket);
        return ;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Email Address: ", emp.emailAddress, sizeof(emp.emailAddress)) ==-1) {
        close(clientSocket);
        return ;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Mobile Number: ", emp.mobile, sizeof(emp.mobile)) ==-1) {
        close(clientSocket);
        return ;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Date Of Joining: ", emp.date_of_joining, sizeof(emp.date_of_joining)) ==-1) {
        close(clientSocket);
        return ;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Address : ", emp.address, sizeof(emp.address)) ==-1) {
        close(clientSocket);
        return ;
    }
    
     if (sendPromptAndReceiveResponse(clientSocket, "Is employee assigned for loan procedure(y/n)? : ", emp.address, sizeof(emp.assigned_for_loan)) ==-1) {
        close(clientSocket);
        return ;
    }
    
    
    strcpy(emp.position, temp.position);
    strcpy(emp.managerId, temp.managerId);
    
    //printf("bEFORE CHANGING USING LSEEK SEEK_CUR :%d\n", SEEK_CUR);    
    lseek(openFD, -sizeof(struct employee), SEEK_CUR);
    //printf("AFTER CHANGING USING LSEEK SEEK_CUR :%d\n", SEEK_CUR);
      
    ssize_t bytes_written = write(openFD, &emp, sizeof(emp));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
        mylock.l_type=F_UNLCK;
        fcntl(openFD, F_SETLKW, &mylock);
    
   
       sprintf(buffer, "...................Updated Employee Data..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nMobile: %s\nEmailAddress: %s\nDate Of Joining: %s\nPosition: %s\nManager Id: %s\nAssigned For Loan Procedure: %s\nAddress: %s\n.............................................\nPress Enter to Move on\n",
                emp.name, emp.age, emp.loginId, emp.password, emp.mobile, emp.emailAddress,emp.date_of_joining, emp.position, emp.managerId, emp.assigned_for_loan, emp.address);
   send(clientSocket, buffer, strlen(buffer), 0);

        // Send the details to the client
        send(clientSocket, "Successfully updated\n", strlen("Successfully updated\n"), 0);
    }
         
         
         
         
         
      break;
 }

        




        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }
        
        

        recordoff += sizeof(struct employee);
}









     if(found==false) 
     {
        send(clientSocket, "Employee not found\n", strlen("Employee not found\n"), 0);
     }

    close(openFD); // Close the file after use
}


















void modifyCustomer(int clientSocket) {
off_t recordoff = 0;
    struct customer cust, temp;
    int openFD = open("database/customer_database.txt", O_RDWR, 0774); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(buffer, '\0', sizeof(buffer));

    send(clientSocket, "Enter Customer ID: ", strlen("Enter Customer ID: "), 0);
    int readResult = read(clientSocket, cust.loginId, sizeof(cust.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Customer ID from server", strlen("Error receiving Customer ID from server"), 0);
        return;
    }
    cust.loginId[readResult] = '\0';

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    
    
    
    
    struct flock mylock;
    cust.loginId[readResult] = '\0';
while (read(openFD, &temp, sizeof(temp)) > 0) {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct customer);
        
         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }

        if (strcmp(cust.loginId, temp.loginId) == 0) {
            // Compare the student IDs
            found = true;
             if (found) {
    
    sprintf(buffer, "...................Previous  Customer Data..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\n.............................................\n",
                temp.name, temp.age, temp.loginId, temp.password);
   send(clientSocket, buffer, strlen(buffer), 0);
        // Construct the details message
    struct customer cus;
    strcpy(cus.loginId,temp.loginId);
	
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Name: ", cus.name, sizeof(cus.name)) == -1) {
        close(clientSocket);
        return ;
    }


    if (sendPromptAndReceiveResponse(clientSocket, "Password\n", cus.password, sizeof(cus.password)) == -1) {
        close(clientSocket);
        return ;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Age: ", cus.age, sizeof(cus.age)) == -1) {
        close(clientSocket);
        return ;
    }

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Email Address: ", cus.emailAddress, sizeof(cus.emailAddress)) ==-1) {
        close(clientSocket);
        return ;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Address: ", cus.address, sizeof(cus.address)) ==-1) {
        close(clientSocket);
        return ;
    }
    
     if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Mobile Number: ", cus.mobile, sizeof(cus.mobile)) ==-1) {
        close(clientSocket);
        return ;
    }
    
    strcpy(cus.applied_for_loan,"n");
    
    
    lseek(openFD, -sizeof(struct customer), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &cus, sizeof(cus));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
    
   
       sprintf(buffer, "...................Updated Customer Data..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\n.............................................\n",
                cus.name, cus.age, cus.loginId, cus.password);
   send(clientSocket, buffer, strlen(buffer), 0);

        // Send the details to the client
        send(clientSocket, "Successfully updated\n", strlen("Successfully updated\n"), 0);
    }

            break;
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
        send(clientSocket, "Customer not found\n", strlen("Customer not found\n"), 0);
    }

    close(openFD); // Close the file after use
}





















void modifyAccount(int clientSocket) {
off_t recordoff = 0;



    struct account cust, temp;
    int openFD = open("database/account_database.txt", O_RDWR, 0744); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false, found_pass=false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(buffer, '\0', sizeof(buffer));
    
    
    
    
    

    send(clientSocket, "Enter Customer ID: ", strlen("Enter Customer ID: "), 0);
    int readResult = read(clientSocket, cust.loginId, sizeof(cust.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Customer ID from server", strlen("Error receiving Customer ID from server"), 0);
        return;
    }
    cust.loginId[readResult] = '\0';
    printf("Customer Id we are getiing is : %s\n", cust.loginId);
    
    
    
    
    
    
    
    
    
    
    
    

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct account);

         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        
        //This checks whther my login id is matching or not
        if (   strcmp(cust.loginId, temp.loginId) == 0) {
        
        
        // if the login id matched but the password is not matched, then
         found = true;
       
        
        
           
             if (found) 
             {
    
    sprintf(buffer, "...................Previous  Account Data..................\nName: %s\nAccount Balance: %s\nLogin ID: %s\nDate Of Opening: %s\n.............................................\n",
                temp.account_holder_name, temp.balance, temp.loginId, temp.date_of_opening);
   send(clientSocket, buffer, strlen(buffer), 0);
        // Construct the details message
    struct account cus;
    strcpy(cus.loginId,temp.loginId);
    strcpy(cus.account_holder_name,temp.account_holder_name);
    strcpy(cus.account_number,temp.account_number);
    strcpy(cus.date_of_opening,temp.date_of_opening);
    //strcpy(cus.activation,temp.activation);
    strcpy(cus.bank_name, temp.bank_name);
    
      

    
    
    if (sendPromptAndReceiveResponse(clientSocket, "Activation Status(a or d):  ", cus.activation, sizeof(cus.activation)) ==-1) {
        close(clientSocket);
        return ;
    }
    
     if (sendPromptAndReceiveResponse(clientSocket, "Enter Branch Name: ", cus.branch_name, sizeof(cus.branch_name)) ==-1) {
        close(clientSocket);
        return ;
    }
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter New Balance: ", cus.balance, sizeof(cus.balance)) ==-1) {
        close(clientSocket);
        return ;
    }
    
   
    
    
    lseek(openFD, -sizeof(struct account), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &cus, sizeof(cus));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
    
   
       sprintf(buffer, "...................Updated Account Data..................\nName: %s\nAge: %s\nAccount Balance: %s\nDate Of Opening: %s\n.............................................\n",              cus.account_holder_name, cus.balance, cus.loginId, cus.date_of_opening);
   send(clientSocket, buffer, strlen(buffer), 0);

        // Send the details to the client
        send(clientSocket, "Successfully updated\n", strlen("Successfully updated\n"), 0);
    }

            break;
        }










       

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }

        recordoff += sizeof(struct account);
    }







    if(found==false) {
        send(clientSocket, "Account not found\n", strlen("Account not found\n"),0);
    }

    close(openFD); // Close the file after use
}































void activateAccount(int clientSocket){
    struct account my_account, temp;
    int openFD = open("database/account_database.txt", O_RDWR, 0644); // Open in read-only mode
    int recordoff=0;

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data

    send(clientSocket, "Enter Account User  ID: ", strlen("Enter Account User  ID: "), 0);
    int readResult = read(clientSocket, my_account.loginId, sizeof(my_account.loginId) - 1);
    if (readResult <= 0) {
        send(clientSocket, "Error receiving Account  ID from server", strlen("Error receiving Account  ID from server"), 0);
        return;
    }
    my_account.loginId[readResult] = '\0';
    
    
    struct flock mylock;
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);

    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0) {
    mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct customer);

         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
    
    if (strcmp(my_account.loginId, temp.loginId) == 0) { // Compare the student IDs
            found = true;
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }

            break;
        }
        
        
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }





    }

    if (found) {
    
    sprintf(buffer, "...................Previous Student Data..................\nName: %s\nLogin ID: %s\nActivation status(1 for active and 0 for blocked): %s\n.............................................\n",
                temp.account_holder_name, temp.loginId,  temp.activation);
   send(clientSocket, buffer, strlen(buffer), 0);
 
 strcpy(temp.activation,"a");
    
    lseek(openFD, -sizeof(temp), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
    
   
       sprintf(buffer, "...................Updated Account Data..................\nName: %s\nLogin ID: %s\nActivation status(1 for active and 0 for blocked): %s\n.............................................\n",
                temp.account_holder_name, temp.loginId, temp.activation);
   send(clientSocket, buffer, strlen(buffer), 0);

        // Send the details to the client
        send(clientSocket, "Successfully updated\n", strlen("Successfully updated\n"), 0);
    } else {
        send(clientSocket, "Account not found\n", strlen("Account not found\n"), 0);
    }

    close(openFD); // Close the file after use

}










void manage_user_roles(int clientSocket){

  /*
  Ask few questions to admin and fif the answers are yes to all, the make him as the manager
  */
  int count=0;
    struct employee my_emp, temp;
    off_t recordoff = 0;
    int openFD = open("database/employee_database.txt", O_RDWR, 0775); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(buffer,'\0',sizeof(buffer));

    send(clientSocket, "Enter Employee ID: ", strlen("Enter Employee ID: "), 0);
    int readResult = read(clientSocket, my_emp.loginId, sizeof(my_emp.loginId) - 1);

    //printf("Employee Id Entered: %s\n", my_emp.loginId);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Employee ID from server", strlen("Error receiving Employee ID from server"), 0);
        return;
    }
    
    struct flock mylock;
    my_emp.loginId[readResult] = '\0';
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
{      
        count++;
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct employee);
        mylock.l_pid = getpid();
        
        
        // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
       
        }

        

        if ( strcmp(my_emp.loginId, temp.loginId)== 0 && strcmp(temp.position,"emp")==0 ) 
    { 
            // Compare the student IDs
            found = true;
            
       
            
       //printf("SEEK_CUR :%d\n", SEEK_CUR);     
            
            
 if (found) 
 {   
 
    /*
    I'll ask some questions to the client
    1. Is he commited to the bank?
    2. Test score >70
    3. Interview Score more than >75
    
    If above all are yes then yes you can make the person as manager, not empployee    
    */
    
    char bc[2],ts[2],is[2];
    memset(bc,'\0',sizeof(bc));
    memset(ts,'\0',sizeof(ts));
    memset(is,'\0',sizeof(is));
    
    
    send(clientSocket, "\nIs Employee Committed to Bank?(y/n):  ", strlen("\nIs Employee Committed to Bank?(y/n): "), 0);
    int readResult1 = read(clientSocket, bc, sizeof(bc) - 1);
    if (readResult <= 0) {
        send(clientSocket, "Error receiving Account  ID from server", strlen("Error receiving Account  ID from server"), 0);
        return;
    }
    bc[readResult1] = '\0';   
    
    
    
    send(clientSocket, "\nIs Written test Score is more than 70?(y/n):  ", strlen("\nIs Written test Score is more than 70?(y/n):  "), 0);
    int readResult2 = read(clientSocket, ts, sizeof(ts) - 1);
    if (readResult <= 0) {
        send(clientSocket, "Error receiving Account  ID from server", strlen("Error receiving Account  ID from server"), 0);
        return;
    }
    ts[readResult2] = '\0'; 
    
    
    
    
    send(clientSocket, "\nIs Interview test Score is more than 75?(y/n):  ", strlen("\nIs Interview test Score is more than 75?(y/n):  "), 0);
    int readResult3 = read(clientSocket, is, sizeof(is) - 1);
    if (readResult <= 0) {
        send(clientSocket, "Error receiving Account  ID from server", strlen("Error receiving Account  ID from server"), 0);
        return;
    }
    is[readResult3] = '\0'; 
    
    //printf("Printing\n");
    //printf("\nbc: %s   ts: %s  is: %s\n",bc,ts,is);
    if((strcmp(bc,"y")==0) && (strcmp(ts,"y")==0) && (strcmp(is,"y")==0)){
    
       printf("\nAll condidtions satisfied");
       strcpy(temp.position,"mng");
       strcpy(temp.managerId,temp.loginId);
    
    }
    
        
 
     
    //printf("bEFORE CHANGING USING LSEEK SEEK_CUR :%d\n", SEEK_CUR);    
    lseek(openFD, -sizeof(struct employee), SEEK_CUR);
    //printf("AFTER CHANGING USING LSEEK SEEK_CUR :%d\n", SEEK_CUR);
      
    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
        mylock.l_type=F_UNLCK;
        fcntl(openFD, F_SETLKW, &mylock);
        send(clientSocket, "Successfully updated\n", strlen("Successfully updated\n"), 0);
    }  
      break;
 }
        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }
        recordoff += sizeof(struct employee);
}


     if(found==false) 
     {
        send(clientSocket, "Employee not found\n", strlen("Employee not found\n"), 0);
     }

    close(openFD); // Close the file after use

}





















void deactivateAccount(int clientSocket){
    struct account my_account, temp;
    int openFD = open("database/account_database.txt", O_RDWR, 0644); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data

    send(clientSocket, "Enter Account User  ID: ", strlen("Enter Account User  ID: "), 0);
    int readResult = read(clientSocket, my_account.loginId, sizeof(my_account.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Account  ID from server", strlen("Error receiving Account  ID from server"), 0);
        return;
    }
    my_account.loginId[readResult] = '\0';

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);

    // Loop to search for the student in the file
    while (read(openFD, &temp, sizeof(temp)) > 0) {
        if (strcmp(my_account.loginId, temp.loginId) == 0) { // Compare the student IDs
            found = true;
            break;
        }
    }

    if (found) {
    
    sprintf(buffer, "...................Previous Account Data..................\nName: %s\nLogin ID: %s\nActivation status(a-activated, d- deactivated): %s\n.............................................\n",
                temp.account_holder_name, temp.loginId,  temp.activation);
   send(clientSocket, buffer, strlen(buffer), 0);
 
 strcpy(temp.activation,"d");
    
    lseek(openFD, -sizeof(temp), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
    
   
       sprintf(buffer, "...................Updated Student Data..................\nName: %s\nLogin ID: %s\nActivation status(a-activated, d- deactivated): %s\n.............................................\n",
                temp.account_holder_name, temp.loginId, temp.activation);
   send(clientSocket, buffer, strlen(buffer), 0);

        // Send the details to the client
        send(clientSocket, "Successfully deactivated\n", strlen("Successfully deactivated\n"), 0);
    } else {
        send(clientSocket, "Account not found\n", strlen("Account not found\n"), 0);
    }

    close(openFD); // Close the file after use

}

















