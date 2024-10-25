#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <time.h>
#include "admin.h"

// returns current date and time
void getCurrentDateTime(char *dateTimeStr, int maxSize) {
    time_t now = time(NULL); // Get the current time
    struct tm *localTime = localtime(&now); // Convert to local time

    // Format the date and time as "YYYY-MM-DD HH:MM:SS"
    strftime(dateTimeStr, maxSize, "%Y-%m-%d %H:%M:%S", localTime);
}


void generateRandomID(char *id, int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int charsetSize = sizeof(charset) - 1; // Exclude the null terminator

    // Seed the random number generator
    srand(time(0));

    // Generate the random string
    for (int i = 0; i < length; i++) {
        int randomIndex = rand() % charsetSize;
        id[i] = charset[randomIndex];
    }
    id[length] = '\0'; // Null-terminate the string
}




bool account=false;
struct userCred temporary3;
struct account_number_store{

   char from_account_no[50];
   char to_account_no[50];

};
struct account_number_store storing;




void viewAccountBalance(int clientSocket);
void depositMoney(int clientSocket);
void withdrawMoney(int clientSocket);
void transferFunds(int clientSocket);
void applyForLoan(int clientSocket);
void changePassword(int clientSocket);
void addFeedback(int clientSocket);
void viewTransactionHistory(int clientSocket);
void viewTransactionHistory1(int clientSocket);



bool customer_authenticate(int clientSocket){
struct customer cust, temp;
int openFD = open("database/customer_database.txt", O_RDWR, 0744); // Open in read-only mode
int recordoff=0;
    if (openFD == -1) {
        perror("Error opening file");
        return false;
    }

    bool found = false; // Initialize found to false
    char buffer[1024]; // Declare buffer for sending data
    memset(buffer, '\0', sizeof(buffer));
    
    
      

    send(clientSocket, "Enter Customer ID: ", strlen("Enter Customer ID: "), 0);
    int readResult = read(clientSocket, cust.loginId, sizeof(cust.loginId) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Admin ID from server", strlen("Error receiving Admin ID from server"), 0);
        return false;
    }
    cust.loginId[readResult] = '\0';
    
    if(isOnline(cust.loginId)){
    
         send(clientSocket, "ID Already Logged in\n", strlen("ID Already Logged in\n"), 0);
         return false;
    
    }
    
    
    send(clientSocket, "Password\n", strlen("Password\n"), 0);
     readResult = read(clientSocket, cust.password, sizeof(cust.password) - 1);

    if (readResult <= 0) {
        send(clientSocket, "Error receiving Password from server", strlen("Error receiving Password from server"), 0);
        return false;
    }
    cust.password[readResult] = '\0';
        
   
   
    

    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct customer);

  
        //This checks whther my login id is matching or not
        if (   strcmp(cust.loginId, temp.loginId) == 0) 
        {
        
        
         if (   strcmp(cust.password, temp.password) != 0)
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
               memset(&temporary3,'\0',sizeof(temporary3));    
    
   	   strcpy(temporary3.loginId, temp.loginId);
   	   strcpy(temporary3.password, temp.password);
   	   add_session(temporary3.loginId);
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





void logout2(int clientSocket){

   remove_session(temporary3.loginId);

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
        
      //printf("\nHello I'm in customer\n");  
      //printf("\nCustomer: %s    %s", temporary3.loginId, temporary3.password);

        int choice = atoi(readbuff);
         //send(clientSocket, readbuff, sizeof(readbuff), 0);

        switch (choice) {
            case 1:viewAccountBalance(clientSocket);
         //printf("\nCustomer: %s    %s", temporary3.loginId, temporary3.password);
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
          	viewTransactionHistory1(clientSocket);
          	//printf("activated\n");
            break;
            
          case 9:
          	logout2(clientSocket);
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







void entry_to_transaction_database(int clientSocket,struct transaction temp){

    //get the current date and time.
    char currentDateTime[20]; // Buffer to store the formatted date and time
    getCurrentDateTime(currentDateTime, sizeof(currentDateTime));
    strcpy(temp.date_time, currentDateTime);
      
    
    int idLength = 10; // Length of the random ID
    char randomID[11]; // Buffer to hold the generated ID (length + 1 for the null terminator)
    // Call the function to generate a random ID
    generateRandomID(randomID, idLength);
    strcpy(temp.transaction_id, randomID);
    
    
    // The data is prepared now, Just push the data to the end of the transaction database. 
    
    int openFD = open("database/transaction_database.txt", O_RDWR | O_CREAT | O_APPEND, 0744); // Open the file in append mode

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
    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));

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



void depositMoney(int clientSocket){
  
  //Step0: Get the amount of money to deposited
  char readbuff1[15];
  char balance[15];
  int readResult;
  
  
  
        
  
  
  /*     step1: Balance In account database to be updated:
         a) Make write lock on the file and the record when the id matches.  
         b) get the balance information and update the balance by adding it, then pass the structure to the transaction. 
         c) Transaction will print properly.
     
  */  
  bool flag=false;
  int recordoff=0, prev_balance=0,new_balance=0;
  struct  account acc, temp;
  int openFD = open("database/account_database.txt", O_RDWR, 0744); // Open in read-only mode
struct flock mylock;
strcpy(acc.loginId, temporary3.loginId);
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

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
    if (strcmp(acc.loginId, temp.loginId) == 0 && strcmp(temp.activation,"a")==0) 
       {   
           flag = true;
           
           prev_balance = atoi(temp.balance);
           
           //printf("Hi you are depositing money\n");
send(clientSocket, "Amount to deposit:  ", strlen("Amount to deposit:  "), 0);
readResult = read(clientSocket, readbuff1, sizeof(readbuff1) - 1);

    if (readResult <= 0) 
    {
send(clientSocket, "Recieving Deposit Money Error\n", strlen("Recieving Deposit Money Error\n"), 0);
  return;
    }
    readbuff1[readResult] = '\0';
    int deposit_money = atoi(readbuff1);
           
           
           
           
           
           new_balance = prev_balance+deposit_money;
           memset(balance, '\0', sizeof(balance));
           sprintf(balance,"%d", new_balance);
           
           strcpy(temp.balance, balance);
           
           lseek(openFD, -sizeof(temp), SEEK_CUR);
           ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
           
	    if (bytes_written == -1) {
	      perror("write");
	      send(clientSocket,"Balance Error\n",strlen("Balance Error \n"),0);
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
        if(flag==true){
    char buff[1024],ball[15];
    
    
    sprintf(ball, "%d", prev_balance);
    
    
    snprintf(buff,sizeof(buff) ,"\nSuccefully Deposited Money: %s\nPreviously The Balance was: %s\nNew Balance is: %s\nEntering Transaction database\nEnter to proceed\n",readbuff1,ball, temp.balance);    
    send(clientSocket,buff,sizeof(buff),0);
    break;
       }
        
       
    
    recordoff += sizeof(struct account);
}
close(openFD);
   /*
   Step3: Now it time to make the transaction and pass it to the function , it will push the things to the transaction database finally. 
   */
   if(flag){
   struct transaction t1; 
   memset(&t1,'\0',sizeof(t1));
   strcpy(t1.from_customer_id, temporary3.loginId);
   strcpy(t1.from_account_no, temp.account_number);
   strcpy(t1.to_account_no, temp.account_number);
   strcpy(t1.transaction_amount, readbuff1);
   strcpy(t1.description_to_transaction, "deposit +");
   strcpy(t1.balance_from_account_after_transaction, temp.balance);
   
   
   entry_to_transaction_database(clientSocket,t1);
   }
   
    if(flag==false){
  
     send(clientSocket,"Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n",strlen("Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n"),0);
  }
   
   return; 
    
  

};


/*

struct account_number_store{

   char from_account_no[50];
   char to_account_no[50];

};

*/







bool get_account_number(struct userCred S,int num){
/*
get the account number for the sent account id, in account database, just the read lock. 
store it in the place mentioned
*/
  int readResult=-1;
  int recordoff=0;
  struct  account  temp;
  struct flock mylock;
  bool flag=false;
  int openFD = open("database/account_database.txt", O_RDONLY, 0744); // Open in read-only mode

while (read(openFD, &temp, sizeof(temp)) > 0) 
{

        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct account);
         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        
        //This checks whther my login id is matching or not
    if (strcmp(S.loginId, temp.loginId) == 0 && strcmp(temp.activation,"a")==0) 
       {   flag = true; 
    
	if(num==1)
	    strcpy( storing.from_account_no, temp.account_number);
	    
	else
	    strcpy( storing.to_account_no, temp.account_number);
	
       }
        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }
       
    
    recordoff += sizeof(struct account);
}
return flag;
close(openFD);
};



void increase_amount_on_to_account(int clientSocket ,int amount_to_transfer,struct userCred to_customer){




//Step0: Get the amount of money to deposited
  char readbuff1[15];
  char balance[15];
  int readResult;
  
  
  
        
  
  
  /*     step1: Balance In account database to be updated:
         a) Make write lock on the file and the record when the id matches.  
         b) get the balance information and update the balance by adding it, then pass the structure to the transaction. 
         c) Transaction will print properly.
     
  */  
  bool flag=false;
  int recordoff=0, prev_balance=0,new_balance=0;
  struct  account acc, temp;
  int openFD = open("database/account_database.txt", O_RDWR, 0774); // Open in read-only mode
struct flock mylock;
strcpy(acc.loginId, to_customer.loginId);
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

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
    if (strcmp(acc.loginId, temp.loginId) == 0 && strcmp(temp.activation,"a")==0) 
       {   
           flag = true;
           
           prev_balance = atoi(temp.balance);
           
           //printf("Hi you are depositing money\n");

    int deposit_money = amount_to_transfer;
     snprintf(readbuff1,sizeof(readbuff1),"%d", amount_to_transfer);      
           
           
           
           
           new_balance = prev_balance+deposit_money;
           memset(balance, '\0', sizeof(balance));
           
           
           snprintf(balance,sizeof(balance),"%d", new_balance);
           
           strcpy(temp.balance, balance);
           lseek(openFD, -sizeof(temp), SEEK_CUR);
           ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
           
	    if (bytes_written == -1) {
	      perror("write");
	      send(clientSocket,"Balance Error\n",strlen("Balance Error \n"),0);
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
        if(flag==true){
    char buff[1024],ball[15];
    
    
    sprintf(ball, "%d", prev_balance);
    
    
    snprintf(buff,sizeof(buff) ,"\nSuccefully Recieved Money: %d\nPreviously The Balance was: %s\nNew Balance is: %s\nEntering Transaction database\nEnter to proceed\n",amount_to_transfer,ball, temp.balance);    
    send(clientSocket,buff,sizeof(buff),0);
    break;
       }
        
       
    
    recordoff += sizeof(struct account);
}
close(openFD);
   /*
   Step3: Now it time to make the transaction and pass it to the function , it will push the things to the transaction database finally. 
   */
   if(flag){
   struct transaction t1; 
   memset(&t1,'\0',sizeof(t1));
   strcpy(t1.from_customer_id, acc.loginId);
   strcpy(t1.from_account_no, storing.from_account_no);
   strcpy(t1.to_account_no, storing.to_account_no);
   strcpy(t1.transaction_amount, readbuff1);
   strcpy(t1.description_to_transaction, "Transferred +");
   strcpy(t1.balance_from_account_after_transaction, temp.balance);
   
   
   entry_to_transaction_database(clientSocket,t1);
   }
   
    if(flag==false){
  
     send(clientSocket,"Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n",strlen("Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n"),0);
  }
   
   return; 
    
  











};

























int reduce_from_account(int clientSocket, struct userCred from_customer){


/*
get the account number for bot
*/
  //Step0: Get the amount of money to deposited
  char readbuff1[15];
  char balance[15];
  int readResult=-1;
  int withdraw_money;
  

     
  /*     step1: Balance In account database to be updated:
         a) Make write lock on the file and the record when the id matches.  
         b) get the balance information and update the balance by adding it, then pass the structure to the transaction. 
         c) Transaction will print properly.
     
  */  
  bool flag=false,dotransaction=false;
  int recordoff=0, prev_balance=0,new_balance=0;
  struct  account acc, temp;
  struct flock mylock;
  int openFD = open("database/account_database.txt", O_RDWR, 0744); // Open in read-only mode

strcpy(acc.loginId, from_customer.loginId);
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

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
    if (strcmp(acc.loginId, temp.loginId) == 0 && strcmp(temp.activation,"a")==0) 
       {   
           flag = true;
           
           
           prev_balance = atoi(temp.balance);           
           if(prev_balance<=0){
           
             send(clientSocket,"Sorry You Do not have Sufficient Balance\nCurrent Balance: 0000\n",strlen("Sorry You Do not have Sufficient Funds\nCurrent Balance: 0000\n"),0);
             mylock.l_type = F_UNLCK;
             if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }    
             
             break;
             
           }
           dotransaction=true;
           
             //printf("Hi you are depositing money\n");
             char withbuff[200];
             memset(withbuff,'\0',sizeof(withbuff));
             
             snprintf(withbuff,sizeof(withbuff) ,"Current Balance: %s\nEnter Amount to transfer:  ",temp.balance);
                          
             send(clientSocket, withbuff, sizeof(withbuff), 0);
             readResult = read(clientSocket, readbuff1, sizeof(readbuff1) - 1);
             if (readResult <= 0) 
                {
                 send(clientSocket, "Recieving transfer Money Error\n", strlen("Recieving transfer Money Error\n"), 0);
                 return -1;
                 }
    
    readbuff1[readResult] = '\0';
    withdraw_money = atoi(readbuff1);
           
           
           
           
           
           
           new_balance = prev_balance - withdraw_money;
           memset(balance, '\0', sizeof(balance));
           sprintf(balance,"%d", new_balance);
           
           strcpy(temp.balance, balance);
           
           lseek(openFD, -sizeof(temp), SEEK_CUR);
           ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
           
	    if (bytes_written == -1) {
	      perror("write");
	      send(clientSocket,"Balance Error\n",strlen("Balance Error \n"),0);
	      close(openFD);
	      close(clientSocket);
	        return -1;
	    }
           
           

        }
    

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }
        
        
        if(flag==true){
    char buff[1024],ball[15];
    
    sprintf(ball, "%d", prev_balance);
    
    snprintf(buff,sizeof(buff) ,"\nSuccefully Transferred Money: %s\nPreviously The Balance was: %s\nNew Balance is: %s\nEntering Transaction database\nEnter to proceed\n",readbuff1,ball, temp.balance);    
    send(clientSocket,buff,sizeof(buff),0);
    break;
       }
        
       
    
    recordoff += sizeof(struct account);
}

close(openFD);
   /*
   Step3: Now it time to make the transaction and pass it to the function , it will push the things to the transaction database finally. 
   */
   
   if(dotransaction)
 {
   struct transaction t1; 
   memset(&t1,'\0',sizeof(t1));
   strcpy(t1.from_customer_id, temporary3.loginId);
   strcpy(t1.from_account_no, storing.from_account_no);
   strcpy(t1.to_account_no, storing.to_account_no);
   strcpy(t1.transaction_amount, readbuff1);
   strcpy(t1.description_to_transaction, "Transfer -");
   strcpy(t1.balance_from_account_after_transaction, temp.balance);
   
   
   entry_to_transaction_database(clientSocket,t1);
   
    
  }
  if(flag==false){
  
     send(clientSocket,"Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n",strlen("Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n"),0);
  }
  return withdraw_money; 


}






































void transferFunds(int clientSocket)
{
   int readResult=-1;
   struct userCred to_customer, from_customer;
   memset(&to_customer,'\0',sizeof(to_customer));
   memset(&from_customer,'\0',sizeof(from_customer));
   send(clientSocket, "Enter the Empid to whom The money to be sent:   ", strlen("Enter the Empid to whom The money to be sent:   "),0);
   readResult = read(clientSocket, to_customer.loginId, sizeof(to_customer.loginId) - 1);
   to_customer.loginId[readResult] = '\0';
   strcpy(from_customer.loginId, temporary3.loginId);
   
   
   
   
   
   
   
   if(strcmp(from_customer.loginId,to_customer.loginId)==-1){
   
      //lock from customer id account if not blocked
      //lock to customer id account record if not blocked
   }
   else if((strcmp(from_customer.loginId,to_customer.loginId)== 1)){
   
      //lock to customer id if not blocked
      //lock from customer id if not blocked
   }
   else{
   
send(clientSocket, "Invalid action\nThis is used to transfer between 2 different accounts\nEnter to move on\n", strlen("Invalid action\nThis is used to transfer between 2 different accounts\nEnter to move on\n"),0);
   
   }
   
   bool fggg = get_account_number(from_customer,1);
   if(!fggg){
      
send(clientSocket, "My account is blocked\n", strlen("My account is blocked\n"),0);
      
   };
   
   fggg = get_account_number(to_customer,0);
   
    if(!fggg){
   
send(clientSocket, "To account is blocked\n", strlen("To account is blocked\n"),0);   
   
    };
   
     
     
     
     
     
     
     
     
     
     
     
   
   if(fggg){
   
   int amount_to_transfer = reduce_from_account(clientSocket, from_customer);
   if(amount_to_transfer == -1){
   
send(clientSocket, "There is no sufficient amount to transfer\n", strlen("There is no sufficient amount to transfer\n"),0);

   }
   
   
   else{
   increase_amount_on_to_account(clientSocket ,amount_to_transfer,to_customer);
   }
   
   }
   
   
   
   
   
}



















void withdrawMoney(int clientSocket){
  
  //Step0: Get the amount of money to deposited
  char readbuff1[15];
  char balance[15];
  int readResult=-1;
  

     
  /*     step1: Balance In account database to be updated:
         a) Make write lock on the file and the record when the id matches.  
         b) get the balance information and update the balance by adding it, then pass the structure to the transaction. 
         c) Transaction will print properly.
     
  */  
  bool flag=false,dotransaction=false;
  int recordoff=0, prev_balance=0,new_balance=0;
  struct  account acc, temp;
  struct flock mylock;
  int openFD = open("database/account_database.txt", O_RDWR, 0744); // Open in read-only mode

strcpy(acc.loginId, temporary3.loginId);
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

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
    if (strcmp(acc.loginId, temp.loginId) == 0 && strcmp(temp.activation,"a")==0) 
       {   
           flag = true;
           
           
           prev_balance = atoi(temp.balance);           
           if(prev_balance<=0){
           
             send(clientSocket,"Sorry You Do not have Sufficient Balance\nCurrent Balance: 0000\n",strlen("Sorry You Do not have Sufficient Funds\nCurrent Balance: 0000\n"),0);
             mylock.l_type = F_UNLCK;
             if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }    
             
             break;
             
           }
           dotransaction=true;
           
             //printf("Hi you are depositing money\n");
             char withbuff[200];
             memset(withbuff,'\0',sizeof(withbuff));
             
             snprintf(withbuff,sizeof(withbuff) ,"Current Balance: %s\nEnter Amount to withdraw:  ",temp.balance);
                          
             send(clientSocket, withbuff, sizeof(withbuff), 0);
             readResult = read(clientSocket, readbuff1, sizeof(readbuff1) - 1);
             if (readResult <= 0) 
                {
                 send(clientSocket, "Recieving Withdraw Money Error\n", strlen("Recieving Withdraw Money Error\n"), 0);
                 return;
                 }
    
    readbuff1[readResult] = '\0';
    int withdraw_money = atoi(readbuff1);
           
           
           
           
           
           
           new_balance = prev_balance - withdraw_money;
           memset(balance, '\0', sizeof(balance));
           sprintf(balance,"%d", new_balance);
           
           strcpy(temp.balance, balance);
           
           lseek(openFD, -sizeof(temp), SEEK_CUR);
           ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
           
	    if (bytes_written == -1) {
	      perror("write");
	      send(clientSocket,"Balance Error\n",strlen("Balance Error \n"),0);
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
        if(flag==true){
    char buff[1024],ball[15];
    
    sprintf(ball, "%d", prev_balance);
    
    snprintf(buff,sizeof(buff) ,"\nSuccefully Withdraw Money: %s\nPreviously The Balance was: %s\nNew Balance is: %s\nEntering Transaction database\nEnter to proceed\n",readbuff1,ball, temp.balance);    
    send(clientSocket,buff,sizeof(buff),0);
    break;
       }
        
       
    
    recordoff += sizeof(struct account);
}

close(openFD);
   /*
   Step3: Now it time to make the transaction and pass it to the function , it will push the things to the transaction database finally. 
   */
   
   if(dotransaction)
 {
   struct transaction t1; 
   memset(&t1,'\0',sizeof(t1));
   strcpy(t1.from_customer_id, temporary3.loginId);
   strcpy(t1.from_account_no, temp.account_number);
   strcpy(t1.to_account_no, temp.account_number);
   strcpy(t1.transaction_amount, readbuff1);
   strcpy(t1.description_to_transaction, "withdraw -");
   strcpy(t1.balance_from_account_after_transaction, temp.balance);
   
   
   entry_to_transaction_database(clientSocket,t1);
   
    
  }
  if(flag==false){
  
     send(clientSocket,"Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n",strlen("Your Account Blocked\nContact Admin or Manager\nPress ENTER to move on\n"),0);
  }
  return; 

};







// changing applied loan status to y
void change_the_applied_loan_status_customer(int clientSocket,char *s, char *yn){

  struct customer c1,temp;
  memset(&c1,'\0',sizeof(c1));
  memset(&temp,'\0', sizeof(temp));
  
  int recordoff = 0, readResult=-1;
  strcpy(c1.loginId, s);
  int openFD = open("database/customer_database.txt", O_RDWR, 0774); // Open the file in append mode

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
        mylock.l_len = sizeof(struct customer);
         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        
        //This checks whther my login id is matching or not
        
    if (strcmp(c1.loginId, temp.loginId) == 0) 
       {            
         strcpy(temp.applied_for_loan,yn);  
    lseek(openFD, -sizeof(struct customer), SEEK_CUR);  
    
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
       
    
    recordoff += sizeof(struct customer);
}// end of while loop  

close(openFD);
  


}















void applyForLoan(int clientSocket){

 /*
 Nothing else here just keep on asking the questions and get teh input from the client, finally push inton the database at the last and check. 
 */
 bool applyloan = false;
 struct loan lo;
 memset(&lo, '\0', sizeof(lo));
 strcpy(lo.status,"pen");
 strcpy(lo.empId,"null");
 strcpy(lo.userId, temporary3.loginId);
 strcpy(lo.read_by_manager,"n");
 strcpy(lo.processed,"n");
 
 
 
 
 int idLength = 10; // Length of the random ID
    char randomID[11]; // Buffer to hold the generated ID (length + 1 for the null terminator)
    // Call the function to generate a random ID
    generateRandomID(randomID, idLength);    
    strcpy(lo.loanid,randomID);   
    
    
 
 
  if (sendPromptAndReceiveResponse(clientSocket, "Describe yourslef Format(this decides your loan agrement):\n(Salary(pm): value \tAsset[weight in terms of current market in rupees]): value \tCS[Credit score]: value\tLF [Loan For]: Description\t Loan Amount: Value\n", lo.description_to_grant_loan, sizeof(lo.description_to_grant_loan)) == -1) {
        close(clientSocket);
        return;
    }
    
    
    
// all the things are ready, Just push the things into loan database
 int openFD = open("database/loan_database.txt", O_RDWR | O_CREAT | O_APPEND, 0744); // Open the file in append mode

    if (openFD == -1) {
        perror("open");
        close(clientSocket);
        close(openFD);
        return;
    }


struct flock mylock;
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = 0;
        mylock.l_len = 0;
        mylock.l_pid = getpid();
        
        
         
        
    fcntl(openFD, F_SETLKW, &mylock);
    lseek(openFD, 0, SEEK_END);
    ssize_t bytes_written = write(openFD, &lo, sizeof(lo));

    if (bytes_written == -1) {
        perror("write");
        close(openFD);
        close(clientSocket);
        return;
    }
    
    
    
    mylock.l_type = F_UNLCK;
    fcntl(openFD, F_SETLK, &mylock);
    // change the customer applied loan attribute to y
    change_the_applied_loan_status_customer(clientSocket,lo.userId,"y");
    
    close(openFD);
    return;
 
};





















void addFeedback(int clientSocket){

  /*
  step 1. Get the employee id for my loan application.
  step2. Give properly the empolyee id as input
  step3. give feedback, that will be written to the database of loan
    
  */
 
  // step1: Employee id
  struct flock mylock;
  int recordoff = 0, readResult=-1;
  struct loan cust, temp;
  memset(&cust,'\0',sizeof(cust));
  memset(&temp,'\0',sizeof(temp));
  strcpy(cust.userId, temporary3.loginId);
  bool emp_assn=false, pen=true, f = false;
  
  char buffer[1024], tempbuf[1024];
  memset(buffer,'\0',sizeof(buffer));
  memset(tempbuf,'\0',sizeof(tempbuf));
  
  
  int openFD = open("database/loan_database.txt", O_RDWR, 0744); // Open the file in append mode

    if (openFD == -1) {
        perror("open");
        send(clientSocket, "Database is not exist\n", sizeof("Database is not exist\n"),0);
        close(clientSocket);
        close(openFD);
        return;
    }
    
    
    
    
    
    
   
   
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
    if (strcmp(cust.userId, temp.userId) == 0) 
       {   
           f=true;
           if(strcmp(temp.empId,"null")==0) continue;
           emp_assn = true;
           
           
           if(strcmp(temp.status,"pen")==0) continue;
           pen = false;
           
           
           
           
           memset(tempbuf,'\0',sizeof(tempbuf));           
           snprintf(tempbuf,sizeof(tempbuf),"\nEmpid: %s\nDescription: %s\nLoan Application Status:%s\n\nGive Feedback:  ",temp.empId,temp.status, temp.description_to_grant_loan);
           
           send(clientSocket, tempbuf, sizeof(tempbuf),0);
           readResult = read(clientSocket, temp.feedback_from_customer_about_employee, sizeof(temp.feedback_from_customer_about_employee)-1);

    if (readResult <= 0) {
        send(clientSocket, "Feedback error\n", strlen("Feedback error\n"), 0);
        return ;
    }
    temp.feedback_from_customer_about_employee[readResult] = '\0';
    lseek(openFD, -sizeof(temp), SEEK_CUR);  
    
    ssize_t bytes_written = write(openFD, &temp, sizeof(temp));
    if (bytes_written == -1) {
        perror("write");
        send(clientSocket,"Feedback err\n", strlen("Feedback err\n"),0);
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
       
    
    recordoff += sizeof(struct loan);
}// end of while loop
    
    
    if(pen == true)
    {
       send(clientSocket, "Pending loan\nPress Enter to Move on\n", strlen("Pending loan\nPress Enter to Move on\n"),0);
    }
    
    if(emp_assn==false){
      send(clientSocket, "Employee Not assigned\nPress Enter to move on\n", strlen("Employee Not assigned\nPress Enter to move on\n"),0);
    }
    
    
    if(f==false){
      send(clientSocket, "If applied, then rejected and removed\nElse Not applied\nPress Enter to move on\n", strlen("If applied, then rejected and removed\nElse Not applied\nPress Enter to move on\n"),0);
    }
    

close(openFD);

  
};// end of add feedback_from_customer_about_employee




































void viewAccountBalance(int clientSocket){ 
   
   
struct  account acc, temp;
int openFD = open("database/account_database.txt", O_RDONLY, 0744); // Open in read-only mode

char buffer[1024],tempbuf[1024]; // Declare buffer for sending data
memset(buffer, '\0', sizeof(buffer));
    

strcpy(acc.loginId, temporary3.loginId);
struct flock mylock;   
int recordoff=0;  
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_RDLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct account);
         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        
        //This checks whther my login id is matching or not
   if (strcmp(acc.loginId, temp.loginId) == 0) 
       {
       
   snprintf(buffer,sizeof(buffer), "\nBalance: %s\n",temp.balance);
     

        }
    

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }
        
    
    recordoff += sizeof(struct account);
}

    strcat(buffer,"Press Enter to move on\n");    
    send(clientSocket,buffer,sizeof(buffer),0);
    
    
   
    close(openFD); // Close the file after use
    return; 
    
   
   
   
   
};












































void changePassword(int clientSocket){
    
    
    struct customer cust,temp;
    memset(&temp, '\0',sizeof(temp));
    memset(&cust, '\0',sizeof(cust));
    
    strcpy(temp.loginId,temporary3.loginId);
    
    // now the data is ready, but we shall change it. 
    
    
    int openFD = open("database/customer_database.txt", O_RDWR, 0744); // Open in read-only mode
    int recordoff=0,readResult=-1;
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

         // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        //This checks whther my login id is matching or not
        if (   strcmp(cust.loginId, temp.loginId) == 0) 
        {
        
         // if the login id matched but the password is not matched, then
         found = true;
       
        
           
             if (found) 
             {
               // then change the changed password to be written at this place
send(clientSocket, "Passowrd\n", strlen("Passowrd\n"), 0);
readResult = read(clientSocket, temp.password, sizeof(temp.password) - 1);

    if (readResult <= 0) 
    {
send(clientSocket, "Error receiving Password Customer ID in Manger from server\n", strlen("Error receiving Password Customer ID in Manger from server\n"), 0);
  return;
    }
    
    temp.password[readResult] = '\0';
    strcpy(cust.password, temp.password);
    
    
    lseek(openFD, -sizeof(cust), SEEK_CUR);
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
send(clientSocket, "Could not find account\n", strlen("Could not find account\n"),0);
    }
    
    

    close(openFD); // Close the file after use
    return;
       
}

















































void viewTransactionHistory(int clientSocket){

  int readResult=0;
  
  struct  transaction cust, temp;
int openFD = open("database/transaction_database.txt", O_RDWR, 0744); // Open in read-only mode
int recordoff=0;
    if (openFD == -1) {
        perror("Error opening file");
        return ;
    }

    bool found = false; // Initialize found to false
    char buffer[1024],tempbuf[1024]; // Declare buffer for sending data
    memset(buffer, '\0', sizeof(buffer));
    
    
    
    
/* 
The below is taking login id and password from the client
*/    

    strcpy(cust.from_customer_id, temporary3.loginId);
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;
    
    
while (read(openFD, &temp, sizeof(temp)) > 0) 
     {

        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct transaction);
// Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }
        
        //This checks whther my login id is matching or not
   if (strcmp(cust.from_customer_id, temp.from_customer_id) == 0) 
       {
       
        found = true;
        memset(tempbuf,'\0',sizeof(tempbuf));
          
           
 if (found) 
    {
     
   snprintf(tempbuf,sizeof(tempbuf), "\nFrom Account NO: %s\nTo account Number: %s\nDate Time: %s\nTransaction Amount: %s\nDescription: %s\nBalance :%s\n",temp.from_account_no, temp.to_account_no, temp.date_time, temp.transaction_amount, temp.description_to_transaction, temp.balance_from_account_after_transaction);
   
    
       strcat(buffer,tempbuf);
       
        
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

    strcat(buffer,"\nPress Enter to move on\n");    
    send(clientSocket,buffer,sizeof(buffer),0);
    
    if(found==false) {
        send(clientSocket, "No entry in transaction\n", strlen("No entry in transaction\n"),0);
    }

    close(openFD); // Close the file after use
    return; 
    
   
  
};




void viewTransactionHistory1(int clientSocket) {
    int readResult = 0;

    struct transaction cust, temp;
    int openFD = open("database/transaction_database.txt", O_RDWR, 0744); // Open in read-only mode
    int recordoff = 0;
    if (openFD == -1) {
        perror("Error opening file");
        return;
    }

    bool found = false; // Initialize found to false
    char buffer[4096], tempbuf[1024]; // Increase buffer size for large data
    memset(buffer, '\0', sizeof(buffer));

    /* 
    The below is taking login id and password from the client
    */
    strcpy(cust.from_customer_id, temporary3.loginId);
    // Reset the file pointer to the beginning of the file
    lseek(openFD, 0, SEEK_SET);
    struct flock mylock;

    // Add table headers to the buffer
    snprintf(buffer, sizeof(buffer), "%-15s %-15s %-20s %-15s %-30s %-15s\n",
             "From Account", "To Account", "Date Time", "Amount", "Description", "Balance");
    strcat(buffer, "----------------------------------------------------------------------------------------------\n");

    while (read(openFD, &temp, sizeof(temp)) > 0) {
        mylock.l_type = F_WRLCK;
        mylock.l_whence = SEEK_SET;
        mylock.l_start = recordoff;
        mylock.l_len = sizeof(struct transaction);

        // Lock the record
        if (fcntl(openFD, F_SETLKW, &mylock) == -1) {
            perror("Error locking record");
            exit(1);
        }

        // This checks whether my login id is matching or not
        if (strcmp(cust.from_customer_id, temp.from_customer_id) == 0) {
            found = true;
            memset(tempbuf, '\0', sizeof(tempbuf));

            // Format the output for tabular representation
            snprintf(tempbuf, sizeof(tempbuf), "%-15s %-15s %-20s %-15s %-30s %-15s\n",
                     temp.from_account_no, temp.to_account_no, temp.date_time, 
                     temp.transaction_amount, temp.description_to_transaction, 
                     temp.balance_from_account_after_transaction);

            strcat(buffer, tempbuf);
        }

        // Unlock the record after processing
        mylock.l_type = F_UNLCK;
        if (fcntl(openFD, F_SETLK, &mylock) == -1) {
            perror("Error unlocking record");
            exit(1);
        }

        recordoff += sizeof(struct transaction);
    }

    strcat(buffer, "\nPress Enter to move on\n");
    send(clientSocket, buffer, strlen(buffer), 0);

    if (!found) {
        send(clientSocket, "No entry in transaction\n", strlen("No entry in transaction\n"), 0);
    }

    close(openFD); // Close the file after use
    return;
}




