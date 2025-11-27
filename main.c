#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>

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
} Inventory;

typedef struct {
  int product_code;
  char product_name[50];
  int product_quantity;
  float product_price;
  float product_discount;
  float total_value;
  float commission_total;
} Sales;

//Functions
int getRandomSeed();
void decrypt(char *encrypted_text, int seed);
void encrypt(char *text, int seed);
void getInput(char *output_dest, size_t output_size);
void getInputHidden(char *output_dest, size_t output_size);
void getInputValidChars(char *output_dest, size_t output_size);
void registering();
void login(char *file_name);
void clearScr();
void inventoryMenu(FILE *file_pointer);
void disAssemInv(FILE *file_pointer);
void addInventory(FILE *file_pointer);
void invUIHead();

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

  return;
}

//Register stock process
void registering(){
  int inv_count = 0, sales_count = 0;
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
  fwrite(&inv_count, sizeof(inv_count), 1,f);
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
    inventoryMenu(f);
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

void addInventory(FILE *file_pointer){
  Inventory to_add;
  int inv_count, sale_count;

  fseek(file_pointer, sizeof(stock), SEEK_SET);
  fread(&inv_count, sizeof(inv_count), 1, file_pointer);
  fread(&sale_count, sizeof(sale_count), 1, file_pointer);

  int new_inv_count = inv_count + 1;
  char priceBuff[100], quantBuff[80];
  to_add.item_id = new_inv_count;
  printf("\033[92mEnter Item Name: \033[0m");
  getInputValidChars(to_add.item_name, sizeof(to_add.item_name));
  printf("\033[92m\nEnter Item Price: \033[0m");
  getInputOnlyNum(priceBuff, sizeof(priceBuff));
  printf("\033[92m\nEnter Item Quantity: \033[0m");
  getInputOnlyNum(quantBuff, sizeof(quantBuff));
  to_add.item_price = atof(priceBuff);
  to_add.item_quantity = atoi(quantBuff);
  long write_pos = sizeof(stock) + (sizeof(int) * 2) + (sizeof(Inventory) * (long)inv_count);

  fseek(file_pointer, write_pos, SEEK_SET);
  fwrite(&to_add, sizeof(to_add), 1, file_pointer);
  fseek(file_pointer, sizeof(stock), SEEK_SET);
  fwrite(&new_inv_count, sizeof(new_inv_count), 1, file_pointer);
}
//Inventory creation menu
void inventoryMenu(FILE *file_pointer){
  int inv_count, sale_count;
  Inventory inv;
  int ch;
  while(1){
    fseek(file_pointer, sizeof(stock), SEEK_SET);
    fread(&inv_count, sizeof(inv_count), 1, file_pointer);
    fread(&sale_count, sizeof(sale_count), 1, file_pointer);
    clearScr();
    for(int i = 0; i < inv_count; i++){
      fread(&inv, sizeof(inv), 1, file_pointer);
      printf("[ Item code ] == %d\n", inv.item_id);
      printf("[ Item name ] == %s\n", inv.item_name);
      printf("[ Item quantity ] == %d\n", inv.item_quantity);
      printf("[ Item price ] == %.2fPHP\n", inv.item_price);
      printf("\n");
    }
    printf("Total Items [ %d ]\n", inv_count);
    printf("[ D ] Delete an inventory item. \n");
    printf("[ A ] Add an inventory item. \n");
    printf("[ U ] Update details of an inventory item. \n");
    printf("[ Esc or Enter ] To exit. \n");
    ch = _getch();
    if(ch == 27 || ch == 13){ printf("Exiting salamat sa pag gamit!\n"); break; }
    switch((char)ch){
      case 'd':
        printf("Delete inventory option\n");
        break;
      case 'a':
        addInventory(file_pointer);
        break;
      case 'u':
        printf("Update inventory option\n");
        break;
      case 's':
        printf("Search option\n");
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
}
