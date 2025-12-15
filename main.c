#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <stdbool.h>

#define INV_FILE "inventory.inv"
//Global variables
int loginCount = 4; float main_commission = 0.05;

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
  int item_id;
} Sales;

//Functions
int getRandomSeed();
void decrypt(char *encrypted_text, int seed);
void encrypt(char *text, int seed);
bool matchString(const char *text, const char *pattern);
void getInput(char *output_dest, size_t output_size);
void getInputHidden(char *output_dest, size_t output_size);
void getInputValidChars(char *output_dest, size_t output_size);
void checkInvFile();
void registering();
void login(char *file_name);
void clearScr();
void clearCh();
void inventoryMenu();
void disAssemInv(FILE *file_pointer);
void addInventory(int item_id);
int getInvItemWithId(Inventory *item_to_modify, int id_what);
void deleteInv();
void searchModeInv(int end_inv);
void updateInvItem();
void ustpHeader();
void invUIHead();
void invUIlist(Inventory inventory);
void salesUIHead();
void salesUIlist(Sales *sale);
void salesMenu(char *file_name);
void addSale(char *file_name);
void updateSale(char *file_name);
void deleteSale(char *file_name);
void searchModeSale(char *file_name);
void updateSaleCount(char *file_name, int update_num);

int main(int arg_count, char *args[]){
  UINT oldcon = GetConsoleOutputCP(); SetConsoleOutputCP(CP_UTF8);

  if(arg_count >= 2){
    if(strcmp("-register", args[1]) == 0){
      registering();
    } else if(strcmp("-login", args[1]) == 0){
      if(arg_count >= 3) login(args[2]);
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
void clearCh(){
  while(_kbhit()) _getch();
}
//Get random seed lol
int getRandomSeed(){
  srand(time(NULL));
  int min = 2, max = 10;
  return min + (rand() % (max - min + 1));
}

//Search match method
bool matchString(const char *text, const char *pattern){
    // Case-insensitive, partial match
    if (!text || !pattern) return false;
    char lowerText[256], lowerPattern[256];
    int i;
    // Convert both strings to lowercase
    for (i = 0; text[i] && i < 255; i++)
        lowerText[i] = tolower((unsigned char)text[i]);
    lowerText[i] = '\0';
    for (i = 0; pattern[i] && i < 255; i++)
        lowerPattern[i] = tolower((unsigned char)pattern[i]);
    lowerPattern[i] = '\0';
    // Check if pattern is contained anywhere in text
    return strstr(lowerText, lowerPattern) != NULL;
}

//Cancel method
int triggerCancel(){
  printf("Are you sure about this? [ Y ]yes - [ N ]no ");
  int ch = _getch();
  if((char)ch == 'y' || (char)ch == 'Y') return 1;
  return -1;
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
    if(str_count >= output_size-1 && ch != 8) continue;

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
    if(str_count >= output_size-1 && ch != 8) continue;

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
    if(str_count >= output_size-1 && ch != 8) continue;

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
  fclose(f);
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
    //inventoryMenu();
    salesMenu(fileN);
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
    fclose(f);
    login(file_name);
  } else {
    printf("\nLogin attempts ran out.\n");
  }

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

//Ustp frfr
void ustpHeader(){
  printf("┌──────────────────────────────┬───────────────────────────────────────────┬───────┬─────────────────────────┐\n");
  printf("│                                                                                                            │\r");
  printf("│ USTP STOCKS INVENTORY SYSTEM │ Made by: VALMORIA, JOHN RUSHELL P.        │ IT1R1 │ Started on: 11/25/2025 \n");
  printf("└──────────────────────────────┴───────────────────────────────────────────┴───────┴─────────────────────────┘\n");
}

//Inventory methods
int getInvItemWithId(Inventory *item_to_modify, int id_what){
  checkInvFile();
  FILE *f = fopen(INV_FILE, "rb");
  if(f == NULL){
    printf("Error in opening in getInvItemWithId().\n");
    return -1;
  }

  Inventory temp; int foundOne = -1;
  while(fread(&temp, sizeof(Inventory), 1, f)){
    if(temp.item_id == id_what && temp.isExist){
      item_to_modify->item_id = temp.item_id;
      strcpy(item_to_modify->item_name, temp.item_name);
      item_to_modify->item_price = temp.item_price;
      item_to_modify->item_quantity = temp.item_quantity;
      item_to_modify->isExist = true;
      foundOne = 0;
      break;
    }
  }
  fclose(f);
  return foundOne;
}
void updateInvItem(){
  int chosen_id; char id_buff[8]; char choose[4];
  printf("\033[92mPut the item ID: ");
  getInputOnlyNum(id_buff, sizeof(id_buff));
  chosen_id = atoi(id_buff);

  FILE *f = fopen(INV_FILE, "rb+");
  if(f == NULL){
    printf("Error opening file in updateInvItem().");
    return;
  }

  Inventory item; bool founded = false;
  while(fread(&item, sizeof(item), 1, f)){
    if(item.item_id == chosen_id){
      founded = true;
      printf("\n");
      printf("Data found: %d, %s.\n", item.item_id, item.item_name);
      char quantBuff[8], priceBuff[15];
      
      //Item name update
      printf("[   ] Modify item name (yes/no).\r\033[1C");
      strcpy(choose, ""); getInputValidChars(choose, sizeof(choose));
      if(strcmp(choose, "yes") == 0){
        printf("\r\033[0K");
        printf("Change Item Name: ");
        getInputValidChars(item.item_name, sizeof(item.item_name));
      }
      printf("\n");

      //Item quantity update
      strcpy(choose, ""); printf("[   ] Modify item quantity (yes/no).\r\033[1C");
      getInputValidChars(choose, sizeof(choose));
      if(strcmp(choose, "yes") == 0){
        printf("\r\033[0K");
        printf("Change Item Quantity: ");
        getInputValidChars(quantBuff, sizeof(quantBuff));
        if(strlen(quantBuff) > 0) item.item_quantity = atoi(quantBuff);
      }
      printf("\n");

      //Item price update
      printf("[   ] Modify item price (yes/no).\r\033[1C");
      strcpy(choose, ""); getInputValidChars(choose, sizeof(choose));
      if(strcmp(choose, "yes") == 0){
        printf("\r\033[0K");
        printf("Change Item Price: ");
        getInputValidChars(priceBuff, sizeof(priceBuff));
        if(strlen(priceBuff) > 0) item.item_price = atof(priceBuff);
      }
      printf("\n");
      item.isExist = true;

      fseek(f, -(long)sizeof(item), SEEK_CUR);
      fwrite(&item, sizeof(item), 1, f); fclose(f);
      printf("\033[0m");
      break;
    }
  }

  if(!founded){
    printf("\nDid not find the item.\n");
    fclose(f);
    printf("\033[0m");
  }
}
void searchModeInv(int end_inv){
  char search[128] = "";
  int ch;

  FILE *f = fopen(INV_FILE, "rb");
  if(f == NULL){
    printf("Error opening file in searchModeInv().\n");
    return;
  }

  while (1){
    clearScr();
    invUIHead();
    Inventory inv;
    fseek(f, 0, SEEK_SET);
    char itemIdBuff[8];
    for(int i = 0; i < end_inv; i++){
      fread(&inv, sizeof(inv), 1, f);
      sprintf_s(itemIdBuff, sizeof(itemIdBuff), "%d", inv.item_id);
      bool inSearch = (matchString(inv.item_name, search)) ||
                      (matchString(itemIdBuff, search));
      if(inv.isExist && inSearch){
        invUIlist(inv);
      }
    }
    printf("└──────────────┴───────────────────────────────┴───────────────┴───────────────────┘\n\n");
    printf("SEARCH MODE (type to filter) | ENTER/ESC = exit\n");
    printf("Search: %s", search);

    ch = _getch();
    // EXIT SEARCH MODE
    if(ch == 27 || ch == '\r'){
      fclose(f);
      printf("\nEnding search mode...\n");
      return;
    }
    // BACKSPACE
    if(ch == 8){
      int len = strlen(search);
      if(len > 0){
        search[len - 1] = '\0';
      }
      continue;
    }
    // NORMAL CHAR INPUT
    if(isprint(ch)){
      int len = strlen(search);
      if(len < 127){
        search[len] = ch;
        search[len + 1] = '\0';
      }
    }
  }
}
void deleteInv(){
  int id_to_delete; char id_buff[8]; char choose[2];
  printf("\033[92mPut the item ID to delete: ");
  getInputOnlyNum(id_buff, sizeof(id_buff));
  printf("\n");
  id_to_delete = atoi(id_buff);
  
  FILE *f = fopen(INV_FILE, "rb+");
  if(f == NULL){
    printf("Error opening file in deleteInv().\n");
    return;
  }

  Inventory loop; bool isFound = false;
  while(fread(&loop, sizeof(loop), 1, f)){
    if(id_to_delete == loop.item_id){
      isFound = true;
      printf("Data found: %d, %s.\n", loop.item_id, loop.item_name);
      printf("[ ] Type '1' if(yes) / '0' if(no)\r[");
      getInputOnlyNum(choose, sizeof(choose));
      if(atoi(choose) == 1){
        loop.isExist = false;
        fseek(f, -(long)sizeof(loop), SEEK_CUR);
        fwrite(&loop, sizeof(loop), 1, f);
        printf("\nDeleted!\n");
        break;
      }
    }
  }
  if(!isFound) printf("Did not find the inventory data.");
  printf("\033[0m");
  fclose(f);
}
void addInventory(int item_id){
  FILE *f = fopen(INV_FILE, "ab");
  if(f == NULL){
    printf("Error opening file in addInventory().\n");
    return;
  }

  Inventory to_add;
  int sale_count;

  char priceBuff[15], quantBuff[8];
  to_add.item_id = item_id;
  printf("\033[92mEnter Item Name: ");
  getInputValidChars(to_add.item_name, sizeof(to_add.item_name));
  printf("\nEnter Item Quantity: ");
  getInputOnlyNum(quantBuff, sizeof(quantBuff));
  printf("\nEnter Item Price: ");
  getInputOnlyNum(priceBuff, sizeof(priceBuff));
  to_add.item_price = atof(priceBuff);
  to_add.item_quantity = atoi(quantBuff);
  to_add.isExist = true;

  //Cancel popup
  printf("\n");
  int isCanceled = triggerCancel();
  if(isCanceled != 1){
    printf("\nCanceled editing sale.\033[0m");
    fclose(f);
    return;
  }

  printf("\nWriting to file...\033[0m");
  fwrite(&to_add, sizeof(to_add), 1, f);
  fclose(f);
}
//Inventory creation menu
void inventoryMenu(){
  int sale_count, end_of_inv, count;
  Inventory inv;
  int ch;
  while(1){
    fflush(stdin);
    clearScr();
    invUIHead();
    checkInvFile();
    //Get the inv count
    end_of_inv = 0; count = 0;
    FILE *f = fopen(INV_FILE, "rb");
    if(f == NULL){ printf("Error opening file in inventoryMenu().\n"); return; }
    while(fread(&inv, sizeof(inv), 1, f)) end_of_inv++;

    fseek(f, 0, SEEK_SET);
    printf("\033[92m");
    for(int i = 0; i < end_of_inv; i++){
      fread(&inv, sizeof(inv), 1, f);
      if(inv.isExist){
        invUIlist(inv);
        count++;
      }
    }
    printf("└──────────────┴───────────────────────────────┴───────────────┴───────────────────┘\n\n");
    printf("\033[0m");
    fclose(f);
    
    printf("Total Items [ %d ]\n", count);
    printf("[ D ] Delete an item. \n");
    printf("[ A ] Add an item. \n");
    printf("[ U ] Update details of an item. \n");
    printf("[ S ] Search mode(supports ID, Name). \n");
    printf("[ R ] To refresh. \n");
    printf("[ Esc or Enter ] To exit. \n");
    ch = _getch();
    int methodDelay = 1200;
    if(ch == 27 || ch == 13){ printf("Exiting inventory menu.\n"); break; }
    switch((char)ch){
      case 'd':
        deleteInv();
        Sleep(methodDelay);
        break;
      case 'a':
        addInventory(end_of_inv);
        Sleep(methodDelay);
        break;
      case 'u':
        updateInvItem();
        Sleep(methodDelay);
        break;
      case 's':
        searchModeInv(end_of_inv);
        Sleep(methodDelay);
        break;
      case 'r':
        printf("Refresh\n");
        Sleep(500);
        break;
      default:
        printf("Invalid option!\n");
        Sleep(2000);
        break;
    }
    clearCh();
  }
}
void invUIHead(){
  printf("\033[96m");
  printf("┌──────────────┬───────────────────────────────┬───────────────┬───────────────────┐\n");
  printf("│ Item Code    │ Item Name                     │ Item Quantity │ Item Price        │\n");
  printf("├──────────────┼───────────────────────────────┼───────────────┼───────────────────┤\n");
  printf("\033[0m");
};
void invUIlist(Inventory inventory){
  //Buffers
  char id[13], quan[14], price[18];

  printf("│              │                               │               │                   │");
  printf("\r\033[2C");
  printf("\033[93m");
  sprintf_s(id, sizeof(id), "%d", inventory.item_id); printf("%s", id);
  printf("\r\033[17C");
  printf("%.29s", inventory.item_name);
  printf("\r\033[49C");
  sprintf_s(quan, sizeof(quan), "%d", inventory.item_quantity); printf("%s", quan);
  printf("\r\033[66C");
  sprintf_s(price, sizeof(price), "%.2f", inventory.item_price); printf("%s", price);
  printf("\033[92m");
  //printf("\033[1B\r");
  //printf("├──────────────┼───────────────────────────────┼───────────────┼───────────────────┤");
  printf("\033[1B\r");
}

//Sales type shit
void salesUIHead(){
  printf("\033[96m");
  printf("┌──────────┬───────────┬─────────────────────┬──────────┬────────────┬──────────┬──────────────┬────────────┐\n");
  printf("│ Sales ID │ Item Code │ Item Name           │ Quantity │ Price(PHP) │ Discount │ Total Amount │ Commission │\n");
  printf("├──────────┼───────────┼─────────────────────┼──────────┼────────────┼──────────┼──────────────┼────────────┤\n");
  printf("\033[0m");
}
void salesUIlist(Sales *sale){
  Inventory item;
  item.isExist = false;
  getInvItemWithId(&item, sale->item_id);
  if(!item.isExist) return;
  char salesIdBuff[9], itemCodeBuff[10], itemNameBuff[20], quanBuff[9];
  char priceBuff[11], discountBuff[9], totalBuff[13], comBuff[11];

  //Calculations
  sale->product_price = item.item_price;
  sale->total_value = (sale->product_quantity * sale->product_price) - sale->product_discount;
  sale->commission_total = sale->total_value * main_commission;
  strcpy(sale->product_name, item.item_name);

  printf("│          │           │                     │          │            │          │              │            │");
  printf("\033[92m");
  printf("\r\033[2C");
  sprintf_s(salesIdBuff, sizeof(salesIdBuff), "%d", sale->product_code); printf("%s", salesIdBuff);
  printf("\r\033[13C");
  sprintf_s(itemCodeBuff, sizeof(itemCodeBuff), "%d", item.item_id); printf("%s", itemCodeBuff);
  printf("\r\033[25C");
  printf("\033[30;48;5;82m");
  sprintf_s(itemNameBuff, sizeof(itemNameBuff), "%.19s", sale->product_name); printf("%s", itemNameBuff);
  printf("\033[0m");
  printf("\r\033[47C");
  printf("\033[92m");
  sprintf_s(quanBuff, sizeof(quanBuff), "%d", sale->product_quantity); printf("%s", quanBuff);
  printf("\r\033[58C");
  printf("\033[93m");
  sprintf_s(priceBuff, sizeof(priceBuff), "%.2f", sale->product_price); printf("%s", priceBuff);
  printf("\r\033[71C");
  sprintf_s(discountBuff, sizeof(discountBuff), "%.2f", sale->product_discount); printf("%s", discountBuff);
  printf("\r\033[82C");
  sprintf_s(totalBuff, sizeof(totalBuff), "%.2f", sale->total_value); printf("%s", totalBuff);
  printf("\r\033[97C");
  sprintf_s(comBuff, sizeof(comBuff), "%.2f", sale->commission_total); printf("%s", comBuff);
  printf("\r\033[1B");
  printf("\033[0m");
  printf("├──────────┼───────────┼─────────────────────┼──────────┼────────────┼──────────┼──────────────┼────────────┤\n");
}
void salesMenu(char *file_name){
  int ch, total_sales, count;
  float grand_total;
  while(1){
    clearScr();
    checkInvFile();
    ustpHeader();
    salesUIHead();
    grand_total = 0.00; count = 0;

    //Open file frfr
    FILE *f = fopen(file_name, "rb");
    if(f == NULL){
      printf("Error opening the damn file.");
      return;
    }
    fseek(f, sizeof(stock), SEEK_CUR);
    fread(&total_sales, sizeof(total_sales), 1, f);
    Sales sale_temp;
    for(int i = 0; i < total_sales; i++){
      fread(&sale_temp, sizeof(sale_temp), 1, f);
      if(sale_temp.isExist){
        count++;
        salesUIlist(&sale_temp);
        grand_total += sale_temp.total_value;
        grand_total += sale_temp.commission_total;
      }
    }
    //Grand total
    printf("├──────────┴───────────┴─────────────────────┴──────────┴────────────┴──────────┴──────────────┴────────────┤\n");
    printf("│                                                                                                           │\n");
    printf("└───────────────────────────────────────────────────────────────────────────────────────────────────────────┘");
    printf("\r\033[1A\033[2C");
    printf("\033[4m\033[92m");
    printf("Grand Total: %f\n\n", grand_total);
    printf("\033[0m");
    fclose(f);

    printf("\nTotal Items [ %d ]\n", count);
    printf("[ D ] Delete an sale record. \n");
    printf("[ A ] Add an sale record. \n");
    printf("[ U ] Update details of an sale record. \n");
    printf("[ S ] Search mode supports(Name, ID, Discount)\n");
    printf("[ I ] Go to inventory. \n");
    printf("[ R ] To refresh. \n");
    printf("[ Esc or Enter ] To exit. \n");
    ch = _getch();
    int methodDelay = 1300;
    if(ch == 27 || ch == 13){ printf("Exiting salamat sa pag gamit!\n"); break; }
    switch((char)ch){
      case 'd':
        deleteSale(file_name);
        Sleep(methodDelay);
        break;
      case 'a':
        addSale(file_name);
        Sleep(methodDelay);
        break;
      case 'u':
        updateSale(file_name);
        Sleep(methodDelay);
        break;
      case 's':
        searchModeSale(file_name);
        Sleep(methodDelay);
        break;
      case 'r':
        printf("Refresh\n");
        Sleep(500);
        break;
      case 'i':
        inventoryMenu();
        Sleep(methodDelay);
        break;
      default:
        printf("Invalid choice\n");
        Sleep(2000);
        break;
    }
    clearCh();
  }
}
//Sale methods
int getSaleCount(char *file_name){
  FILE *f = fopen(file_name, "rb");
  int result;
  if(f == NULL){
    printf("Error opening file for getSale().\n");
    return -1;
  }
  fseek(f, sizeof(stock), SEEK_CUR);
  fread(&result, sizeof(result), 1, f);
  fclose(f);
  return result;
}
void updateSaleCount(char *file_name, int update_num){
  FILE *f = fopen(file_name, "r+b");
  if(f == NULL){
    printf("Error opening sale file in updateSaleCount().\n");
    return;
  }
  fseek(f, sizeof(stock), SEEK_SET);
  fwrite(&update_num, sizeof(int), 1, f);
  fclose(f);
}
void addSale(char *file_name){
  FILE *f = fopen(file_name, "ab");
  if(f == NULL){
    printf("Error opening sale file in addSale().\n");
    return;
  }
  printf("\033[92m");
  int total_sales;
  total_sales = getSaleCount(file_name);
  Sales ap_sale; Inventory item_temp;

  char discountBuff[15], quantBuff[8], itemIdBuff[8];

  //Determine sale using ID bruh
  int ch;
  while(1){
    printf("Enter Item ID: ");
    getInputOnlyNum(itemIdBuff, sizeof(itemIdBuff));
    int invIsExist = getInvItemWithId(&item_temp, atoi(itemIdBuff));
    printf("\r\033[0K");
    if(invIsExist == 0){
      printf("Item exists [%s, %.2f] | Would you like to use this item? [ Y ]yes - [ N ]no ", item_temp.item_name, item_temp.item_price);
      ch = _getch();
      if((char)ch == 'y' || (char)ch == 'Y'){
        ap_sale.item_id = item_temp.item_id;
        printf("\n");
        break;
      } 
    } else {
        printf("Item does not exist | Would you like to cancel(yes)? Or enter an ID again(no). [ Y ]yes - [ N ]no ");
        ch = _getch();
        if((char)ch == 'y' || (char)ch == 'Y'){ printf("\nCanceled adding sale.\033[0m"); fclose(f); return; }
    }
    printf("\r\033[0K");
  }
  ap_sale.product_code = total_sales;
  printf("Enter Sale Quantity: ");
  getInputOnlyNum(quantBuff, sizeof(quantBuff));
  printf("\nEnter Sale Discount: ");
  getInputOnlyNum(discountBuff, sizeof(discountBuff));
  ap_sale.product_discount = atof(discountBuff);
  ap_sale.product_quantity = atoi(quantBuff);
  ap_sale.isExist = true;

  //Cancel popup
  printf("\n");
  int isCanceled = triggerCancel();
  if(isCanceled != 1){
    printf("\nCanceled adding sale.\033[0m");
    fclose(f);
    return;
  }

  printf("\nWriting data...\033[0m");
  fwrite(&ap_sale, sizeof(ap_sale), 1, f);
  fclose(f);
  updateSaleCount(file_name, total_sales+1);
}
void updateSale(char *file_name){
  int chosen_id; char id_buff[8]; char choose[4];
  printf("\033[92mPut the Sales ID: ");
  getInputOnlyNum(id_buff, sizeof(id_buff));
  chosen_id = atoi(id_buff);

  FILE *f = fopen(file_name, "rb+");
  if(f == NULL){
    printf("Error opening file in updateSale().");
    return;
  }
  fseek(f, sizeof(stock)+sizeof(int), SEEK_SET);

  Sales sale_temp;
  Inventory item; bool founded = false;
  while(fread(&sale_temp, sizeof(sale_temp), 1, f)){
    if(sale_temp.product_code == chosen_id){
      founded = true;
      getInvItemWithId(&item, sale_temp.item_id);
      printf("\nData found: %d, %s.\n", sale_temp.product_code , item.item_name);
      char quantBuff[8], discountBuff[15], itemIdBuff[8];
      
      //Sale id update
      printf("[   ] Change item (yes/no).\r\033[1C");
      strcpy(choose, ""); getInputValidChars(choose, sizeof(choose));
      if(strcmp(choose, "yes") == 0){
        printf("\r\033[0K");
        int ch; Inventory item_temp;
        while(1){
          printf("Enter Item ID: ");
          getInputOnlyNum(itemIdBuff, sizeof(itemIdBuff));
          int invIsExist = getInvItemWithId(&item_temp, atoi(itemIdBuff));
          printf("\r\033[0K");
          if(invIsExist == 0){
            printf("Item exists [%s, %.2f] | Would you like to use this item? [ Y ]yes - [ N ]no ", item_temp.item_name, item_temp.item_price);
            ch = _getch();
            if((char)ch == 'y' || (char)ch == 'Y'){
              sale_temp.item_id = item_temp.item_id;
              break;
            } 
          } else {
              printf("Item does not exist | Would you like to cancel(yes)? Or enter an ID again(no). [ Y ]yes - [ N ]no ");
              ch = _getch();
              if((char)ch == 'y' || (char)ch == 'Y'){ printf("\nCanceled adding sale.\033[0m"); fclose(f); return; }
          }
          printf("\r\033[0K");
        }
      }
      printf("\n");

      //Sale quantity update
      strcpy(choose, ""); printf("[   ] Modify sale quantity (yes/no).\r\033[1C");
      getInputValidChars(choose, sizeof(choose));
      if(strcmp(choose, "yes") == 0){
        printf("\r\033[0K");
        printf("Change Sale Quantity: ");
        getInputValidChars(quantBuff, sizeof(quantBuff));
        if(strlen(quantBuff) > 0) sale_temp.product_quantity = atoi(quantBuff);
      }
      printf("\n");

      //Sale discount update
      printf("[   ] Modify sale discount (yes/no).\r\033[1C");
      strcpy(choose, ""); getInputValidChars(choose, sizeof(choose));
      if(strcmp(choose, "yes") == 0){
        printf("\r\033[0K");
        printf("Change Sale Discount: ");
        getInputValidChars(discountBuff, sizeof(discountBuff));
        if(strlen(discountBuff) > 0) sale_temp.product_discount = atof(discountBuff);
      }
      printf("\n");
      sale_temp.isExist = true;
   
      //Cancel popup
      int isCanceled = triggerCancel();
      if(isCanceled != 1){
        printf("\nCanceled editing sale.\033[0m");
        fclose(f);
        return;
      }

      printf("\nWriting to file...\033[0m");
      fseek(f, -(long)sizeof(sale_temp), SEEK_CUR);
      fwrite(&sale_temp, sizeof(sale_temp), 1, f); fclose(f);
      break;
    }
  }

  if(!founded){
    printf("\nDid not find the item.\n");
    fclose(f);
    printf("\033[0m");
  }
}
void deleteSale(char *file_name){
  int id_to_delete; char id_buff[8]; char choose[2];
  printf("\033[92mPut the Sale ID to delete: ");
  getInputOnlyNum(id_buff, sizeof(id_buff));
  printf("\n");
  id_to_delete = atoi(id_buff);
  
  FILE *f = fopen(file_name, "rb+");
  if(f == NULL){
    printf("Error opening file in deleteSale().\n");
    return;
  }
  fseek(f, sizeof(stock)+sizeof(int), SEEK_SET);

  Sales loop; Inventory inv;
  bool founded = false;
  while(fread(&loop, sizeof(loop), 1, f)){
    if(id_to_delete == loop.product_code){
      founded = true;
      getInvItemWithId(&inv, loop.item_id);
      printf("Data found: %d, %s.\n", loop.product_code, inv.item_name);
      printf("[ ] Type '1' if(yes) / '0' if(no)\r[");
      getInputOnlyNum(choose, sizeof(choose));
      if(atoi(choose) == 1){
        loop.isExist = false;
        fseek(f, -(long)sizeof(loop), SEEK_CUR);
        fwrite(&loop, sizeof(loop), 1, f);
        printf("\nDeleted!\n");
        break;
      } else {
        printf("\nCanceled delete process.\n");
        break;
      }
    }
  }
  if(!founded){
    printf("Did not found the data.\n");
  }
  printf("\033[0m");
  fclose(f);
}
void searchModeSale(char *file_name){
  char search[128] = "";
  int ch;

  FILE *f = fopen(file_name, "rb");
  if(f == NULL){
    printf("Error opening file in searchModeSale().\n");
    return;
  }

  while (1){
    clearScr();
    salesUIHead();
    fseek(f, sizeof(stock)+sizeof(int), SEEK_SET);
    Sales sl; Inventory inv;
    char codeIdBuff[8], discountBuff[14];
    while(fread(&sl, sizeof(sl), 1, f)){
      getInvItemWithId(&inv, sl.item_id);
      sprintf_s(codeIdBuff, sizeof(codeIdBuff), "%d", sl.product_code);
      sprintf_s(discountBuff, sizeof(discountBuff), "%.2f", sl.product_discount);
      bool inSearch = (matchString(inv.item_name, search)) ||
                      (matchString(codeIdBuff, search)) || matchString(discountBuff, search);
      if(sl.isExist && inSearch){
        salesUIlist(&sl);
      }
    }
    printf("└──────────┴───────────┴─────────────────────┴──────────┴────────────┴──────────┴──────────────┴────────────┘\n");
    printf("SEARCH MODE (type to filter) |  [ ENTER/ESC ] = exit\n");
    printf("Search: %s", search);

    ch = _getch();
    // EXIT SEARCH MODE
    if(ch == 27 || ch == '\r'){
      fclose(f);
      printf("\nEnding search mode...\n");
      return;
    }
    // BACKSPACE
    if(ch == 8){
      int len = strlen(search);
      if(len > 0){
        search[len - 1] = '\0';
      }
      continue;
    }
    // NORMAL CHAR INPUT
    if(isprint(ch)){
      int len = strlen(search);
      if(len < 127){
        search[len] = ch;
        search[len + 1] = '\0';
      }
    }
  }
}
