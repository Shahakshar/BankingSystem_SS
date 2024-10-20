// takes dummy inout during logging in 
// admin is only ine who do not need anythin to store
struct userCred{
char loginId[50];
char password[50];

};

struct admin{

char loginId[50];
char password[50];
};

// Database customer
//customer attributes
struct customer{

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
struct employee{

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
struct loan{
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
struct account{
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
struct transaction{

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







