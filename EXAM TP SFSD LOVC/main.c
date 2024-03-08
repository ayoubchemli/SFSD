#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define SEPARATION 50
#define charTable_size 1000

/** membres : CHEMLI Ayoub BELKADI adam **/

// Champs des structures

// Structure pour les informations individuelles de contact
typedef struct {
    char id[9]; // si id == '?' --> contact supprimé
    char nomComplet[31];
    char numeroTelephone[11];
    char email[31];
    char *observations; //de 50 a 250 caractere
} Contact;

// Structure du bloc
typedef struct{
    char Tab[charTable_size + 1];
    int nbrDeCaracter;
    int nextBloc;
}Bloc;

// Structure de l'entete
typedef struct {
    int adressePremierBloc;
    int tailleFichier;
    int nbContacts;
    int nbContactsInseres;
    int nbContactsSupprimes;
} EnTeteFichier;

// La structure représente un nœud dans la liste chaînee
typedef struct ContactNode {
    int id;
    int length;
    struct ContactNode* next;
}ContactNode;

// Indexe

// Contact index structure
typedef struct{
    int id;
    int blocNumber;
    int position;
}ContactIndex;

typedef struct{
    ContactIndex* dynamicArray;
    int size;  // the size of the array
    int contactCounter;
}ArrayIndex;



//variables globales
FILE *contactsFile; // Pointeur de fichier pour 'Contacts.bin'
FILE *indexFile;    // Pointeur de fichier pour 'Contacts_Index.bin'
EnTeteFichier fileHeader;  // Variable d'en-tête de fichier globale
Bloc *buffer;

// global index array
ArrayIndex indexArray;


int N ;


// Fonctions prototypes
void lireBloc(FILE *fichier, int i, Bloc* Buffer);
void ecrireBloc(FILE *fichier, int i, Bloc* Buffer);
void printBlock(int index);
void printAllBlocks();
int getLastBlockNumber();
void displayTable();
int lireEntete(FILE *fichier, int i);
void affecterEntete(FILE *fichier, int i, int val);
void afficherEntete();
void generateRandomString(char *str, int length);
void generateRandomID(char *id, int n);
void initializeFiles();
void closeFiles();
void initializeEnTeteFichier(EnTeteFichier *entete);
int isIDUsed(const char *id, int N, const Contact *contacts);
char* fromContactToString(Contact *newContact);
void randomContact(Contact *newContact, int N, int *usedIDs);
void initializeRandomSeed();
void createContacts(int N);
void getIdFromString(char* str, int blocNumber, char* ID);
char* getIdBeforeAt(const char *token);
void rechercheContacte(int id, int *position, int *i);
bool isIDUsedInFile(int id);
void readContactFromUser(Contact *newContact);
void insertContactAtEnd();
void supprimerContactLogique(int id);
int isFirstContactSplit(char* inputArray);
ContactNode* generateLinkedList(char* inputArray, int *lastId, bool alreadyCheked, int blocNumber);
void initializeBlockNodeTable(struct ContactNode* contactTab[]);
void freeLinkedList(struct ContactNode* head);
void displayLinkedList(struct ContactNode* head);
void printOneContactRow(const struct ContactNode* head, int nextBlocNumber);
void printTwoContactRow(const ContactNode* head1, const ContactNode* head2, int NextBlocNumber1, int NextBlocNumber2);
void printThreeContactRow(const ContactNode* head1, const ContactNode* head2, const ContactNode* head3, int NextBlocNumber1, int NextBlocNumber2, int NextBlocNumber3);
void displayContactTableGroups(ContactNode* contactTab[]);
void printTables(struct ContactNode* contactTab[], int numTables);
int initializeIndexArray(int initialSize);
int binarySearch(int arr[], int left, int right, int target);
int binarySearchContactIndex(ContactIndex arr[], int left, int right, int target);
int binarySearchInsertPosition(ContactIndex arr[], int low, int high, int newValue);
void insererIndex(int id, int blocNumber, int position);
void removeElementById(int id);




void clearScreen() {
    system("cls");
}

int main(int argc, char *argv[]) {
    // Initialize the files at the start of the program
    initializeFiles();
    initializeEnTeteFichier(&fileHeader);

    //index initialisation

    initializeIndexArray(N);


    // Menu page
    printf("\033[1;33m********************************************************************************************************************\n");
    printf("********************************************* GESTION DES CONTACTS *************************************************\n");
    printf("********************************************************************************************************************\n\033[0m");

    // Read N
    printf("\n\033[1;34mVeuillez entrer N, le nombre de contacts a generer : \033[0m");
    do {
        scanf("%d", &N);
        if (N <= 0) {
            printf("\033[1;31mIl faut generer plus de zero contacts : \033[0m");
            scanf("%d", &N);
        }
    } while (N <= 0);

    if (N > 30000) {
        do {
            printf("\033[1;31mVous ne pouvez pas generer plus de 30000 contacts : \033[0m");
            scanf("%d", &N);
        } while (N > 30000);
    }

    createContacts(N);
    displayTable();

    ContactNode *contactTab[getLastBlockNumber()];
    initializeBlockNodeTable(contactTab);
    displayContactTableGroups(contactTab);

    int choice, position, numBloc, id, idASupprimer, numeroBlocAVisualiser;



    do {
        // Display the main menu
        printf("\n\033[1;36mMAIN MENU:\n");
        printf("1.  Rechercher un contact par ID\n");
        printf("2.  Inserer un contact\n");
        printf("3.  Supprimer un contact\n");
        printf("4.  Afficher les contacts\n");
        printf("5.  Afficher l'entete\n");
        printf("6.  Afficher un bloc specifique\n");
        printf("7.  Afficher toutes les structures de blocs (Textuelle)\n");
        printf("8.  Afficher toutes les structures de blocs (Tableaux d'informations)\n");
        printf("9.  Afficher l'entete (Textuelle)\n");
        printf("10. Afficher les contacts (Textuelle)\n");
        printf("0.  Exit\n");
        printf("\033[1;34mEntrez votre choix : \033[0m");




        // Get user choice
        scanf("%d", &choice);
        clearScreen();

        switch (choice) {
            case 1:
                // Recherche de contact
                printf("\n\033[1;34mVeuillez entrer l'ID du contact que vous souhaitez rechercher : \033[0m");
                scanf("%d", &id);
                rechercheContacte(id, &position, &numBloc);
                break;
            case 2:
                // Insérer un contact
                insertContactAtEnd();
                displayTable();
                initializeBlockNodeTable(contactTab);
                displayContactTableGroups(contactTab);
                break;
            case 3:
                // Supprimer un contact
                printf("\n\033[1;34mEntrez l'ID du contact que vous souhaitez supprimer : \033[0m");
                scanf("%d", &idASupprimer);
                supprimerContactLogique(idASupprimer);
                printf("\n");
                displayTable();
                initializeBlockNodeTable(contactTab);
                displayContactTableGroups(contactTab);
                break;
            case 4:
                // Afficher les contacts
                initializeBlockNodeTable(contactTab);
                printTables(contactTab, getLastBlockNumber());
                break;
            case 5:
                // Afficher l'entête
                displayTable();
                break;
            case 6:
                // Afficher un bloc spécifique
                printf("\033[1;34mEntrez le numero de bloc que vous souhaitez afficher : \033[0m");
                scanf("%d", &numeroBlocAVisualiser);
                printBlock(numeroBlocAVisualiser);
                break;
            case 7:
                // Imprimer toutes les structures de blocs (Textuelle)
                printAllBlocks();
                break;
            case 8:
                // Imprimer toutes les structures de blocs (Tableaux)
                initializeBlockNodeTable(contactTab);
                displayContactTableGroups(contactTab);
                break;
            case 9:
                // Afficher l'entête (Textuelle)
                afficherEntete();
                break;
            case 10:
                // Afficher les contacts (Textuelle)
                for (int i = 1; i < getLastBlockNumber() + 1; i++) {
                    displayLinkedList(contactTab[i]);
                }
                break;
            case 0:
                printf("\033[1;32mFin du programme. Merci de l'avoir utilise !\033[0m\n");
                printf("\033[0;35m\nProgramme realise par:\nChemli Ayoub\nBelkadi Adam\033[0m\n");
                break;
            default:
                printf("\033[1;31mChoix non valide. Veuillez reessayer.\033[0m\n");
        }

    } while (choice != 0);

    displayTable();

    // ecrire la table d'index sur le fichier
    writeArrayIndexToFile(indexFile);

    closeFiles();

    return 0;
}



// les fonctions premitive

// fonction pour la licture d'un bloc
void lireBloc(FILE *fichier, int i, Bloc* Buffer) {
    fseek (fichier,(i * sizeof(Bloc))+ sizeof(EnTeteFichier), SEEK_SET);
    fread (Buffer, sizeof(Bloc), 1, fichier);
}

// fonction pour l'ecriture d'un bloc
void ecrireBloc(FILE *fichier, int i, Bloc* Buffer) {
    fseek(fichier, (i * sizeof(Bloc)) + sizeof(EnTeteFichier), SEEK_SET);
    fwrite(Buffer, sizeof(Bloc), 1, fichier);
}

// Fonction pour imprimer un bloc specifique dans le fichier
void printBlock(int index) {
    Bloc currentBlock;
    lireBloc(contactsFile, index, &currentBlock);
    printf("Block %d:\n", index);
    printf("Tab: \t %s\n", currentBlock.Tab);
    printf("nbrDeCaracteres: \t %d\n", currentBlock.nbrDeCaracter);
    printf("blockSuivant: \t %d\n", currentBlock.nextBloc);
    printf("\n");
}

// Fonction pour imprimer toutes les structures de blocs dans le fichier
void printAllBlocks() {
    int lastBlockIndex = 0 ;
    Bloc *buffer = malloc(sizeof(Bloc));

    do{
        lastBlockIndex ++ ;
        lireBloc(contactsFile, lastBlockIndex, buffer);
    }while (buffer->nextBloc != -1) ;
    Bloc currentBlock;

    for (int i = 1; i <= lastBlockIndex; i++) {
        lireBloc(contactsFile, i, buffer);
        printf("Block %d:\n", i);
        printf("Tab: \t %s\n", buffer->Tab);
        printf("nbrDeCaracteres: \t %d\n", buffer->nbrDeCaracter);
        printf("blockSuivant: \t %d\n", buffer->nextBloc);
        printf("\n");
    }
    free(buffer);
}


//Fonction pour obtenir le numéro de dernier bloc
int getLastBlockNumber() {
    int lastBlockIndex = 0;
    Bloc *buffer1 = malloc(sizeof(Bloc));

    do {
        lastBlockIndex++;
        lireBloc(contactsFile, lastBlockIndex, buffer1);
    } while (buffer1->nextBloc != -1);

    free(buffer1);
    return lastBlockIndex;
}

//Afficher l'entete sous forme du Table
void displayTable() {
    printf("%30s+----------------------+-----------------+\n", "");
    printf("%30s| %-20s | %-15s |\n", "", "champ", "valeur");
    printf("%30s+----------------------+-----------------+\n", "");
    printf("%30s| %-20s | %-15d |\n", "", "adressePremierBloc", lireEntete(contactsFile,1));
    printf("%30s| %-20s | %-15d |\n", "", "tailleFichier", lireEntete(contactsFile,2));
    printf("%30s| %-20s | %-15d |\n", "", "nbContacts", lireEntete(contactsFile,3));
    printf("%30s| %-20s | %-15d |\n", "", "nbContactsInseres", lireEntete(contactsFile,4));
    printf("%30s| %-20s | %-15d |\n", "", "nbContactsSupprimes", lireEntete(contactsFile,5));
    printf("%30s+----------------------+-----------------+\n", "");
}


//lire entete
int lireEntete(FILE *fichier, int i) {
    int valeur;
    fseek(fichier, i *sizeof(int), SEEK_SET);
    fread(&valeur, sizeof(int), 1, fichier);
    return valeur;
}

//affecter Entete
void affecterEntete(FILE *fichier, int i, int val) {
    fseek(fichier, i * sizeof(int), SEEK_SET);
    fwrite(&val, sizeof(int), 1, fichier);
}

void afficherEntete (){
    printf ("Adresse du premier bloc du fichier: %d \n" ,lireEntete(contactsFile,1)) ;
    printf ("Taille du fichier: %d \n",lireEntete(contactsFile,2)) ;
    printf ("Nombre de contactes: %d \n",lireEntete(contactsFile,3)) ;
    printf ("Nombre de contactes inseres: %d \n",lireEntete(contactsFile,4)) ;
    printf ("Nombre de contactes supprimes: %d \n" ,lireEntete(contactsFile,5)) ;
}


// Générer une chaîne aléatoire de longueur n
void generateRandomString(char *str, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < length; ++i) {
        int index = rand() % (sizeof(charset) - 1);
        str[i] = charset[index];
    }
    str[length] = '\0'; // Terminer la chaîne par un caractère nul
}

// Fonction pour générer un identifiant de chaîne aléatoire
void generateRandomID(char *id, int n) {
    sprintf(id, "%08d", rand() % n + 1);
}


/**
 * Initialise les fichiers nécessaires au gestionnaire de contacts.
 * Ouvre les fichiers "Contacts.bin" et "Contacts_Index.bin" en mode lecture/écriture binaire.
 * En cas d'échec de l'ouverture des fichiers, affiche un message d'erreur et quitte le programme.
 * Lit l'en-tête du fichier à partir du fichier des contacts.
 */
 void initializeFiles() {
    contactsFile = fopen("Contacts.bin", "wb+"); // Open in binary read/write mode
    indexFile = fopen("Contacts_Index.bin", "wb+");


    if (contactsFile == NULL || indexFile == NULL) {
        // Handle file opening errors
        printf("Error opening files.\n");
        exit(EXIT_FAILURE);
    }

    // Read the file header
    fread(&fileHeader, sizeof(EnTeteFichier), 1, contactsFile);
}

// Fonction pour fermer les fichiers une fois terminé
void closeFiles() {
    if (contactsFile != NULL) {
        fclose(contactsFile);
    }
    if (indexFile != NULL) {
        fclose(indexFile);
    }
}



// Fonction pour initialiser une instance de EnTeteFichier
void initializeEnTeteFichier(EnTeteFichier *entete) {
    affecterEntete(contactsFile, 1, 1);
    affecterEntete(contactsFile, 2, 0);
    affecterEntete(contactsFile, 3, 0);
    affecterEntete(contactsFile, 4, 0);
    affecterEntete(contactsFile, 5, 0);
}

// Fonction pour verifier si un ID est utiliser
int isIDUsed(const char *id, int N, const Contact *contacts) {
    for (int i = 0; i < N; ++i) {
        if (strcmp(id, contacts[i].id) == 0) {
            return 1; // ID is already used
        }
    }
    return 0; // ID is not used
}

// transformer de Contact Vers Chaine
char* fromContactToString (Contact *newContact){
    char *str = malloc(400);
    if (str == NULL) {
        return NULL;
    }

    sprintf(str, "%s|%s|%s|%s|%s#",newContact->id,newContact->nomComplet,newContact->numeroTelephone,newContact->email,newContact->observations);
    return str;
    // '|' pour les champs
    // '#' pour les contacts

}


//Fonction pour obtenir des séquences aléatoires différentes à chaque exécution.
void initializeRandomSeed() {
    srand(time(NULL));
}


//Fonction pour generer un Contact Aleatoire
void randomContact(Contact *newContact, int N, int *usedIDs){

    // Generate unique random ID
        do {
        generateRandomID(newContact->id, N);
        } while (usedIDs[atoi(newContact->id) - 1]);

        usedIDs[atoi(newContact->id) - 1] = 1; // Mark the ID as used


        // Generate random name (30 characters)
        generateRandomString(newContact->nomComplet, 30);

        // Generate random phone number (10 digits)
        for (int j = 0; j < 10; ++j) {
            newContact->numeroTelephone[j] = '0' + rand() % 10;
        }
        newContact->numeroTelephone[10] = '\0'; // Null-terminate the phone number

        // Generate random email (30 characters)
        generateRandomString(newContact->email, 30);

        // Generate random observations (50 to 250 characters)
        int observationsLength = 50 + rand() % 201; // Random length between 50 and 250
        newContact->observations = malloc((observationsLength + 1) * sizeof(char));
        generateRandomString(newContact->observations, observationsLength);
}

// Fonction pour créer N contactes avec des données aléatoires et écrire dans le fichier en utilisant la structure LVC
void createContacts(int N) {

    initializeRandomSeed();

    buffer = malloc(sizeof(Bloc));
    buffer->nextBloc = 2;

    // Set to track used IDs
    int *usedIDs = calloc(N, sizeof(int));
    if (usedIDs == NULL) {
        // Handle memory allocation failure
        printf("Error allocating memory for used IDs.\n");
        exit(EXIT_FAILURE);
    }

    buffer->nbrDeCaracter = 0;
    buffer->Tab[0] = '\0';
    int nbBloc = 1;
    int nbContacte = 0;
    char firstPart[500]; // +1 for null terminator
    char secondPart[500];

    for (int i = 0; i < N; ++i) {
        Contact newContact;
        nbContacte ++;

        //create a random contact
        randomContact(&newContact, N, usedIDs);

        //transform from Contact To String
        char *str = fromContactToString(&newContact);
        int tailleContacte = strlen(str);

        if (tailleContacte < (charTable_size - buffer->nbrDeCaracter) ){

            //ecrire dans tab index
            insererIndex(atoi(newContact.id), nbBloc, buffer->nbrDeCaracter);

            buffer->nbrDeCaracter = buffer->nbrDeCaracter + tailleContacte ;

            strcat(buffer->Tab, str);



        }else{

            //ecrire dans tab index
            insererIndex(atoi(newContact.id), nbBloc, buffer->nbrDeCaracter);

            int nb = charTable_size - buffer->nbrDeCaracter;
            firstPart[nb] = '\0';
            secondPart[nb] = '\0';


            // Copy the characters before the separation position to the first part
            strncpy(firstPart, str, nb);
            firstPart[nb] = '\0'; // Null-terminate the first part

            // Copy the characters after the separation position to the second part
            strcpy(secondPart, str + nb);

            strcat(buffer->Tab, firstPart);

            buffer->nbrDeCaracter = buffer->nbrDeCaracter + strlen(firstPart) ;

            ecrireBloc(contactsFile, nbBloc, buffer);
            nbBloc++ ;

            affecterEntete(contactsFile, 2, lireEntete(contactsFile,2)+ sizeof(Bloc) );
            affecterEntete(contactsFile, 3, lireEntete(contactsFile,3)+ nbContacte );

            buffer->nbrDeCaracter = 0;
            buffer->Tab[0] = '\0';
            buffer->nextBloc ++;

            strcat(buffer->Tab, secondPart);
            buffer->nbrDeCaracter = tailleContacte - nb;

            nbContacte = 0;


        }
        // free(newContact->observations);
        free (str);
    }
    //derniere iteration pour un buffer non complet (ou complet)
    buffer->nextBloc = -1;
    ecrireBloc(contactsFile, nbBloc, buffer);
    affecterEntete(contactsFile, 2, lireEntete(contactsFile,2)+ sizeof(Bloc) );
    affecterEntete(contactsFile, 3, lireEntete(contactsFile,3)+ nbContacte );


    // Free allocated memory
    free(buffer);

}

// Obtenir Id avant '|'
char* getIdBeforeAt(const char *token) {
    // Find the position of | in the token
    char *subtoken = strchr(token, '|');

    if (subtoken != NULL) {
        // Calculate the length of the substring before |
        int length = subtoken - token;

        // Create a buffer to store the substring before |
        char *substring = (char *)malloc((length + 1) * sizeof(char));
        strncpy(substring, token, length);
        substring[length] = '\0';  // Null-terminate the substring

        return substring;
    }

    return NULL;
}

void getIdFromString(char* str, int blocNumber, char* ID) {

    ID[0] = '\0';
    if(str[0] == '?') {
        strcpy(ID, "????????");
        ID[8] = '\0';
        return;
    }
    if(strlen(str) < 8) {
        strcpy(ID, str);
        ID[strlen(str)] = '\0';

        if(ID[0] != '0' && atoi(ID) == 0) return;

        Bloc* buffer2 = malloc(sizeof(Bloc));
        lireBloc(contactsFile, blocNumber + 1, buffer2);

        char *delimiterPosition = strchr(buffer2->Tab, '|');
        size_t index = delimiterPosition - buffer2->Tab;
        strncat(ID, buffer2->Tab, index);
        free(buffer2);
    } else {
        strncpy(ID, str, 8);
    }
    ID[8] = '\0';
    return;
}




// fonction qui recherche un Contact
void rechercheContacte(int id, int *position, int *i) {
    *i = 0;
    bool found = false;
    buffer = (Bloc*) malloc(sizeof(Bloc));
    bool alreadyCheked = false;
    do {
        *i = *i + 1;
        lireBloc(contactsFile, *i, buffer);
        char *token = strtok(buffer->Tab, "#");
        *position = 0; // Variable to store the position of the ID

        while (token != NULL) {
            char* token2 = malloc(9 * sizeof(char));
            getIdFromString(token, *i, token2);
            if(alreadyCheked == true) {
                    alreadyCheked = false;
                    *position += strlen(token) + 1;
                    token = strtok(NULL, "#");
                    continue;
            }

            if (atoi(token2) == id) {
                free(buffer);
                found = true; // Set the flag to indicate contact is found
                break; // Break out of the inner while loop
            }
            *position += strlen(token) + 1; // Update the position based on the length of the token
            free(token2);
            token = strtok(NULL, "#");
        }
        if (found) {
            printf("\n");
            printf("\033[1;32mcontacte %d trouve a la position %d dans le Block numero %d\033[0m\n", id, *position, *i);

        }

        if(buffer->Tab[sizeof(buffer->Tab) - 1] != '#') {
                alreadyCheked = true;
        }
    }while (buffer->nextBloc != -1 && !found);


    if (!found) {

        printf("\033[1;31m\n contacte %d non trouve dans le fichier\n\033[0m\n", id);
        free(buffer);
    }

}

// Fonction pour vérifier si un ID est déjà utilisé dans le fichier
bool isIDUsedInFile(int id) {
    Bloc *buffer1 = malloc(sizeof(Bloc));
    int i = 0;
    bool found = false;

    do {
        i++;
        lireBloc(contactsFile, i, buffer1);
        char *token = strtok(buffer1->Tab, "#");

        while (token != NULL) {
            char *token2 = getIdBeforeAt(token);
            if (atoi(token2) == id) {
                free(buffer1);
                return true; // ID is already used in the file
            }
            token = strtok(NULL, "#");
        }
    } while (buffer1->nextBloc != -1);
    free(buffer1);
    return false; // ID is not used in the file
}
// Fonction pour lire un contact de l'utilisateur
void readContactFromUser(Contact *newContact) {
    int id;
    // ID (unique identifier on 8 characters)
    do {
        printf("Entrer l'ID (numerique, superieur a %d) :", N);
        while (getchar() != '\n');
        scanf("%d", &id);

        if (id <= N) {
            printf("Erreur : L'ID doit etre superieur a %d. Veuillez entrer un identifiant valide.\n", N);
            scanf("%d", &id);
        } else if (isIDUsedInFile(id)) {
            printf("Erreur : L'ID est deja utilise dans le fichier. Veuillez choisir un identifiant diffecferent.\n");
            scanf("%d", &id);
        }
        // Consume the newline character in the input buffer
        while (getchar() != '\n');

    } while (id <= N || isIDUsedInFile(id) );



    sprintf(newContact->id, "%d", id); // Convert the ID to a string and set it in the newContact structure

    // Nom complet (30 characters)
    printf("Entrer le nom complet (jusqu'a 30 caracteres) : ");
    scanf("%30s", newContact->nomComplet);

    // Num ro de t l phone (10 characters)
    printf("Entrer le numero de telephone (10 caracteres) : ");
    scanf("%10s", newContact->numeroTelephone);

    // Adresse email (30 characters)
    printf("Enterer Email (jusqu'a 30 caracteres): ");
    scanf("%30s", newContact->email);

    // Observations (50 to 250 characters)
    printf("Entrer des observations (entre 50 et 250 caracteres) : ");
    newContact->observations = malloc(256); // Adjust the size as needed

    // Read up to 250 characters
    scanf(" %250s", newContact->observations);

    // Validate the length of observations
    while (strlen(newContact->observations) < 50 || strlen(newContact->observations) > 250) {
        printf("Erreur : Les observations doivent comporter entre 50 et 250 caracteres. Veuillez les reintroduire : ");
        scanf(" %250s", newContact->observations);
    }

}

// Fonction pour insérer un nouveau contact à la fin du fichier
void insertContactAtEnd() {
    Contact *newContact = malloc(sizeof(Contact));
    // Allocate memory for the buffer
    buffer = malloc(sizeof(Bloc));

    // Find the last block in the file
    int lastBlockIndex = 0 ;

    do{
        lastBlockIndex ++ ;
        lireBloc(contactsFile, lastBlockIndex, buffer);
    }while (buffer->nextBloc != -1) ;
    //read Contact From User
    readContactFromUser(newContact);

    // Convert the new contact to a string
    char *str = fromContactToString(newContact);

    if ((strlen(buffer->Tab) + strlen(str)) < sizeof(buffer->Tab)){

        //ecrire dans tab index
        insererIndex(atoi(newContact->id), lastBlockIndex, strlen(buffer->Tab));

        // Append the new contact to the last block
        strcat(buffer->Tab, str);
        buffer->nbrDeCaracter = buffer->nbrDeCaracter + strlen(str);

        // Write the updated buffer to the last block in the file
        ecrireBloc(contactsFile, lastBlockIndex, buffer);

        // Update the file header in the file
        affecterEntete(contactsFile, 3, lireEntete(contactsFile,3)+ 1 );
        affecterEntete(contactsFile, 4, lireEntete(contactsFile,4)+ 1);



    }else{

        //ecrire dans tab index
        insererIndex(atoi(newContact->id), lastBlockIndex, strlen(buffer->Tab));

        int remainingSpace = sizeof(buffer->Tab) - strlen(buffer->Tab);
        int tailleContacte2 = strlen(str);

        char firstPart2[remainingSpace+1]; // +1 for null terminator
        char secondPart2[tailleContacte2-remainingSpace+1];

        // Copy the characters before the separation position to the first part
        strncpy(firstPart2, str, remainingSpace);
        firstPart2[remainingSpace] = '\0'; // Null-terminate the first part

        // Copy the characters after the separation position to the second part
        strcpy(secondPart2, str + remainingSpace);

        strcat(buffer->Tab, firstPart2);

        buffer->nbrDeCaracter = sizeof(buffer->Tab);
        buffer->nextBloc = lastBlockIndex+1;

        ecrireBloc(contactsFile, lastBlockIndex, buffer);

        lastBlockIndex++ ;


        buffer->nbrDeCaracter = 0;
        buffer->Tab[0] = '\0';

        strcat(buffer->Tab, secondPart2);
        buffer->nbrDeCaracter = tailleContacte2 - remainingSpace;
        buffer->nextBloc = -1;

        ecrireBloc(contactsFile, lastBlockIndex, buffer);


        affecterEntete(contactsFile, 2, lireEntete(contactsFile,2)+ sizeof(Bloc) );
        affecterEntete(contactsFile, 3, lireEntete(contactsFile,3)+ 1 );
        affecterEntete(contactsFile, 4, lireEntete(contactsFile,4)+ 1);


    }

    // Free allocated memory
    free(buffer);
    free(newContact);
}


//fonction pour la suppretion Logique d'un Contact
void supprimerContactLogique(int id) {
    removeElementById(id);
    Bloc *buffer2 ;
    int position;
    int numBloc;
    if (isIDUsedInFile(id)){

        rechercheContacte( id, &position, &numBloc);
        buffer2 = malloc(sizeof(Bloc));

        lireBloc(contactsFile, numBloc,buffer2);
        while (buffer2->Tab[position] != '|' && buffer2->Tab[position] != '\0'){
            buffer2->Tab[position] = '?';
            position++;
        };

        ecrireBloc(contactsFile, numBloc, buffer2);

        int i = 0;
        if(buffer2->Tab[position] != '\0'){
            lireBloc(contactsFile, numBloc + 1,buffer2);
            while (buffer2->Tab[i] != '|'){
                buffer2->Tab[i] = '?';
                i++;
            }
        }


        affecterEntete(contactsFile, 5, lireEntete(contactsFile,5)+ 1 );
        affecterEntete(contactsFile, 3, lireEntete(contactsFile,3)- 1 );

        free(buffer2);
    }else{
        printf("\033[1;31mL'ID n'est pas valide\033[0m\n");
    }
}


// Fonction pour déterminer si le premier contact est divisé ou non
int isFirstContactSplit(char* inputArray) {
    // Check if the entire string ends with '#'
    return (inputArray[strlen(inputArray) - 1] == '#') ? 0 : 1;
}


// Fonction pour générer une liste chaînée à partir d'un tableau de caractères
    ContactNode* generateLinkedList(char* inputArray, int *lastId, bool alreadyCheked, int blocNumber) {

    struct ContactNode* head = NULL; // Head of the linked list
    struct ContactNode* current = NULL; // Current node being processed
    char* token1;

    // Tokenize the inputArray based on the '#' character
    token1 = strtok(inputArray, "#");
    bool new = true;
    char token[9];
    while (token1 != NULL) {
        // Create a new node
        struct ContactNode* newNode = (struct ContactNode*)malloc(sizeof(struct ContactNode));
        getIdFromString(token1, blocNumber, token);

        int id;

        if(alreadyCheked == true) {
            alreadyCheked = false;
            id = *lastId;
            token1 = strtok(NULL, "#");
        } else {
            if(token[0] == '?') {
                id = -1;
                *lastId = id;

            } else {
                id = atoi(token);
                *lastId = id;
            }
        }
        newNode->id = id;
        newNode->length = strlen(token1);
        // Add the new node to the linked list
        if (head == NULL) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }
        // Move to the next token
        token1 = strtok(NULL, "#");
        new == false; // hanling the first part

    }
        current->next = NULL; // Set the next pointer of the last node to NULL
    return head;
}

// Fonction pour initialiser un tableau de BlockNode
void initializeBlockNodeTable(ContactNode* contactTab[]) {
    int capacity = getLastBlockNumber();
    int lastId;

    buffer = malloc(sizeof(Bloc));
    bool alreadyCheked = false;
    for (int i = 1; i < capacity + 1; ++i) {
        lireBloc(contactsFile, i, buffer);
        contactTab[i] = generateLinkedList(buffer->Tab, &lastId, alreadyCheked, i);
        if(buffer->Tab[sizeof(buffer->Tab) - 1] != '#') {
            alreadyCheked = true;
        }
    }
    free(buffer);
}

// Fonction pour initialiser un tableau de BlockNode avec une capacité de 50

// Fonction pour libérer la mémoire occupée par la liste chaînée
void freeLinkedList(struct ContactNode* head) {
    struct ContactNode* current = head;
    struct ContactNode* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

//afficher Liste Chaine
void displayLinkedList(struct ContactNode* head) {
    struct ContactNode* current = head;

    while (current != NULL) {
        printf("ID: %d, Length: %d\n", current->id, current->length);
        current = current->next;
    }
}


// Fonction pour imprimer un block sous forme tableau d'informations
void printOneContactRow(const ContactNode* head, int nextBlocNumber) {
    // Print table header
    printf("+---------------+--------+\n");
    printf("| next bloc  :%10d |\n", nextBlocNumber);
    printf("+---------------+--------+\n");

    // Iterate through the linked list and print each ContactNode's information
    const ContactNode* current = head;
    while (current != NULL) {
        if (current->id == -1) {
            printf("|%-25s | %5sc |", "\033[1;31mdeleted\033[0m", 0);
        } else {
            printf("|contact %6d | %5dc |\n", current->id, current->length);
        }
        current = current->next;

    }

    // Print table footer
    printf("+---------------+--------+\n");
}

void printTwoContactRow(const ContactNode* head1, const ContactNode* head2, int NextBlocNumber1, int NextBlocNumber2) {
    // Print table header
    printf("+---------------+--------+%11s+---------------+--------+\n", "");
    printf("| next bloc  :%9d  |%11s| next bloc  :%9d  |\n", NextBlocNumber1, "", NextBlocNumber2);
    printf("+---------------+--------+%11s+---------------+--------+\n", "");

    // Iterate through the linked lists and print each ContactNode's information
    const ContactNode* current1 = head1;
    const ContactNode* current1_temp = head1;
    int nombreDeContacteBloc1 = 0;
    const ContactNode* current2 = head2;
    const ContactNode* current2_temp = head2;
    int nombreDeContacteBloc2 = 0;

    // Calculate the number of contacts in each linked list
    while (current1_temp->next != NULL) {
        current1_temp = current1_temp->next;
        nombreDeContacteBloc1++;
    }
    while (current2_temp->next != NULL) {
        current2_temp = current2_temp->next;
        nombreDeContacteBloc2++;
    }

    // Determine the maximum number of contacts among the two linked lists
    int maxNumber = nombreDeContacteBloc1;
    if (nombreDeContacteBloc2 > maxNumber) {
        maxNumber = nombreDeContacteBloc2;
    }

    char* deletedContact = "|%-25s | %5sc |";
    char* notDeletedContact = "|contact %6s | %5sc |";
    char* noexistingContact = "|%14s | %6s |";
    char* skipLine = "\n";
    char* tab = "%11s";

    for (int i = 0; i < maxNumber + 1; i++) {
        bool status1 = (current1 && current1->id != -1);
        bool status2 = (current2 && current2->id != -1);

        const char* finalString1 = (status1) ? notDeletedContact : deletedContact;
        const char* finalString2 = (status2) ? notDeletedContact : deletedContact;

        char* concatenatedString = malloc(strlen(finalString1) + strlen(tab) + strlen(finalString2) + strlen(skipLine) + 1);

        if (current1 && current2) {
            sprintf(concatenatedString, "%s%s%s%s", finalString1, tab, finalString2, skipLine);
        } else if (current1) {
            sprintf(concatenatedString, "%s%s%s%s", finalString1, tab, noexistingContact, skipLine);
        } else if (current2) {
            sprintf(concatenatedString, "%s%s%s%s", noexistingContact, tab, finalString2, skipLine);
        }

        char* var1 = "";
        char* var2 = "";
        if (current1) {
            if (current1->id != -1) {
                var1 = malloc(8);
                sprintf(var1, "%d", current1->id);
                var2 = malloc(8);
                sprintf(var2, "%d", current1->length);
            } else {
                var1 = "\033[1;31mdeleted\033[0m";
                var2 = "0";
            }
            current1 = current1->next;
        }

        char* var3 = "";
        char* var4 = "";
        if (current2) {
            if (current2->id != -1) {
                var3 = malloc(8);
                sprintf(var3, "%d", current2->id);
                var4 = malloc(8);
                sprintf(var4, "%d", current2->length);
            } else {
                var3 = "\033[1;31mdeleted\033[0m";
                var4 = "0";
            }
            current2 = current2->next;
        }

        printf(concatenatedString, var1, var2, "", var3, var4);


        free(concatenatedString);
    }


    // Print table footer
    printf("+---------------+--------+%11s+---------------+--------+\n", "");
}



void printThreeContactRow(const ContactNode* head1, const ContactNode* head2, const ContactNode* head3, int NextBlocNumber1, int NextBlocNumber2, int NextBlocNumber3) {
    // Print table header
    printf("+---------------+--------+%11s+---------------+--------+%11s+---------------+--------+\n", "", "");
    printf("| next bloc  :%9d  |%11s| next bloc  :%9d  |%11s| next bloc  :%9d  |\n", NextBlocNumber1, "", NextBlocNumber2, "", NextBlocNumber3);
    printf("+---------------+--------+%11s+---------------+--------+%11s+---------------+--------+\n", "", "");

    // Iterate through the linked list and print each ContactNode's information
    const ContactNode* current1 = head1;
    const ContactNode* current2 = head2;
    const ContactNode* current3 = head3;

    char* deletedContact = "|%-25s | %5sc |";
    char* notDeletedContact = "|contact %6s | %5sc |";
    char* noexistingContact = "|%14s | %6s |";
    char* skipLine = "\n";
    char* tab = "%11s";


    while (current1 || current2 || current3) {
        const char* finalString1 = (current1 && current1->id != -1) ? notDeletedContact : deletedContact;
        const char* finalString2 = (current2 && current2->id != -1) ? notDeletedContact : deletedContact;
        const char* finalString3 = (current3 && current3->id != -1) ? notDeletedContact : deletedContact;

        char* concatenatedString = malloc(strlen(finalString1) + strlen(tab) + strlen(finalString2) + strlen(tab) + strlen(finalString3) + strlen(skipLine) + 1);
        if(current1 && current2 && current3) {
        sprintf(concatenatedString, "%s%s%s%s%s%s", finalString1, tab, finalString2, tab, finalString3, skipLine); //concatenate the strings
        } else if(current1 && current2) {
        sprintf(concatenatedString, "%s%s%s%s%s%s", finalString1, tab, finalString2, tab, noexistingContact, skipLine); //concatenate the strings
        } else if(current1 && current3) {
        sprintf(concatenatedString, "%s%s%s%s%s%s", finalString1, tab, noexistingContact, tab, finalString3, skipLine); //concatenate the strings
        } else if(current2 && current3) {
        sprintf(concatenatedString, "%s%s%s%s%s%s", noexistingContact, tab, finalString2, tab, finalString3, skipLine); //concatenate the strings
        } else if(current1) {
        sprintf(concatenatedString, "%s%s%s%s%s%s", finalString1, tab, noexistingContact, tab, noexistingContact, skipLine); //concatenate the strings
        } else if(current2) {
        sprintf(concatenatedString, "%s%s%s%s%s%s", noexistingContact, tab, finalString2, tab, noexistingContact, skipLine); //concatenate the strings
        } else if(current3) {
        sprintf(concatenatedString, "%s%s%s%s%s%s", noexistingContact, tab, noexistingContact, tab, finalString3, skipLine); //concatenate the strings
        }
        char* var1 = "";
        char* var2 = "";
        if (current1) {
            if (current1->id != -1) {
                var1 = malloc(8);
                sprintf(var1, "%d", current1->id);
                var2 = malloc(8);
                sprintf(var2, "%d", current1->length);
            } else {
                var1 = "\033[1;31mdeleted\033[0m";
                var2 = "0";
            }
            current1 = current1->next;
        }

        char* var3 = "";
        char* var4 = "";
        if (current2) {
            if (current2->id != -1) {
                var3 = malloc(8);
                sprintf(var3, "%d", current2->id);
                var4 = malloc(8);
                sprintf(var4, "%d", current2->length);
            } else {
                var3 = "\033[1;31mdeleted\033[0m";
                var4 = "0";
            }
            current2 = current2->next;
        }

        char* var5 = "";
        char* var6 = "";
        if (current3) {
            if (current3->id != -1) {
                var5 = malloc(8);
                sprintf(var5, "%d", current3->id);
                var6 = malloc(8);
                sprintf(var6, "%d", current3->length);
            } else {
                var5 = "\033[1;31mdeleted\033[0m";
                var6 = "0";
            }
            current3 = current3->next;
        }

        printf(concatenatedString, var1, var2, "", var3, var4, "", var5, var6);
        free(concatenatedString);

    }

    // Print table footer
    printf("+---------------+--------+%11s+---------------+--------+%11s+---------------+--------+\n", "", "");
}

void displayContactTableGroups(ContactNode* contactTab[]) {
    int lastNextBlocNumber = getLastBlockNumber();
    int i = 1;
    Bloc* temporaryBuffer1 = malloc(sizeof(Bloc));
    Bloc* temporaryBuffer2 = malloc(sizeof(Bloc));
    Bloc* temporaryBuffer3 = malloc(sizeof(Bloc));

    // Iterate through the table and display contact information based on the remaining blocks
    while (i < lastNextBlocNumber + 1) {
        lireBloc(contactsFile, i, temporaryBuffer1);
        lireBloc(contactsFile, i + 1, temporaryBuffer2);
        lireBloc(contactsFile, i + 2, temporaryBuffer3);
        if (i + 2 < lastNextBlocNumber + 1) {
            // Display 3 blocks
            bool condition = false;
            if (i + 2 == lastNextBlocNumber) condition = true;
            printThreeContactRow(contactTab[i], contactTab[i + 1], contactTab[i + 2], temporaryBuffer1->nextBloc, temporaryBuffer2->nextBloc, temporaryBuffer3->nextBloc);
            i += 3;  // Increment the index by 3 for the next iteration
        } else if (i + 1 < lastNextBlocNumber + 1) {
            // Display 2 blocks
            lireBloc(contactsFile, i, temporaryBuffer1);
            lireBloc(contactsFile, i + 1, temporaryBuffer2);
            printTwoContactRow(contactTab[i], contactTab[i + 1], temporaryBuffer1->nextBloc, temporaryBuffer2->nextBloc);
            i += 2;  // Increment the index by 2 for the next iteration
        } else {
            // Display 1 block
            lireBloc(contactsFile, i, temporaryBuffer1);
            printOneContactRow(contactTab[i], temporaryBuffer1->nextBloc);
            i++;
        }
    }
    free(temporaryBuffer1);
    free(temporaryBuffer2);
    free(temporaryBuffer3);
}


// Fonction pour imprimer tout les block sous forme des tableau d'informations
void printTables(ContactNode* contactTab[], int numTables) {
    int tablesPrinted = 0;

    // Iterate through the tables
    for (int i = 1; i < numTables + 1; ++i) {
        // Print the table and increment the count
        printf("Block %d:\n", i );
        printOneContactRow(contactTab[i], i + 1);
        printf("\n");
        tablesPrinted++;

    }
}


//*************************************************************************************************************//

int initializeIndexArray(int initialSize) {
    indexArray.dynamicArray = malloc(initialSize * sizeof(ContactIndex));
    if (indexArray.dynamicArray == NULL) {
        // Memory allocation failed
        return 0;
    }
    indexArray.size = initialSize;
    indexArray.contactCounter = 0;
    return 1;
}
// Binary search function
int binarySearch(int arr[], int left, int right, int target) {
    while (left <= right) {
        int mid = left + (right - left) / 2;

        // Check if the target is present at the middle
        if (arr[mid] == target)
            return mid;

        // If the target is greater, ignore the left half
        if (arr[mid] < target)
            left = mid + 1;
        // If the target is smaller, ignore the right half
        else
            right = mid - 1;
    }

    // If the target is not present in the array
    return -1;
}

int binarySearchInsertPosition(ContactIndex arr[], int low, int high, int newValue) {
    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (arr[mid].id < newValue) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    // If the new value is not found, return the position where it should be inserted
    return low;
}

// inserer index function
void insererIndex(int id, int blocNumber, int position) {


    if (indexArray.size <= indexArray.contactCounter) {

        int newSize = indexArray.size + 10;
        ContactIndex* temp = realloc(indexArray.dynamicArray, newSize * sizeof(ContactIndex));

        if (temp == NULL) {
            // Handle memory allocation failure
            printf("Memory reallocation failed.\n");
            return;
        }

        // Update the dynamic array pointer to point to the new memory
        indexArray.dynamicArray = temp;

        // Update the size
        indexArray.size = newSize;
    }

    ContactIndex tempContact;
    tempContact.id = id;
    tempContact.blocNumber = blocNumber;
    tempContact.position = position;

    if (indexArray.contactCounter == 0) {
        indexArray.dynamicArray[0] = tempContact;
    } else {
        int insertPosition = binarySearchInsertPosition(indexArray.dynamicArray, 0, indexArray.contactCounter - 1, id);

        // Shift elements to the right starting from the end of the array
        for (int i = indexArray.contactCounter - 1; i >= insertPosition; i--) {
            indexArray.dynamicArray[i + 1] = indexArray.dynamicArray[i];
        }

        // Insert the new value at the calculated position
        indexArray.dynamicArray[insertPosition] = tempContact;
    }

    indexArray.contactCounter += 1;
}

// Binary search function for ContactIndex array
int binarySearchContactIndex(ContactIndex arr[], int left, int right, int target) {
    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid].id == target)
            return mid;

        if (arr[mid].id < target)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return -1;
}

// Function to remove an element with a given ID
void removeElementById(int id) {
    // Check if the array is not empty
    if (indexArray.contactCounter > 0) {
        // Find the index of the element with the given ID
        int removeIndex = binarySearchContactIndex(indexArray.dynamicArray, 0, indexArray.contactCounter - 1, id);

        // If the element is found, remove it
        if (removeIndex != -1) {
            // Shift elements to the left to overwrite the removed element
            for (int i = removeIndex; i < indexArray.contactCounter - 1; i++) {
                indexArray.dynamicArray[i] = indexArray.dynamicArray[i + 1];
            }

            // Decrement the contactCounter
            indexArray.contactCounter--;

            // Optionally, you can shrink the array if needed
            // Note: This can be done based on your requirements and performance considerations
            // Here, we shrink the array when contactCounter becomes less than half the size
            if (indexArray.contactCounter < indexArray.size / 2) {
                indexArray.dynamicArray = realloc(indexArray.dynamicArray, (indexArray.size / 2) * sizeof(ContactIndex));
                indexArray.size = indexArray.size / 2;
            }


        } else {

        }
    } else {

    }
}

// Function to write ArrayIndex to a file
void writeArrayIndexToFile(FILE *file) {


    // Write the dynamic array size to the file
    fseek (file,0, SEEK_SET);
    fwrite(&(indexArray.contactCounter), sizeof(int), 1, file);

    // Write the contact counter to the file
    fseek (file,1 * sizeof(int), SEEK_SET);
    fwrite(&(indexArray.size), sizeof(int), 1, file);

    // Write the dynamic array separately
    fseek (file,2 * sizeof(int), SEEK_SET);
    fwrite(indexArray.dynamicArray, sizeof(ContactIndex), indexArray.contactCounter, file);
}

// Function to read ArrayIndex from a file
void readArrayIndexFromFile(FILE *file) {

    free(indexArray.dynamicArray);

    // Read the dynamic array size from the file
    fseek (file,0, SEEK_SET);
    fread(&(indexArray.contactCounter), sizeof(int), 1, file);

    // Read the contact counter to the file
    fseek (file,1 * sizeof(int), SEEK_SET);
    fread(&(indexArray.size), sizeof(int), 1, file);

    // Allocate memory for the dynamic array based on the read contactCounter
    indexArray.dynamicArray = malloc(indexArray.contactCounter * sizeof(ContactIndex));

    // Read the dynamic array separately
    fseek (file,2 * sizeof(int), SEEK_SET);
    fread(indexArray.dynamicArray, sizeof(ContactIndex), indexArray.contactCounter, file);

}





















