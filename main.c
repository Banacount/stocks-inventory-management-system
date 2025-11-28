#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <stdbool.h>

#define INV_FILE "inventory.inv"
//Global variables
int loginCount = 4;

typedef struct {
  char userName[20];
  char userPassword[20];
  int seed; //For encryption
} stock;

typedef struct {
  int item_id;
  char item_name[50];
  int item_quantity;
  float item_price;
  bool isExist;
} Inventory;

typedef struct {
  int product_code;
  char product_name[50];
  int product_quantity;
  float product_price;
  float product_discount;
  float total_value;
  float commission_total;
  bool isExist;
} Sales;

//Functions
int getRandomSeed();
void decrypt(char *encrypted_text, int seed);
void encrypt(char *text, int seed);
void getInput(char *output_dest, size_t output_size);
void getInputHidden(char *output_dest, size_t output_size);
void getInputValidChars(char *output_dest, size_t output_size);
void checkInvFile();
void registering();
void login(char *file_name);
void clearScr();
void inventoryMenu();
void disAssemInv(FILE *file_pointer);
void addInventory(int item_id);
void deleteInv();
void invUIHead();
void invUIlist(Inventory inventory);

int main(int arg_count, char *args[]){
  UINT oldcon = GetConsoleOutputCP(); SetConsoleOutputCP(CP_UTF8);

  if(arg_count >= 2){
    if(strcmp("-register", args[1]) == 0){
      registering();
    } else if(strcmp("-login", args[1]) == 0){
      if(arg_count >= 3)login(args[2]);
    } else {
      printf("Argument not found..\n");
    }
  }

  SetConsoleOutputCP(oldcon);
  return 0;
}

//Clear screen frfr
void clearScr(){
  system("cls");
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
//Sigma male input
void getInputHidden(char *output_dest, size_t output_size){
  int ch, str_count;
  strcpy(output_dest, "");
  str_count = 0;

  while(1){
    ch = _getch();
    if(ch == 27 || ch == 13) break;
    if(str_count > output_size-1 && ch != 8) continue;

    if(ch == 8){
      //Handle backspace frfr
      if(str_count > 0){
        output_dest[str_count-1] = '\0';
        str_count--;
        printf("\b \b");
      }
    } else if(ch >= 32 && ch <= 126){
      output_dest[str_count] = (char)ch;
      printf("*");
      str_count++;
    }
    output_dest[str_count] = '\0';
  }

  return;
}
void getInputValidChars(char *output_dest, size_t output_size){
  int ch, str_count;
  strcpy(output_dest, "");
  str_count = 0;

  while(1){
    ch = _getch();
    if(ch == 27 || ch == 13) break;
    if(str_count > output_size-1 && ch != 8) continue;

    if(ch == 8){
      //Handle backspace frfr
      if(str_count > 0){
        output_dest[str_count-1] = '\0';
        str_count--;
        printf("\b \b");
      }
    } else if(ch >= 32 && ch <= 126){
      output_dest[str_count] = (char)ch;
      printf("%c", (char)ch);
      str_count++;
    }
    output_dest[str_count] = '\0';
  }

  return;
}
void getInputOnlyNum(char *output_dest, size_t output_size){
  int ch, str_count;
  strcpy(output_dest, "");
  str_count = 0;

  while(1){
    ch = _getch();
    if(ch == 27 || ch == 13) break;
    if(str_count > output_size-1 && ch != 8) continue;

    if(ch == 8){
      //Handle backspace frfr
      if(str_count > 0){
        output_dest[str_count-1] = '\0';
        str_count--;
        printf("\b \b");
      }
    } else if((ch >= 48 && ch <= 57) || ch == 46){
      output_dest[str_count] = (char)ch;
      printf("%c", (char)ch);
      str_count++;
    }
    output_dest[str_count] = '\0';
  }
}
void checkInvFile(){
  FILE *f = fopen(INV_FILE, "rb");
  if(f == NULL){
    f = fopen(INV_FILE, "wb");
    fclose(f);
    return;
  }
}

//Register stock process
void registering(){
  int sales_count = 0;
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

  FILE *f = fopen(file_name, "wb");
  if(f == NULL){
    printf("File creation error\n");
    return;
  }
  fwrite(&account, sizeof(account), 1,f);
  fwrite(&sales_count, sizeof(sales_count), 1,f);
  fclose(f);
  printf("Register process done!\n");
  return;
}

//Still justs prints sum info not actually "loging in"
void login(char *file_name){
  stock account; stock cmp;
  char fileN[406];
  sprintf_s(fileN, sizeof(fileN),"%s.stock", file_name);

  FILE *f = fopen(fileN, "rb+");
  if(f == NULL){
    printf("File probably doesn't exist");
    return;
  }

  fread(&account, sizeof(account), 1, f);
  printf("Enter username: ");
  getInput(cmp.userName, sizeof(cmp.userName));
  printf("Enter password: ");
  getInputHidden(cmp.userPassword, sizeof(cmp.userPassword));

  //Decrypt the account fr
  decrypt(account.userName, account.seed);
  decrypt(account.userPassword, account.seed);

  int isUser = strcmp(account.userName, cmp.userName);
  int isPass = strcmp(account.userPassword, cmp.userPassword);
  if(isUser == 0 && isPass == 0){
    inventoryMenu();
  } else if(loginCount > 0){
    clearScr();
    loginCount--;
    if(loginCount > 0){
      printf("\033[91m\033[5m");
      printf("Wrong password or username! Login attempt left is %d.\n", loginCount);
      printf("\033[0m");
    } else {
      printf("Last try.\n");
    }
    login(file_name);
  } else {
    printf("\nLogin attempts ran out.\n");
  }

  fclose(f);
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

//Inventory methods
void deleteInv(){
  int id_to_delete; char id_buff[7]; char choose[1];
  printf("Put the item ID: ");
  getInputOnlyNum(id_buff, sizeof(id_buff));
  printf("\n");
  id_to_delete = atoi(id_buff);
  
  FILE *f = fopen(INV_FILE, "rb+");
  if(f == NULL){
    printf("Error opening file.\n");
    return;
  }

  Inventory loop;
  while(fread(&loop, sizeof(loop), 1, f)){
    if(id_to_delete == loop.item_id){
      printf("\033[92m");
      printf("Data found: %d, %s\n", loop.item_id, loop.item_name);
      printf("[ ] Type 1_if(yes) / 0_if(no)\r[");
      getInputOnlyNum(choose, sizeof(choose));
      if(atoi(choose) == 1){
        loop.isExist = false;
        fseek(f, sizeof(loop)*-1, SEEK_CUR);
        fwrite(&loop, sizeof(loop), 1, f);
        printf("\nDeleted!\n");
        Sleep(1000);
        break;
      }
    }
  }
  printf("\033[0m");
  fclose(f);
}
void addInventory(int item_id){
  FILE *f = fopen(INV_FILE, "ab");
  if(f == NULL){
    printf("Error opening file.\n");
    return;
  }

  Inventory to_add;
  int sale_count;

  char priceBuff[14], quantBuff[7];
  to_add.item_id = item_id;
  printf("\033[92mEnter Item Name: \033[0m");
  getInputValidChars(to_add.item_name, sizeof(to_add.item_name));
  printf("\033[92m\nEnter Item Quantity: \033[0m");
  getInputOnlyNum(quantBuff, sizeof(quantBuff));
  printf("\033[92m\nEnter Item Price: \033[0m");
  getInputOnlyNum(priceBuff, sizeof(priceBuff));
  to_add.item_price = atof(priceBuff);
  to_add.item_quantity = atoi(quantBuff);
  to_add.isExist = true;

  fwrite(&to_add, sizeof(to_add), 1, f);
  fclose(f);
}
//Inventory creation menu
void inventoryMenu(){
  int sale_count, end_of_inv;
  Inventory inv;
  int ch;
  while(1){
    fflush(stdin);
    clearScr();
    invUIHead();
    checkInvFile();
    //Get the inv count
    end_of_inv = 0;
    FILE *f = fopen(INV_FILE, "rb");
    if(f == NULL){ printf("Error opening file.\n"); return; }
    while(fread(&inv, sizeof(inv), 1, f)) end_of_inv++;

    fseek(f, 0, SEEK_SET);
    for(int i = 0; i < end_of_inv; i++){
      fread(&inv, sizeof(inv), 1, f);
      if(inv.isExist) invUIlist(inv);
    }
    printf("└──────────────┴─────────────────────┴───────────────┴───────────────────┘\n\n");
    fclose(f);
    
    printf("Total Items [ %d ]\n", end_of_inv);
    printf("[ D ] Delete an inventory item. \n");
    printf("[ A ] Add an inventory item. \n");
    printf("[ U ] Update details of an inventory item. \n");
    printf("[ R ] To refresh. \n");
    printf("[ Esc or Enter ] To exit. \n");
    ch = _getch();
    if(ch == 27 || ch == 13){ printf("Exiting salamat sa pag gamit!\n"); break; }
    switch((char)ch){
      case 'd':
        deleteInv();
        break;
      case 'a':
        addInventory(end_of_inv);
        break;
      case 'u':
        printf("Update inventory option\n");
        break;
      case 's':
        printf("Search option\n");
        break;
      case 'r':
        printf("Refresh\n");
        Sleep(500);
        break;
      default:
        printf("Invalid option!\n");
        break;
    }
  }
}
void invUIHead(){
  printf("┌──────────────┬─────────────────────┬───────────────┬───────────────────┐\n");
  printf("│ Item Code    │ Item Name           │ Item Quantity │ Item Price        │\n");
  printf("├──────────────┼─────────────────────┼───────────────┼───────────────────┤\n");
};
void invUIlist(Inventory inventory){
  //Buffers
  char id[13], quan[14], price[18];

  printf("│              │                     │               │                   │");
  printf("\r\033[2C");
  sprintf_s(id, sizeof(id), "%d", inventory.item_id); printf("%s", id);
  printf("\r\033[17C");
  printf("%.19s", inventory.item_name);
  printf("\r\033[39C");
  sprintf_s(quan, sizeof(quan), "%d", inventory.item_quantity); printf("%s", quan);
  printf("\r\033[56C");
  sprintf_s(price, sizeof(price), "%.2f", inventory.item_price); printf("%s", price);
  printf("\033[1B\r");
  printf("├──────────────┼─────────────────────┼───────────────┼───────────────────┤");
  printf("\033[1B\r");
}
