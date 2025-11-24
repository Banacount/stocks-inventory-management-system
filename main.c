#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//Global variables
int loginCount = 0;

typedef struct {
  char userName[20];
  char userPassword[20];
  int seed; //For encryption
} stock;

//Functions
int getRandomSeed();
void decrypt(char *encrypted_text, int seed);
void encrypt(char *text, int seed);
void getInput(char *output_dest, size_t output_size);
void registering();
void login(char *file_name);

int main(int arg_count, char *args[]){
  if(arg_count >= 2){
    if(strcmp("-register", args[1]) == 0){
      registering();
    } else if(strcmp("-login", args[1]) == 0){
      if(arg_count >= 3)login(args[2]);
    } else {
      printf("Argument not found..\n");
    }
  }
  return 0;
}

//Get random seed lol
int getRandomSeed(){
  srand(time(NULL));
  int min = 2, max = 10;
  return min + (rand() % (max - min + 1));
}

//Get user input faster lmao
void getInput(char *output_dest, size_t output_size){
  fgets(output_dest, output_size, stdin);
  output_dest[strlen(output_dest)-1]='\0';
  fflush(stdin);
  return;
}

//Register stock process
void registering(){
  stock account;
  char init_txt[20]; char file_name[30];
  printf("Enter File Name: ");
  getInput(init_txt, sizeof(init_txt));
  sprintf_s(file_name, sizeof(file_name), "%s.stock", init_txt);
  account.seed = getRandomSeed();

  printf("Enter Username: ");
  getInput(account.userName, sizeof(account.userName));
  encrypt(account.userName, account.seed);
  printf("Enter password: ");
  getInput(account.userPassword, sizeof(account.userPassword));
  encrypt(account.userPassword, account.seed);

  FILE *file = fopen(file_name, "wb");
  if(file == NULL){
    printf("File creation error\n");
    return;
  }
  fwrite(&account, sizeof(account), 1, file);
  printf("Register process done!\n");
  return;
}

//Still justs prints sum info not actually "loging in"
void login(char *file_name){
  stock account;
  FILE *file = fopen(file_name, "rb");
  if(file == NULL){
    printf("File probably doesn't exist");
    return;
  }

  fread(&account, sizeof(account), 1, file);
  printf("Username %s, Password %s. Seed is %d.\n", account.userName, account.userPassword, account.seed);
  return;
}

//Most advanced decrypt and encrypt mechanism of all time.
void decrypt(char *encrypted_text, int seed){
  for(int c = 0; c < strlen(encrypted_text); c++){
    encrypted_text[c] = (int)encrypted_text[c] - seed;
  }
}
void encrypt(char *text, int seed){
  for(int c = 0; c < strlen(text); c++){
    text[c] = (int)text[c] + seed;
  }
}
