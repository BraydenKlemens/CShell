/*
Brayden Klemens
Feb 2019
*/

#include "ish.h"

int main(){

    //vars
    char hostName[255] = "\0";
    char *hName = malloc(sizeof(char)*255);
    char *uName = malloc(sizeof(char)*255);
    //FILE *input;
    FILE *output;
    FILE *input;
    char *fileName = NULL;

    if(uName == NULL || hName == NULL){
        free(hName);
        free(uName);
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    //init
    strcpy(hName, "");
    strcpy(uName, "");

    //const char *mypath[] = { "./", "/usr/bin/", "/bin/", NULL};
    uid_t userID;
    struct passwd *pwd;

    //gt the host name
    if(gethostname(hostName, 255) == 0){
        strcpy(hName,hostName);
    }else{
        //some error code
        free(hName);
        free(uName);
        fprintf(stderr,"usage: %s: ENAMETOOLONG\n", "ish.c");
        exit(EXIT_FAILURE);
    }

    //get the user name
    userID = getuid();
    pwd = getpwuid(userID);

    if(pwd){
        strcpy(uName,pwd->pw_name);
    }else{
        free(hName);
        free(uName);
        fprintf(stderr,"usage: %s: cannot find username for UID %u\n", "ish.c", (unsigned) userID);
        exit(EXIT_FAILURE);
    }

    //run the shell -  get input - parse input - run
    char *userInput = malloc(sizeof(char)*1000);
    char *tempInput = malloc(sizeof(char)*1000);
    char **args = NULL;
    pid_t child;
    int wstatus;

    if(userInput == NULL || tempInput == NULL){
        free(hName);
        free(uName);
        free(userInput);
        free(tempInput);
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    
    while (1) {

        // Wait for input
        if(strcmp(uName, "root") == 0){
            printf("[%s@%s]# ", uName, hName);
        }else{
            printf("[%s@%s]$ ", uName, hName);
        }

        //get input
        fgets(userInput, 1000, stdin);

        //remove the new line
        if(userInput[strlen(userInput) - 1] == '\n'){
            userInput[strlen(userInput) - 1] = '\0';
        }

        //used for args
        strcpy(tempInput, userInput);

        //If necessary locate executable using mypath array 
        //parse arguments into an array
        args = parseInput(userInput);

        //get length of 2d array
        int length = 0;
        while(args[length] != NULL){
            length++;
        }

        //check for exit command
        if(strcmp(args[0], "exit") == 0){
            free(hName);
            free(uName);
            free(userInput);
            free(tempInput);
            exit(EXIT_SUCCESS);
        }else if(strcmp(args[0], "gcd") == 0){
            printf("GCD(%s, %s) = %d\n", args[1], args[2], gcdCommand(args[1], args[2]));
        }else if(strcmp(args[0], "args") == 0){
            argsCommand(tempInput);
        }else if(strcmp(args[0], "b2d") == 0){
            printf("%s in decimal is: %.2f\n", args[1], B2D(args[1]));
        }else if(strcmp(args[0], "cd") == 0){
            if(chdir(args[1]) < 0){
                perror(args[1]);
            }
        }else{

            //Launch executable
            child = fork();
            if(child < 0){
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            if(child == 0){

                for(int i = 0; i < length; i++){
                    if(strcmp(args[i],">") == 0){
                        fileName = getStreamName(args);
                        output = freopen(fileName, "w+", stdout);
                        execvp(args[0], args);
                        fclose(output);
                    }
                    if(strcmp(args[i],"<") == 0){
                        fileName = getStreamName(args);
                        input = freopen(fileName, "r", stdin);
                        execvp(args[0], args);
                        fclose(input);
                    }
                }

                if(execvp(args[0],args) < 0){
                    perror(args[0]);
                    exit(EXIT_FAILURE);
                }

            }else{
                wait(&wstatus);
            } 
        }
        //end of while
    }

    return 0;
}

char *getStreamName(char **args){

    int i = 0;
    char *fileName = malloc(sizeof(char)*50);

    while(args[i] != NULL){
        if(strcmp(args[i], ">" ) == 0){
            strcpy(fileName, args[i + 1]);
            args[i + 1] = NULL;
            args[i] = NULL;
            return fileName;
        }
        if(strcmp(args[i], "<") == 0){
            strcpy(fileName, args[i + 1]);
            args[i] = args[i + 1];
            args[i + 1] = NULL;
            return fileName;
        }
        i++;
    }

    return "";
}

void argsCommand(char *input){

    char *token = NULL;
    char *temp1 = malloc(sizeof(char)*strlen(input));
    char **args = malloc(sizeof(char*)*50);
    int index = 0;

    for(int i = 0; i < strlen(input); i++){
        args[i] = malloc(sizeof(char)*50);
    }

    token = strtok(input, " ");

    while(token != NULL){
        if(token[0] == '"'){
            if(token[strlen(token) - 1] != '"'){
                strcpy(temp1, token);
                token = strtok(NULL, "\"");
                strcat(temp1, " ");
                strcat(temp1, token);
                strcat(temp1, "\"");
                strcpy(args[index], temp1);
                index++;
            }else{
                strcpy(args[index], token);
                index++;
            }
        }else{
            if(strcmp(token,"args") != 0){
                strcpy(args[index], token);
                index++;
            }
        }
        token = strtok(NULL, " ");
    }

    args[index] = NULL;

    printf("argc = %d, args = ", index);

    int i = 0;
    while(args[i] != NULL){
        if(args[i + 1] == NULL){
            printf("%s\n", args[i]);
        }else{
            printf("%s, ", args[i]);
        }
        i++;
    }

    //free vars
    free(temp1);
    for(int i = 0; i < strlen(input); i++){
        free(args[i]);
    }
    free(args);
}


char** parseInput(char *input){

    char *token = NULL;
    char **args = malloc(sizeof(char*) * 50);
    int index = 0;

    if(args == NULL){
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 50; i++){
        args[i] = malloc(sizeof(char) * 50);
        if(args[i] == NULL){
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
    }

    //Parse input 
    while ((token = strsep(&input, " ")) != NULL){
        strcpy(args[index], token);
        index++;
    }

    args[index] = NULL;

    return args;

}

int hexToDec(char *string){

    double mult = 0;
    double conversion = 0;
    double total = 0;
    double power = strlen(string) - 1;

    for(int i = 0; i < strlen(string); i++){

        //get the multiplier
        if(isdigit(string[i])){
            mult = string[i] - '0';
        }else{
            if(string[i] == 'a' || string [i] == 'A'){
                mult = 10;
            }else if(string[i] == 'b' || string [i] == 'B'){
                mult = 11;
            }else if(string[i] == 'c' || string [i] == 'C'){
                mult = 12;
            }else if(string[i] == 'd' || string [i] == 'D'){
                mult = 13;
            }else if(string[i] == 'e' || string [i] == 'E'){
                mult = 14;
            }else if(string[i] == 'f' || string [i] == 'F'){
                mult = 15;
            }else if(string[i] == 'x' || string [i] == 'X'){
                mult = 0;
            }else{
                fprintf(stderr,"usage: %s: hexString\n", "ish.c");
                exit(EXIT_FAILURE);
            }
        }

        conversion = (mult * pow(16, power));
        total += conversion;
        power--;
    }

    if(!(total <= 2147483647)){
        fprintf(stderr,"usage: %s: hexString\n", "ish.c");
        exit(EXIT_FAILURE);
    }

    return (int)total;
}

int gcd(int num1, int num2){

    if (num1 == 0){
        return num2; 
    }
    if (num2 == 0) {
        return num1; 
    }

    // base case 
    if (num1 == num2) {
        return num1; 
    }

     // a is greater 
    if (num1 > num2) {
        return gcd(num1-num2, num2);
    }else{
        return gcd(num1, num2-num1);
    }  

}

int gcdCommand(char *arg1, char *arg2){

    int num1 = 0;
    int num2  = 0;
    int result = 0;

    //assume if it has a hex letter its converted to dec
    //if it is already decimal then its decimal
    if(isNum(arg1)){
        if(atof(arg1) <= 2147483647){
            num1 = atoi(arg1);
        }else{
            fprintf(stderr,"usage: %s: GCDarg1\n", "ish.c");
            exit(EXIT_FAILURE);
        }
    }else if(isHex(arg1)){//test max value
        num1 = hexToDec(arg1);

    }else{
        fprintf(stderr,"usage: %s: GCDarg1\n", "ish.c");
        exit(EXIT_FAILURE);
    }

    if(isNum(arg2)){
        if(atof(arg2) <= 2147483647){
            num2 = atoi(arg2);
        }else{
            fprintf(stderr,"usage: %s: GCDarg2\n", "ish.c");
            exit(EXIT_FAILURE);
        }
    }else if(isHex(arg2)){
        num2 = hexToDec(arg2);
    }else{
        fprintf(stderr,"usage: %s: GCDarg2\n", "ish.c");
        exit(EXIT_FAILURE);
    }

    result = gcd(num1, num2);

    return result;
}

bool isNum(char *string){
    for(int i = 0; i < strlen(string); i++){
        if(!(isdigit(string[i]))){
            return false;
        }
    }

    return true;
}

bool isHex(char *string){

    //go through check if each char is a hex char
    for(int i = 0; i < strlen(string); i++){
        if(string[i] == 'x'){
            i++;
        }
        if(!(isxdigit(string[i]))){
            return false;
        }
    }

    return true;
}

//calc function
double B2D(char *arg1){

    double length = strlen(arg1) - 1;
    double total = 0;

    for(int i = 0; i < strlen(arg1); i++){
        if(checkBinary(arg1)){
            if(arg1[i] == '1'){
                total += pow(2,length);
                length--;
            }else{
                length--;
            }
        }else{
            fprintf(stderr,"usage: %s: B2Darg1\n", "ish.c");
            exit(EXIT_FAILURE);
        }
    }
    return total;
}

bool checkBinary(char *arg1){

    for(int i = 0; i < strlen(arg1); i++){
        if(!(arg1[i] == '1' || arg1[i] == '0')){
            return false;
        }
    }

    return true;
}








