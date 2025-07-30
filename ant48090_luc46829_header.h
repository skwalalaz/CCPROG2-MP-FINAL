#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>  

// Constants
#define MAX_LETTERS 31
#define MAX_NUM 12
#define MAX_ITEMS 5
#define MAX_ENTRIES 50

// Typedefs
typedef char StringName[MAX_LETTERS];
typedef char StringNum[MAX_NUM];

typedef struct {
    int MinPrice;
    int MaxPrice;
} PriceRange;

typedef struct {
    StringName Name;
    StringName Location;
    StringNum ContactNum;
    StringName ServingMode;
    StringName PaymentOpt;
    StringName FoodCateg;
    PriceRange Price;
    int numPopularItems;
    StringName popularItems[MAX_ITEMS];
} EstablishmentDetails;

typedef struct {
    EstablishmentDetails entry[MAX_ENTRIES];
    int numEntries;
} Directory;

// ----- Main Menu and Modes -----
void MainMenu(Directory *directory, char filename[]);
void AdminMode(Directory *directory);
void UserMode(Directory *directory);

// ----- CRUD Operations -----
void addEstablishment(Directory *directory);
void editEstablishment(Directory *directory);
void deleteEstablishment(Directory *directory);

// ----- Filters -----
void SingleFilter(Directory *directory);
void MultipleFilter(Directory *directory);

// ----- Display -----
void DisplayUserTable(EstablishmentDetails entries[], int numEntries);
void DisplayAdminTable(Directory *directory);  // Fixed parameter type

// ----- File Handling -----
void saveToFile(Directory *directory, char filename[]);
void loadFromFile(Directory *directory, char filename[]);
void handleSave(Directory *directory, char filename[]); // Function for file saving 

// ----- Helper Functions -----
void stripNewline(char *str); // Removes newline (\n) when reading
void toLowerCase(const char *src, char *dest); // Converts string to lowercase (used for case-insensitive match)
int stringAlreadyExists(StringName list[], int count, const char *value); // Checks if a given string already exists (avoids duplicates in listing down the filter guide)
void findPriceRange(const Directory *directory, int *min, int *max); // Gets the lowest minPrice and highest maxPrice

// ----- CRUD Operation Helpers -----
void userInputText(const char *label, char *dest, int maxLen);
int userInputNum(const char *label);
void promptPriceRange(PriceRange *price);
void promptPopularItems(int *count, StringName items[]);

// ----- Filter Helpers -----
int filterMatch(EstablishmentDetails *e,
                const char *location,
                const char *category,
                const char *payment,
                const char *serving,
                int userMin,
                int userMax); // Matches a single entry against all filters (AND logic)

void printFilterHints(Directory *directory); // Fixed function name and parameter
void promptFilters(char *location,
                    char *category,
                    char *payment,
                    char *serving,
                    int *min,
                    int *max,
                    int singleMode); // Prompts the user for all filters at once

#endif
