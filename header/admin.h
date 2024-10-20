#include "../database/structure.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include "customer.h"
  
int addEmployee(int clientSocket);
void viewEmployee(int clientSocket);
int addCustomer1(int clientSocket);
void viewCustomer(int clientSocket);
void activateAccount(int clientSocket);
void deactivateAccount(int clientSocket);
void modifyEmployee(int clientSocket);
void modifyCustomer(int clientSocket);
void modifyAccount(int clientSocket);
int sendPromptAndReceiveResponse(int , const char *, char *, size_t );
void checkNumberOfAdmin(int clientSocket);
bool check_login_session(int clientSocket, struct admin);
bool checkAdminExist(int clientSocket, struct admin);
void add_admin_to_database(int clientSocket);

void logout(struct admin adm) {
	struct session us;
	int openFD = open("database/user_session.txt", O_RDWR | O_CREAT, 0644);

    	if (openFD == -1) {
		perror("Error opening file");
		return;
	}	

	bool found = false; // Initialize found to false
	
	lseek(openFD, 0, SEEK_SET);
	struct flock mylock;
	mylock.l_type = F_RDLCK;
	mylock.l_whence = SEEK_SET;
	mylock.l_start = 0;
	mylock.l_len = 0;
	mylock.l_pid = getpid();

	fcntl(openFD, F_SETLKW, &mylock);
	while (read(openFD, &us, sizeof(us)) > 0) {
		if (strcmp(us.loginId, adm.loginId) == 0) {
			found = true;
			us.isActive = false;
			lseek(openFD, -sizeof(us), SEEK_CUR); // Move file pointer back to update the record
                	write(openFD, &us, sizeof(us));
                	break;
                }
	}
	
	mylock.l_type = F_UNLCK;
	fcntl(openFD, F_SETLKW, &mylock);
	close(openFD);
		
}

void changePasswordAdmin(int clientSocket){
   printf("Password Changed\n"); 
}

bool admin_authenticate(int clientSocket, struct admin *user) {
    struct admin adm;
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Admin User ID: ", adm.loginId, sizeof(adm.loginId)) == -1) {
        close(clientSocket);
        return false;
    }
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Admin Password: ", adm.password, sizeof(adm.password)) == -1) {
        close(clientSocket);
        return false;
    }
    strcpy(user->loginId, adm.loginId);
    strcpy(user->password, adm.password);
    
    if(checkAdminExist(clientSocket, adm) && check_login_session(clientSocket, adm)){
    	return true;
    }
    else{
     	send(clientSocket, "Login Failed: ", strlen("Login Failed "), 0);
     	return false;
    }

}

void sendMsg(int clientSocket) {
	char buffer[1024] = "Invalid Input\n";
	send(clientSocket, &buffer, sizeof(buffer), 0);
}

bool adminHandle(int clientSocket) {
	struct admin adm;
    if (admin_authenticate(clientSocket, &adm)) {
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
                     " 11. View Admins\n"
                     " 12. Add Admin\n"
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
          	addCustomer1(clientSocket);
          	break;
	    case 4:
          	viewCustomer(clientSocket);
	        break;
            case 5:
          	activateAccount(clientSocket);
	        break;
	    case 6:
          	deactivateAccount(clientSocket);
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
      	    	checkNumberOfAdmin(clientSocket);
      	    	break;
            case 12:
            	add_admin_to_database(clientSocket);
            	break;
            case 13: 
            	logout(adm);
            	return true; 
                break;
            default: 
            	sendMsg(clientSocket);
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

    if (sendPromptAndReceiveResponse(clientSocket, "Enter Password : ", emp.password, sizeof(emp.password)) == -1) {
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
    
    sprintf(emp.position,"emp");
    sprintf(emp.managerId,"MT2024014");
    /*if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Position(emp or mng ): ", emp.position, sizeof(emp.position)) ==-1) {
        close(clientSocket);
        return 0;
    }*/
    
      /*if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Manger ID: ", emp.managerId, sizeof(emp.managerId)) ==-1) {
        close(clientSocket);
        return 0;
    }*/
    
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

int addCustomer1(int clientSocket) {
	struct customer cust;
	memset(cust.loginId, '\0', sizeof(cust.loginId));
	memset(cust.password, '\0', sizeof(cust.password));
	memset(cust.name, '\0', sizeof(cust.name));
	memset(cust.age, '\0', sizeof(cust.age));
	memset(cust.emailAddress, '\0', sizeof(cust.emailAddress));
	memset(cust.mobile, '\0', sizeof(cust.password));
	memset(cust.address, '\0', sizeof(cust.address));
	memset(cust.applied_for_loan, '\0', sizeof(cust.applied_for_loan));
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your UserId: ", cust.loginId, sizeof(cust.loginId)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your password: ", cust.password, sizeof(cust.password)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your name: ", cust.name, sizeof(cust.name)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your age: ", cust.age, sizeof(cust.age)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your emailAddress: ", cust.emailAddress, sizeof(cust.emailAddress)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your mobile: ", cust.mobile, sizeof(cust.mobile)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your address: ", cust.address, sizeof(cust.address)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	if (sendPromptAndReceiveResponse(clientSocket, "Enter your applied for loan(y/n): ", cust.applied_for_loan, sizeof(cust.applied_for_loan)) == -1) {
		close(clientSocket);
		return 0;
	}
	
	int openFD = open("database/customer_database.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
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
	close(openFD);
	
	char successMessage[1024] = "";
    	memset(successMessage,'\0',sizeof(successMessage));
	sprintf(successMessage,"\n************ Employee added successfully ***************\n                           Login ID is 		%s\n******************************************************\nPress Enter to Move on\n",cust.loginId);
    	send(clientSocket, successMessage, strlen(successMessage), 0);
	
	// success
	return 1;

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
    	
    	
        sprintf(tempbuf, "...................Employee..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\nEmailAddress: %s\nMobile: %s\nDate Of Joining: %s\nPosition: %s\nManager Id: %s\nLoan Assistance: %s\nAddress: %s\n.............................................\n\n",
                temp.name, temp.age, temp.loginId, temp.password,temp.emailAddress, temp.mobile,temp.date_of_joining, temp.position, temp.managerId, temp.assigned_for_loan, temp.address);
                
                // here we want all the info that is why concatitation is done instead of copying content
                strcat(buffer,tempbuf);
                // sprintf(buffer, tempbuf, 0);
                
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
    	} else {
        	send(clientSocket, "Customer not found\n", strlen("Customer not found\n"), 0);
    	}

    	close(openFD);
}

void modifyEmployee(int clientSocket) {
    struct employee my_emp, temp;
    off_t recordoff = 0;
    int openFD = open("database/employee_database.txt", O_RDWR, 0744); // Open in read-only mode

    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false

    char buffer[1024]; // Declare buffer for sending data
    memset(buffer,'\0',sizeof(buffer));

    send(clientSocket, "Enter Employee ID: ", strlen("Enter Employee ID: "), 0);
    int readResult = read(clientSocket, my_emp.loginId, sizeof(my_emp.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Employee ID from server", strlen("Error receiving Employee ID from server"), 0);
        return;
    }
    
    struct flock mylock;
    my_emp.loginId[readResult] = '\0';
    
while (pread(openFD, &temp, sizeof(temp), recordoff) > 0) 
{
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct employee);
        mylock.l_pid = getpid();

        if (strcmp(my_emp.loginId, temp.loginId) == 0) 
    {
            // Compare the student IDs
            found = true;   
 if (found) 
 {
          
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


    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee Password: ", emp.password, sizeof(emp.password)) == -1) {
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
    
    
    
    lseek(openFD, -sizeof(emp), SEEK_CUR);
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

     if(found==false) 
     {
        send(clientSocket, "Employee not found\n", strlen("Employee not found\n"), 0);
     }

    close(openFD); // Close the file after use
}

void modifyCustomer(int clientSocket) {
    off_t recordoff = 0;
    struct customer cust, temp;
    int openFD = open("database/customer_database.txt", O_RDWR, 0744); // Open in read-only mode

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


    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Password: ", cus.password, sizeof(cus.password)) == -1) {
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
    
    
    
    lseek(openFD, -sizeof(cus), SEEK_CUR);
    ssize_t bytes_written = write(openFD, &cus, sizeof(cus));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return ;
    }
    
   
       sprintf(buffer, "...................Updated Faculty Data..................\nName: %s\nAge: %s\nLogin ID: %s\nPassword: %s\n.............................................\n",
                cus.name, cus.age, cus.loginId, cus.password);
   send(clientSocket, buffer, strlen(buffer), 0);

        // Send the details to the client
        send(clientSocket, "Successfully updated\n", strlen("Successfully updated\n"), 0);
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
        send(clientSocket, "Student not found\n", strlen("Student not found\n"), 0);
    }

    close(openFD); // Close the file after use
}

void modifyAccount(int clientSocket) {
off_t recordoff = 0;



    struct account cust, temp;
    int openFD = open("database/customer_database.txt", O_RDWR, 0744); // Open in read-only mode

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
    
    
    
    
    
    
    
    
    
    
    
    

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct customer);

        
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
    strcpy(cus.activation,temp.activation);
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Bank Name: ", cus.bank_name, sizeof(cus.bank_name)) ==-1) {
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
    
   
    
    
    lseek(openFD, -sizeof(cus), SEEK_CUR);
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
}

void activateAccount(int clientSocket){
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
        return;
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

	    if (sendPromptAndReceiveResponse(clientSocket, "Enter Customer Password: ", cust.password, sizeof(cust.password)) == -1) {
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

	    close(openFD);

	    // Optionally, you can send a success message to the client.
	   char successMessage[1024] = "";
	    sprintf(successMessage,"\n************ Customer added successfully ***************\n                           Login ID is %s\n******************************************************\nPress Enter to Move on\n",cust.loginId);
	    send(clientSocket, successMessage, strlen(successMessage), 0);
	    return 1; // Success
}

void checkNumberOfAdmin(int clientSocket) {
	int openFD = open("database/admin_database.txt", O_RDONLY | O_CREAT, 0444);
	if (openFD == -1) {
		perror("read");
		close(clientSocket);
		return;
	}
	
	char buffer[1024] = "";
	memset(buffer, '\0', sizeof(buffer));
	
	struct admin adm;
	lseek(openFD, 0, SEEK_SET);
    
    	struct flock mylock;
        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
    
       fcntl(openFD, F_SETLKW, &mylock);
       
       while (read(openFD, &adm, sizeof(adm)) > 0) {
		char temp[1024] = "";
		memset(temp, '\0', sizeof(temp));
		
		sprintf(temp, ".........................................\nadmin name: %s\npassword: %s\n..............................................\n", adm.loginId, adm.password);
		strcat(buffer, temp);
	}
	if (buffer[0] == '\0') {
		strcpy(buffer, "No Data available");
		buffer[sizeof(buffer)-1] = '\0';
	}
	send(clientSocket, &buffer, sizeof(buffer), 0);
	
	mylock.l_type = F_UNLCK;
	fcntl(openFD, F_SETLKW, &mylock);
	close(openFD);
	
}

bool check_login_session(int clientSocket, struct admin adm) {
	struct session us;
	int openFD = open("database/user_session.txt", O_RDWR | O_CREAT, 0644);

    	if (openFD == -1) {
		perror("Error opening file");
		return false;
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
    while (read(openFD, &us, sizeof(us)) > 0) {
    	if (strcmp(us.loginId, adm.loginId) == 0) {
    		found = true;
		// check timing and also user loggedin or not
		long int curr_time = (long int)time(NULL);
		double elapsedMinutes = (double) (curr_time - us.login_time) / 60.0;
		
		if (us.isActive && elapsedMinutes < 5) {
			// send msg to user that already loggedin
			strcpy(buffer, "Already LoggedIn!");
			send(clientSocket, buffer, sizeof(buffer), 0);
                	mylock.l_type = F_UNLCK;
	                fcntl(openFD, F_SETLKW, &mylock);
        	        close(openFD);
        	        return false;
		} else if (us.isActive && elapsedMinutes >= 5) {
			// allow them to login
			// update the field us.login_time
			us.login_time = curr_time; // Update login time
                	lseek(openFD, -sizeof(us), SEEK_CUR); // Move file pointer back to update the record
                	write(openFD, &us, sizeof(us)); // Update session in the file
	                strcpy(buffer, "Session timed out, relogin successful.");
	                send(clientSocket, buffer, sizeof(buffer), 0);
		} else {
			// allow them to login
			// update the field us.isActive and us.login_time
			us.isActive = true;
		        us.login_time = curr_time;
		        lseek(openFD, -sizeof(us), SEEK_CUR);
		        write(openFD, &us, sizeof(us));
		}
		break;		
    	}
    }
    
    if (!found) {
        struct session newSession;
        strcpy(newSession.loginId, adm.loginId);
        strcpy(newSession.password, adm.password);
        newSession.isActive = true;
        newSession.login_time = (long int)time(NULL); 

        write(openFD, &newSession, sizeof(newSession));
        strcpy(buffer, "New login session created.");
        send(clientSocket, buffer, sizeof(buffer), 0);
        mylock.l_type = F_UNLCK;
	fcntl(openFD, F_SETLKW, &mylock);
	close(openFD);
        return true;
    }

    mylock.l_type = F_UNLCK;
    fcntl(openFD, F_SETLKW, &mylock);
    close(openFD);
    return true;
}

bool checkAdminExist(int clientSocket, struct admin adm) {
    int fd = open("database/admin_database.txt", O_RDONLY);
    
    if (fd == -1) {
        perror("Error opening admin database");
        return false;
    }

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;   
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking the file");
        close(fd);
        return false;
    }

    struct admin tempAdmin;
    bool found = false;
    while (read(fd, &tempAdmin, sizeof(tempAdmin)) > 0) {
        if (strcmp(tempAdmin.loginId, adm.loginId) == 0 && strcmp(tempAdmin.password, adm.password) == 0) {
            found = true;
            break;
        }
    }

    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error unlocking the file");
    }

    close(fd);
    return found;
}

void add_admin_to_database(int clientSocket) {
    struct admin adm;
    memset(adm.loginId, '\0', sizeof(adm.loginId));
    memset(adm.password, '\0', sizeof(adm.password));
    
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee loginId: ", adm.loginId, sizeof(adm.loginId)) == -1) {
        close(clientSocket);
        return;
    }
 
    	
    if (sendPromptAndReceiveResponse(clientSocket, "Enter Employee password: ", adm.password, sizeof(adm.password)) == -1) {
        close(clientSocket);
        return;
    }
    
    int fd = open("database/admin_database.txt", O_RDWR | O_APPEND | O_CREAT, 0644);
    
    if (fd == -1) {
        perror("Error opening admin database");
        return;
    }

    // Initialize file lock
    struct flock lock;
    lock.l_type = F_WRLCK;  // Write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;         // Lock the whole file
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking the file");
        close(fd);
        return;
    }

    if (write(fd, &adm, sizeof(adm)) == -1) {
        perror("Error writing to the file");
    }

    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error unlocking the file");
    }

    close(fd);
}



