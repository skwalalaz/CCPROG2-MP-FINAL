#include "header.h"


/*
 * stripNewline - Removes trailing newline character from a string if present
 * @param str - pointer to the string to process
 * @return void (no return value)
 * Pre-condition: str must point to a valid null-terminated string
 */
void stripNewline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

/*
 * toLowerCase - Converts all characters in source string to lowercase and stores in destination
 * @param src - pointer to the source string to convert
 * @param dest - pointer to the destination buffer for lowercase string
 * @return void (no return value)
 * Pre-condition: src must point to valid string, dest must have sufficient space
 */
void toLowerCase(const char *src, char *dest) {
    while (*src) {
        *dest = tolower(*src);
        src++;
        dest++;
    }
    *dest = '\0';
}

/* 
 * stringAlreadyExists - Checks if a given string value already exists in an array of strings
 * @param list - array of strings to search through
 * @param count - number of valid entries in the list
 * @param value - string value to search for
 * @return int (1 if string exists, 0 if not found)
 * Pre-condition: list must contain valid string data, count must be accurate
 */
int stringAlreadyExists(StringName list[], int count, const char *value) {
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i], value) == 0) {
            return 1;  // found
        }
    }
    return 0;  // not found
}

/* 
 * findPriceRange - Determines the overall minimum and maximum price range from all establishments in directory
 * @param directory - pointer to the directory structure to analyze
 * @param min - pointer to store the minimum price found
 * @param max - pointer to store the maximum price found
 * @return void (no return value)
 * Pre-condition: directory must be initialized, min and max must point to valid integer variables
 */
void findPriceRange(const Directory *directory, int *min, int *max) {
    if (directory->numEntries == 0) {
        *min = *max = -1;
        return;
    }

    *min = directory->entry[0].Price.MinPrice;
    *max = directory->entry[0].Price.MaxPrice;

    for (int i = 1; i < directory->numEntries; i++) {
        int curMin = directory->entry[i].Price.MinPrice;
        int curMax = directory->entry[i].Price.MaxPrice;

        if (curMin < *min) *min = curMin;
        if (curMax > *max) *max = curMax;
    }
} 

// ----- CRUD Operation Helpers -----

/* 
 * userInputText - Prompts user for text input with validation to ensure non-empty input
 * @param label - string label to display as prompt
 * @param dest - destination buffer for user input
 * @param maxLen - maximum length allowed for input
 * @return void (no return value)
 * Pre-condition: dest must have space for maxLen characters, label must be valid string
 */
void userInputText(const char *label, char *dest, int maxLen) {
    printf("%s: ", label);
    fgets(dest, maxLen, stdin);
    stripNewline(dest);

    if (strlen(dest) == 0) {
        printf("Input cannot be empty.\n");
    }
}

/* 
 * userInputNum - Prompts user for numeric input and converts to integer
 * @param label - string label to display as prompt
 * @return int (converted integer value or -1 if empty input)
 * Pre-condition: label must be valid string
 */
int userInputNum(const char *label) {
    char buffer[20];
    printf("%s: ", label);
    fgets(buffer, sizeof(buffer), stdin);
    stripNewline(buffer);

    if (strlen(buffer) == 0) return -1;

    return atoi(buffer);
}

/* 
 * promptPriceRange - Prompts user for minimum and maximum price values with validation
 * @param price - pointer to PriceRange structure to populate
 * @return void (no return value)
 * Pre-condition: price must point to valid PriceRange structure
 */
void promptPriceRange(PriceRange *price) {
    int min, max;

    do {
        printf("Min Price: ");
        char buffer[20];
        fgets(buffer, sizeof(buffer), stdin);
        stripNewline(buffer);
        min = (strlen(buffer) > 0) ? atoi(buffer) : 0;  // Default to 0 if empty

        printf("Max Price: ");
        fgets(buffer, sizeof(buffer), stdin);
        stripNewline(buffer);
        max = (strlen(buffer) > 0) ? atoi(buffer) : 100;  // Default to 100 if empty

        if (min < 0 || max < 0 || max < min) {
            printf("Invalid price range. Please try again.\n");
        }
    } while (min < 0 || max < 0 || max < min);

    price->MinPrice = min;
    price->MaxPrice = max;
}

/* 
 * promptPopularItems - Prompts user to input popular items for an establishment with count validation
 * @param count - pointer to store number of items entered
 * @param items - array to store the popular item names
 * @return void (no return value)
 * Pre-condition: count must point to valid integer, items array must have space for MAX_ITEMS
 */
void promptPopularItems(int *count, StringName items[]) {
    int n = userInputNum("How many popular items? (max 5)");

    if (n < 0 || n > MAX_ITEMS) {
        printf("Invalid number of items.\n");
        *count = 0;
        return;
    }

    *count = n;

    for (int i = 0; i < n; i++) {
        char label[40];
        sprintf(label, "Item %d", i + 1);
        userInputText(label, items[i], MAX_LETTERS);
    }
}

// Matches a single entry against all filters (AND logic)

/*
 * filterMatch - Evaluates if an establishment matches all specified filter criteria using AND logic
 * @param e - pointer to establishment to evaluate
 * @param location - location filter string (empty to skip)
 * @param category - category filter string (empty to skip)
 * @param payment - payment option filter string (empty to skip)
 * @param serving - serving mode filter string (empty to skip)
 * @param userMin - minimum price filter (-1 to skip)
 * @param userMax - maximum price filter (-1 to skip)
 * @return int (1 if all filters match, 0 otherwise)
 * Pre-condition: e must point to valid establishment data
 */
int filterMatch(EstablishmentDetails *e, const char *location, const char *category, const char *payment, const char *serving, int userMin, int userMax){
    char loc1[MAX_LETTERS], loc2[MAX_LETTERS];
    char cat1[MAX_LETTERS], cat2[MAX_LETTERS];
    char pay1[MAX_LETTERS], pay2[MAX_LETTERS];
    char serve1[MAX_LETTERS], serve2[MAX_LETTERS];

    // Case-insensitive comparison
    toLowerCase(e->Location, loc1); toLowerCase(location, loc2);
    toLowerCase(e->FoodCateg, cat1); toLowerCase(category, cat2);
    toLowerCase(e->PaymentOpt, pay1); toLowerCase(payment, pay2);
    toLowerCase(e->ServingMode, serve1); toLowerCase(serving, serve2);

    if (strlen(location) > 0 && strcmp(loc1, loc2) != 0) return 0;
    if (strlen(category) > 0 && strcmp(cat1, cat2) != 0) return 0;
    if (strlen(payment) > 0 && strcmp(pay1, pay2) != 0) return 0;
    if (strlen(serving) > 0 && strcmp(serve1, serve2) != 0) return 0;

    int isValid = 1;

    if (userMin != -1) {
    if (e->Price.MinPrice >= userMin) {
        // OK
    } else {
        isValid = 0;
    }
    }

    if (userMax != -1) {
    if (e->Price.MaxPrice <= userMax) {
        // OK
    } else {
        isValid = 0;
    }
    }

    if (!isValid) return 0;


    return 1;  // All filters matched
}

// Displays available filter values (no duplicates)

/* 
 * printFilterHints - Displays available filter options by showing unique values from all establishments
 * @param directory - pointer to directory structure to analyze for filter options
 * @return void (no return value)
 * Pre-condition: directory must be initialized
 */
void printFilterHints(Directory *directory) {
    char locations[MAX_ENTRIES][MAX_LETTERS];
    char categories[MAX_ENTRIES][MAX_LETTERS];
    char payments[MAX_ENTRIES][MAX_LETTERS];
    char servings[MAX_ENTRIES][MAX_LETTERS];
    int locCount = 0, catCount = 0, payCount = 0, serveCount = 0;

    int minPrice = -1, maxPrice = -1;

    for (int i = 0; i < directory->numEntries; i++) {
        EstablishmentDetails *e = &directory->entry[i];

        if (!stringAlreadyExists(locations, locCount, e->Location)) {
            strcpy(locations[locCount++], e->Location);
        }

        if (!stringAlreadyExists(categories, catCount, e->FoodCateg)) {
            strcpy(categories[catCount++], e->FoodCateg);
        }

        if (!stringAlreadyExists(payments, payCount, e->PaymentOpt)) {
            strcpy(payments[payCount++], e->PaymentOpt);
        }

        if (!stringAlreadyExists(servings, serveCount, e->ServingMode)) {
            strcpy(servings[serveCount++], e->ServingMode);
        }

        if (e->Price.MinPrice > 0 && e->Price.MaxPrice > 0) {
            if (minPrice == -1 || e->Price.MinPrice < minPrice) {
                minPrice = e->Price.MinPrice;
            }
            if (maxPrice == -1 || e->Price.MaxPrice > maxPrice) {
                maxPrice = e->Price.MaxPrice;
            }
        }
    }

    int maxCount = locCount;
    if (catCount > maxCount) maxCount = catCount;
    if (payCount > maxCount) maxCount = payCount;
    if (serveCount > maxCount) maxCount = serveCount;

    printf("\n=================== FILTER HINTS ===================\n\n");
    printf("+------------------------+------------------------+------------------------+------------------------+\n");
    printf("| Location               | Category               | Payment Option         | Serving Mode           |\n");
    printf("+------------------------+------------------------+------------------------+------------------------+\n");

    for (int i = 0; i < maxCount; i++) {
        printf("| ");
        if (i < locCount)
            printf("%-22s", locations[i]);
        else
            printf("%-22s", "");

        printf(" | ");
        if (i < catCount)
            printf("%-22s", categories[i]);
        else
            printf("%-22s", "");

        printf(" | ");
        if (i < payCount)
            printf("%-22s", payments[i]);
        else
            printf("%-22s", "");

        printf(" | ");
        if (i < serveCount)
            printf("%-22s", servings[i]);
        else
            printf("%-22s", "");

        printf(" |\n");
    }

    printf("+------------------------+------------------------+------------------------+------------------------+\n");

    if (minPrice != -1 && maxPrice != -1) {
        printf("\nAvailable Price Range:\n");
        printf("  Minimum: %d\n", minPrice);
        printf("  Maximum: %d\n", maxPrice);
    }

    printf("\n====================================================\n");
}

// Prompts the user for all filters at once

/* 
 * promptFilters - Prompts user for filter criteria in either single or multiple filter mode
 * @param location - buffer to store location filter
 * @param category - buffer to store category filter
 * @param payment - buffer to store payment filter
 * @param serving - buffer to store serving mode filter
 * @param min - pointer to store minimum price filter
 * @param max - pointer to store maximum price filter
 * @param singleMode - flag indicating single filter mode (1) or multiple filter mode (0)
 * @return void (no return value)
 * Pre-condition: all buffer parameters must point to valid memory with sufficient space
 */
void promptFilters(char *location, char *category, char *payment, char *serving, int *min, int *max, int singleMode) {
    char buffer[20];

    if (singleMode) {
    int choice;
    printf("\n====== SINGLE FILTER OPTIONS ======\n");
    printf("1. Location\n");
    printf("2. Category\n");
    printf("3. Payment Option\n");
    printf("4. Serving Mode\n");
    printf("5. Price Range\n");
    printf("6. Back to User Mode\n");  // Added this line 
    printf("===================================\n\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
        case 1:
            printf("Location: ");
            fgets(location, MAX_LETTERS, stdin);
            stripNewline(location);
            break;

        case 2:
            printf("Category: ");
            fgets(category, MAX_LETTERS, stdin);
            stripNewline(category);
            break;

        case 3:
            printf("Payment Option: ");
            fgets(payment, MAX_LETTERS, stdin);
            stripNewline(payment);
            break;

        case 4:
            printf("Serving Mode: ");
            fgets(serving, MAX_LETTERS, stdin);
            stripNewline(serving);
            break;

        case 5:
            printf("Min Price (leave blank to skip): ");
            fgets(buffer, sizeof(buffer), stdin);
            stripNewline(buffer);
            if (strlen(buffer) > 0) {
                *min = atoi(buffer);
            } else {
                *min = -1;
            }

            printf("Max Price (leave blank to skip): ");
            fgets(buffer, sizeof(buffer), stdin);
            stripNewline(buffer);
            if (strlen(buffer) > 0) {
                *max = atoi(buffer);
            } else {
                *max = -1;
            }
            break;

        case 6:  // Added this case
            // Set all filters to empty/default so no results show
            strcpy(location, "");
            strcpy(category, "");
            strcpy(payment, "");
            strcpy(serving, "");
            *min = -1;
            *max = -1;
            printf("Returning to User Mode...\n");
            return;  // Exit the function early

        default:
            printf("Invalid choice.\n");
            break;
   	 }	
    } 
	else {
        printf("\n(Leave blank to skip a filter)\n");

        printf("Location: ");
        fgets(location, MAX_LETTERS, stdin);
        stripNewline(location);

        printf("Category: ");
        fgets(category, MAX_LETTERS, stdin);
        stripNewline(category);

        printf("Payment Option: ");
        fgets(payment, MAX_LETTERS, stdin);
        stripNewline(payment);

        printf("Serving Mode: ");
        fgets(serving, MAX_LETTERS, stdin);
        stripNewline(serving);

        printf("Min Price: ");
        fgets(buffer, sizeof(buffer), stdin);
        stripNewline(buffer);
        if (strlen(buffer) > 0) {
            *min = atoi(buffer);
        } else {
            *min = -1;
        }

        printf("Max Price: ");
        fgets(buffer, sizeof(buffer), stdin);
        stripNewline(buffer);
        if (strlen(buffer) > 0) {
            *max = atoi(buffer);
        } else {
            *max = -1;
        }
    }
}