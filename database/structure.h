#include <sys/sem.h>
#include <sys/shm.h>


struct Session {
    char userid[100];
    int sessionid;
};

// Global variables
struct Session *activeSessions;
int *sessionCount;
int shm_id;
int sem_id;

struct sembuf sem_lock = {0, -1, 0}; 
struct sembuf sem_unlock = {0, 1, 0};

// Function to lock semaphore
void lock() {
    semop(sem_id, &sem_lock, 1);
}

// Function to unlock semaphore
void unlock() {
    semop(sem_id, &sem_unlock, 1);
}

// Function to check if a user is online
bool isOnline(char *userid) {
    lock();
    for (int i = 0; i < *sessionCount; i++) {
        if (strcmp(activeSessions[i].userid, userid) == 0) {
            unlock();
            return true;
        }
    }
    unlock();
    return false;
}

// Function to add a new session
int add_session(char *userid) {
    lock();
    int sessionid = rand();  
    strcpy(activeSessions[*sessionCount].userid, userid);
    activeSessions[*sessionCount].sessionid = sessionid;
    (*sessionCount)++;  
    unlock();
    return sessionid;
}

// Function to remove a session
void remove_session(char *userid) {
    lock();
    for (int i = 0; i < *sessionCount; i++) {
        if (strcmp(activeSessions[i].userid, userid) == 0) {
            // Shift sessions down to fill the gap
            for (int j = i; j < (*sessionCount) - 1; j++) {
                activeSessions[j] = activeSessions[j + 1];
            }
            (*sessionCount)--;
            break;
        }
    }
    unlock();
}

// Function to initialize shared memory and semaphore
void initialize_shared_resources() {
    // Create shared memory for 100 sessions and an integer for session count
    shm_id = shmget(IPC_PRIVATE, sizeof(struct Session) * 100 + sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    activeSessions = shmat(shm_id, NULL, 0);
    if (activeSessions == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // Set the sessionCount pointer to the shared memory location after the sessions
    sessionCount = (int *)((char *)activeSessions + sizeof(struct Session) * 100);
    *sessionCount = 0;

    // Create a semaphore for synchronization
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (sem_id < 0) {
        perror("semget");
        exit(1);
    }

    // Initialize semaphore to 1 (unlocked state)
    if (semctl(sem_id, 0, SETVAL, 1) < 0) {
        perror("semctl");
        exit(1);
    }
}
    






// takes dummy inout during logging in 
// admin is only ine who do not need anythin to store
struct userCred{
char loginId[50];
char password[50];

};

struct admin{ // done 

char loginId[50];
char password[50];
};

// Database customer
//customer attributes
struct customer{
// done
char loginId[50];
char password[50];
char name[50];
char age[5];
char emailAddress[50];
char mobile[11];

// this is used for loan recovery
char address[100];

// n(not applied for loan),y(yes applied for loan)
char applied_for_loan[2];

};




// database employee
struct employee{// done

char loginId[50];
char password[50];
char name[50];
//dob
char age[15];
char emailAddress[50];
char mobile[11];
//date of joining 
char date_of_joining[30];
/*
emp -  employee
mng -  manager
*/
char position[4];
//the login id of the some employee is pasted here
// usually this is stored
char managerId[50];
//y,n 
char assigned_for_loan[2];
// Adress of the employee
char address[100];

};


// database loan
struct loan{  //done
char loanid[30];
//by default this must be equal to null, later empid id will be filled by manager
char empId[50];
char userId[50];

/*
acc - loan granted
pen - loan application is pending
rej - loan application rejected, employee rejected
rev - loan revoked, I do not want loan any more
ret - loan repayed, so no need to make entry here 

After every process except, granted and pending, the recod to be removed.

*/
char status[4];
// based on this descption, the loan is granted on not
char description_to_grant_loan[200];
char feedback_from_customer_about_employee[100];
/*
by default is set to 
n - admin not read my feedback_from_customer_about_employee
y - admin read the feedback_from_customer_about_employee
*/
char read_by_manager[2];

/*
by default is set to 
n - not processed
y - processed
o - procedded but not valid. 

This is nothing but the verification process. 
Check the address is valid.
Salary slip is valid.
Assets he has mentioned is valid or not, are they weighing the money to make the loan.
*/


char processed[2];

};


// database account 
struct account{ //done
// this can be used by both the user and employee, so 
char loginId[50];
//char password[50];
/*
I'm nullifying pasword here, once the user is logged in with the password and username, there is no need for it again and again.
Just the entry of the login ID is required, jsu becuse  
*/
char account_number[50];
char account_holder_name[50];
char bank_name[50];
char branch_name[50];
char balance[15];
char date_of_opening[50];

/*
d- deactivated
a - active
*/
char activation[2];

};

// database transaction
struct transaction{// done

char transaction_id[50];
char from_customer_id[50];
char from_account_no[50];
char to_account_no[50];
char date_time[50];
// s-saving, c- current
char transaction_amount[15];
char description_to_transaction[100];
char balance_from_account_after_transaction[15];


};














