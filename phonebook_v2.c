#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];
    char phone[15];
} Contact;

typedef struct NodeBST {
    Contact contact;
    struct NodeBST* left;
    struct NodeBST* right;
} NodeBST;

NodeBST* root = NULL;
NodeBST* current = NULL; //To store the current contact upon which operations will be done.

//BST Functions
NodeBST* search_byName(NodeBST*, const char*);         // 1) For Searching
NodeBST* search_ByPhone(NodeBST*, const char*);        // 2)
void search_byNameList(NodeBST*, const char*, int*);   // 3)
void displayNodeBST(NodeBST*);                         // 1) For Displaying
void inorderTraverse(NodeBST*);                        // 2)
NodeBST* createNodeBST(Contact);                       // 1) For Adding
void insertNodeBST(NodeBST*, Contact);                 // 2)
NodeBST* findMinNodeBST(NodeBST*);                     // 1) For Deleting
NodeBST* deleteNodeBST(NodeBST*, const char*);         // 2)
void editContactBST();                                 // 1) For Editing

//Program Functions
void search();
void displayCurrent();
void display();
void add();
void remove1();
void edit();
NodeBST* loadContacts(FILE*);
void saveContacts(NodeBST*,FILE*);

int main()
{
    FILE* fr = fopen("contacts.dat", "rb");
    if(fr!=NULL)
    {
        root = loadContacts(fr);
        fclose(fr);
        puts("Contacts Loaded");
    }
    char choice;
    while(1)
    {
        printf("\n----- Contact Manager Menu -----\n");
        puts("1) Search For Contacts");
        puts("2) Display The Selected/Searched Contact");
        puts("3) Display The Entire Phonebook");
        puts("4) Add A Contact");
        puts("5) Remove A Contact");
        puts("6) Edit A Contact");
        puts("0) Quit Program");
        printf("\nEnter your choice: ");
        scanf("%c", &choice);
        while (getchar() != '\n');//clearing extra characters
        switch (choice)
        {
            case '1':
                search();
                break;
            case '2':
                displayCurrent();
                break;
            case '3':
                display();
                break;
            case '4':
                add();
                break;
            case '5':
                remove1();
                break;
            case '6':
                edit();
                break;
            case '0':
                FILE* fw = fopen("contacts.dat", "wb");
                if (fw!=NULL) 
                {
                    saveContacts(root, fw);
                    fclose(fw);
                }
                else
                    puts("Failed to open file for saving.");
                return 0;
            default:
                printf("Unrecognized input.\n");
        }
    }
    return 0;
}

//Program Functions

void search()
{
    if (root==NULL)
    {
        puts("Contact list is empty. No search can be performed.");
        return;
    }
    char choice;
    NodeBST* result;
    while(1)
    {
        printf("\n-- Search Menu --\n");
        puts("1) Search By Name");
        puts("2) Search By Number");
        puts("0) Terminate Search Process");
        printf("Enter your choice: ");
        scanf("%c", &choice);
        while (getchar() != '\n');
        switch(choice)
        {
            case '1': //To search by Name
                char newName[50];
                printf("Enter Name: ");
                fgets(newName, sizeof(newName), stdin);
                newName[strcspn(newName, "\n")] = '\0';

                result = search_byName(root, newName);
                if (result!=NULL)
                {
                    puts("Exact match found:");
                    displayNodeBST(result);
                    current = result;
                }
                else //In case no exact match is found, will print all similar contacts
                {
                    puts("No exact match.");
                    int found = 0;
                    search_byNameList(root, newName, &found);
                    if(found)
                        printf("%d Similar Contacts found\n",found);
                    else
                        puts("No similar contacts found.");
                }
                return;
            case '2': //To search by number
                char newPhone[15];
                printf("Enter Phone Number: ");
                fgets(newPhone, sizeof(newPhone), stdin);
                newPhone[strcspn(newPhone, "\n")] = '\0';
                result = search_ByPhone(root, newPhone);
                if(result==NULL)
                    puts("No matching contacts found.");
                else
                {
                    displayNodeBST(result);
                    current = result;
                }
                return;
            case '0':
                puts("Search Process Terminated");
                return;
            default:
                puts("Unrecognized input.");
        }
    }
}

void displayCurrent()
{
    if(root==NULL)
    {
        puts("Contact list is empty. Nothing to Display.");
        return;
    }
    if(current==NULL)
    {
        puts("No contact selected.");
        return;
    }
    puts("Displaying Selected Contact");
    displayNodeBST(current);
}

void display()
{
    if (root==NULL)
    {
        puts("Contact list is empty. Nothing to display.");
        return;
    }
    puts("Displaying Contacts");
    inorderTraverse(root);
}

void add()  // no arguments
{
    puts("Adding Contact");
    int choice;
    Contact temp;
    printf("Enter contact name: ");
    fgets(temp.name, sizeof(temp.name), stdin);
    temp.name[strcspn(temp.name, "\n")] = '\0';

    printf("Enter contact phone: ");
    fgets(temp.phone, sizeof(temp.phone), stdin);
    temp.phone[strcspn(temp.phone, "\n")] = '\0';

    if (root == NULL)
        root = createNodeBST(temp);
    else
        insertNodeBST(root, temp);
    current = NULL;
    puts("Contact Added");
    return;
}

void remove1()
{
    if(root==NULL)
    {
        puts("Contact list is empty. Nothing to Remove.");
        return;
    }
    if(current==NULL)
    {
        puts("Select/Search for a specific Contact first.");
        return;
    }
    char choice;
    puts("Are you sure you want to remove this Contact ? (y/n)");
    displayNodeBST(current);
    scanf("%c",&choice);
    getchar();
    if((choice=='y')||(choice=='Y'))
    {
        root = deleteNodeBST(root,current->contact.name);
        current = NULL;
        puts("Contact Removed"); 
    }
    else
        puts("Cancelled");
}

void edit()
{
    if(root==NULL)
    {
        puts("Contact list is empty. Nothing to Edit.");
        return;
    }
    if(current==NULL)
    {
        puts("Select/Search for a specific Contact first.");
        return;
    }
    editContactBST();
}

NodeBST* loadContacts(FILE* fp)
{
    Contact temp;
    if (!fread(&temp, sizeof(Contact), 1, fp))
        return NULL;

    // NULL marker encountered
    if((strcmp(temp.name, "")==0)&&(strcmp(temp.phone, "")==0))
        return NULL;
    // Recreate current node
    NodeBST* node = createNodeBST(temp);
    // Now recursively build left and right
    node->left = loadContacts(fp);
    node->right = loadContacts(fp);
    return node;
}

void saveContacts(NodeBST* node, FILE* fp)
{
    if (node==NULL)
    {
        Contact nullContact;
        strcpy(nullContact.name, "");
        strcpy(nullContact.phone, "");
        fwrite(&nullContact, sizeof(Contact), 1, fp);
        return;
    }
    fwrite(&(node->contact), sizeof(Contact), 1, fp);
    saveContacts(node->left, fp);
    saveContacts(node->right, fp);
}

//BST FUNCTIONS

// search functions
NodeBST* search_byName(NodeBST* node, const char* name)
{
    if(node==NULL)
        return NULL;
    int cmp = strcmp(name, node->contact.name);
    if(cmp==0)
        return node;
    else if(cmp<0)
        return search_byName(node->left, name);
    else
        return search_byName(node->right, name);
}

NodeBST* search_ByPhone(NodeBST* node, const char* phone)
{
    if(node==NULL) 
        return NULL;
    if(strcmp(node->contact.phone, phone)==0)
        return node;
    NodeBST* found = search_ByPhone(node->left, phone);
    return found?found:search_ByPhone(node->right, phone);
}

void search_byNameList(NodeBST* node, const char* name, int* found)
    {
    if (node == NULL)
        return;
    search_byNameList(node->left, name, found);
    if (strstr(node->contact.name, name)!=NULL)
    {
        displayNodeBST(node);
        *found = *found + 1;
    }
    search_byNameList(node->right, name, found);
}


//Display Functions
void displayNodeBST(NodeBST* node)
{
    printf("Name: %s\tPhone: %s\n", node->contact.name, node->contact.phone);
}

void inorderTraverse(NodeBST* node)
{
    if (node!=NULL)
    {
        inorderTraverse(node->left);
        displayNodeBST(node);
        inorderTraverse(node->right);
    }
}

//Add Fcuntions
NodeBST* createNodeBST(Contact contact)
{
    NodeBST* newNodeBST = (NodeBST*)malloc(sizeof(NodeBST));
    newNodeBST->contact = contact;
    newNodeBST->left = newNodeBST->right = NULL;
    return newNodeBST;
}

void insertNodeBST(NodeBST* node, Contact contact)
{
    int cmp = strcmp(contact.name, node->contact.name);
    if (cmp==0)
    {
        puts("Identical Contact Already Exists");
        return;
    }
    if(cmp<0)
    {
        if(node->left==NULL)
            node->left = createNodeBST(contact);
        else
            insertNodeBST(node->left, contact);
    }
    else
    {
        if (node->right==NULL)
            node->right = createNodeBST(contact);
        else
            insertNodeBST(node->right, contact);
    }
}

//Deletion Fcuntions
NodeBST* findMinNodeBST(NodeBST* node)// Helper to find the minimum node in a subtree
{
    while(node->left != NULL)
        node = node->left;
    return node;
}

NodeBST* deleteNodeBST(NodeBST* node, const char* name)// Delete function for BST based on name
{
    if (node==NULL)
        return NULL;

    int cmp = strcmp(name, node->contact.name);
    if(cmp<0)
        node->left = deleteNodeBST(node->left, name);
    else if(cmp>0)
        node->right = deleteNodeBST(node->right, name);
    else 
    {
        if((node->left==NULL)&&(node->right == NULL))
        {
            free(node);
            return NULL;
        }
        else if (node->left==NULL)
        {
            NodeBST* temp = node->right;
            free(node);
            return temp;
        }
        else if (node->right==NULL)
        {
            NodeBST* temp = node->left;
            free(node);
            return temp;
        }
        else
        {
            NodeBST* min = findMinNodeBST(node->right);
            node->contact = min->contact;
            node->right = deleteNodeBST(node->right, min->contact.name);
        }
    }
    return node;
}

//Edit functions
void editContactBST()
{
    printf("Current details:\n");
    displayNodeBST(current);
    
    char choice;
    
    // Edit phone number
    printf("Do you want to edit the phone number? (y/n): ");
    scanf(" %c", &choice);
    getchar();
    if((choice=='y')||(choice=='Y'))
    {
        char newPhone[15];
        printf("Enter new phone number: ");
        fgets(newPhone, sizeof(newPhone), stdin);
        newPhone[strcspn(newPhone, "\n")] = '\0';
        strcpy(current->contact.phone, newPhone);
        printf("Phone Number updated.\n");
    }

    // Edit name
    printf("Do you want to edit the name? (y/n): ");
    scanf("%c", &choice);
    getchar();
    if((choice=='y')||(choice=='Y'))
    {
        char newName[50];
        printf("Enter new name: ");
        fgets(newName, sizeof(newName), stdin);
        newName[strcspn(newName, "\n")] = '\0';

        // Store updated contact
        Contact updated = current->contact;
        strcpy(updated.name, newName);

        // Delete old node
        root = deleteNodeBST(root, current->contact.name);

        // Re-insert updated contact
        if (root == NULL)
            root = createNodeBST(updated);
        else
            insertNodeBST(root, updated);
        current = search_byName(root, updated.name);
        printf("Name updated.\n");
    }
    printf("Contact update complete.\n");
    displayNodeBST(current);
}