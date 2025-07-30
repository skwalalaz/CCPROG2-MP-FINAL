#include "ant48090_luc46829_header.h"

// ----- Main Menu and Modes -----

/*
 * MainMenu - Displays the main menu interface and handles user navigation between different modes and file operations
 * @param directory - pointer to the directory structure containing establishment data
 * @param filename - string array to store the current filename for load/save operations
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized
 */
void MainMenu(Directory *directory, char filename[]){
    int choice;
    int running = 1; 
    
    while(running) {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Admin Mode\n");
        printf("2. User Mode\n");
        printf("3. Load from File\n");
        printf("4. Save to File \n");
        printf("5. Exit\n");
        printf("================================\n\n");
        printf("--INPUT NUMBER ONLY--\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();  // to remove leftover newline

        switch (choice) {
            case 1:
                AdminMode(directory);
                break;

            case 2:
                UserMode(directory);
                break;

            case 3: {
                printf("Enter filename to load: ");
                fgets(filename, MAX_LETTERS, stdin);
                stripNewline(filename);
                loadFromFile(directory, filename);
                printf("Loaded from '%s'\n", filename);
                break;
            }

            case 4:
                handleSave(directory, filename);
                break;

            case 5: {
                int saveChoice;
                
                // Input validation loop for 1 or 0 only
                do {
                    printf("Save before exiting? (1 = Yes / 0 = No): ");
                    scanf("%d", &saveChoice);
                    getchar();
                    
                    if (saveChoice != 0 && saveChoice != 1) {
                        printf("Invalid input! Please enter 1 for Yes or 0 for No.\n");
                    }
                } while (saveChoice != 0 && saveChoice != 1);
                
                if (saveChoice == 1) {
                    handleSave(directory, filename);
                }

                running = 0;  // Exit loop
                break;
            }

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    printf("Exiting program. Goodbye!\n");
}


/*
 * AdminMode - Provides administrative interface for CRUD operations on establishment data
 * @param directory - pointer to the directory structure for data manipulation
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized
 */
void AdminMode(Directory *directory){
    int choice;
    int running = 1;

    while(running) {
        printf("\n========== ADMIN MODE ==========\n");
        printf("1. Add Establishment\n");
        printf("2. Edit Establishment\n");
        printf("3. Delete Establishment\n");
        printf("4. Back to Main Menu\n");
        printf("================================\n\n");
        printf("--INPUT NUMBER ONLY--\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();  // to remove leftover newline

        switch(choice) {
            case 1:
                addEstablishment(directory);
                break;
            case 2:
                editEstablishment(directory);
                break;
            case 3:
                deleteEstablishment(directory);
                break;
            case 4:
                running = 0;
                continue;
            default:
                printf("Invalid Input\n");
                continue;
        }

        // After action prompt with input validation
        int again;
        do {
            printf("Do you want to perform another action in Admin Mode? (1 = Yes / 0 = No): ");
            scanf("%d", &again);
            getchar();
            
            if (again != 0 && again != 1) {
                printf("Invalid input! Please enter 1 for Yes or 0 for No.\n");
            }
        } while (again != 0 && again != 1);
        
        if (again == 0) running = 0;
    }
}

/* 
 * UserMode - Provides user interface for filtering and viewing establishment data
 * @param directory - pointer to the directory structure for data retrieval
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized
 */
void UserMode(Directory *directory){
    int choice;
    int running = 1;
    
    while (running) {
        printf("\n========== USER MODE ==========\n");
        printf("1. Single Filter\n");
        printf("2. Multiple Filter\n");
        printf("3. Back to Main Menu\n");
        printf("================================\n\n");
        printf("--INPUT NUMBER ONLY--\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();  // remove newline

        switch (choice) {
            case 1:
                SingleFilter(directory);
                break;
            case 2:
                MultipleFilter(directory);
                break;
            case 3:
                running = 0;
                continue;
            default:
                printf("Invalid Input\n");
                continue;
        }

        // Input validation loop for 1 or 0 only
        int again;
        do {
            printf("Do you want to perform another action in User Mode? (1 = Yes / 0 = No): ");
            scanf("%d", &again);
            getchar();
            
            if (again != 0 && again != 1) {
                printf("Invalid input! Please enter 1 for Yes or 0 for No.\n");
            }
        } while (again != 0 && again != 1);
        
        if (again == 0) running = 0;
    }
}

// ----- CRUD Operations -----

/* 
 * addEstablishment - Adds a new establishment entry to the directory with user input validation
 * @param directory - pointer to the directory structure where new entry will be added
 * @return void (no return value)
 * Pre-condition: Directory must not be full (numEntries < MAX_ENTRIES)
 */
void addEstablishment(Directory *directory) {
    if (directory->numEntries >= MAX_ENTRIES) {
        printf("Directory is full. Cannot add more establishments.\n");
        return;
    }

    EstablishmentDetails *e = &directory->entry[directory->numEntries]; 

    printf("\n=== Add New Establishment ===\n");

    // Text-based fields
    userInputText("Name", e->Name, MAX_LETTERS);
    userInputText("Location", e->Location, MAX_LETTERS);
    userInputText("Contact Number", e->ContactNum, MAX_NUM);
    userInputText("Serving Mode", e->ServingMode, MAX_LETTERS);
    userInputText("Payment Option", e->PaymentOpt, MAX_LETTERS);
    userInputText("Category", e->FoodCateg, MAX_LETTERS);

    // Price range
    promptPriceRange(&e->Price);

    // Popular items
    promptPopularItems(&e->numPopularItems, e->popularItems);

    directory->numEntries++;
    printf("Establishment added successfully!\n");
}

/* 
 * editEstablishment - Allows modification of existing establishment data by selecting fields to update
 * @param directory - pointer to the directory structure containing establishments to edit
 * @return void (no return value)
 * Pre-condition: Directory must contain at least one establishment (numEntries > 0)
 */
void editEstablishment(Directory *directory) {
    if (directory->numEntries == 0) {
        printf("\nNo establishments available to edit.\n");
        return;
    }

    // Display all establishments in table form
    DisplayAdminTable(directory);

    int index;
    printf("Enter the number of the establishment to edit (1 to %d): ", directory->numEntries);
    scanf("%d", &index);
    getchar(); // consume leftover newline

    // Adjust index to array position
    index--;

	//checks if establishment exsist
    if (index < 0 || index >= directory->numEntries) {
        printf("Invalid selection.\n");
        return;
    }

    EstablishmentDetails *e = &directory->entry[index];

    int choice;
    int editing = 1;

    while (editing) {
        printf("\n--- Editing Establishment: %s ---\n", e->Name);
        printf("Choose the field to edit:\n");
        printf("1. Name\n");
        printf("2. Location\n");
        printf("3. Contact Number\n");
        printf("4. Serving Mode\n");
        printf("5. Payment Option\n");
        printf("6. Category\n");
        printf("7. Price Range\n");
        printf("8. Popular Items\n");
        printf("9. Cancel\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // clear newline

        switch (choice) {
            case 1:
                userInputText("New Name", e->Name, MAX_LETTERS);
                break;
            case 2:
                userInputText("New Location", e->Location, MAX_LETTERS);
                break;
            case 3:
                userInputText("New Contact Number", e->ContactNum, MAX_NUM);
                break;
            case 4:
                userInputText("New Serving Mode", e->ServingMode, MAX_LETTERS);
                break;
            case 5:
                userInputText("New Payment Option", e->PaymentOpt, MAX_LETTERS);
                break;
            case 6:
                userInputText("New Category", e->FoodCateg, MAX_LETTERS);
                break;
            case 7:
                promptPriceRange(&e->Price);
                break;
            case 8:
                promptPopularItems(&e->numPopularItems, e->popularItems);
                break;
            case 9:
                editing = 0;
                break;
            default:
                printf("Invalid choice.\n");
        }
    }

    printf("Establishment updated successfully.\n");
}

/* 
 * deleteEstablishment - Removes an establishment from the directory after user confirmation
 * @param directory - pointer to the directory structure from which entry will be deleted
 * @return void (no return value)
 * Pre-condition: Directory must contain at least one establishment (numEntries > 0)
 */
void deleteEstablishment(Directory *directory) {
    if (directory->numEntries == 0) {
        printf("\nNo establishments available to delete.\n");
        return;
    }

    DisplayAdminTable(directory);

    int index;
    printf("Enter the number of the establishment to delete (1 to %d): ", directory->numEntries);
    scanf("%d", &index);
    getchar(); // clear newline

    index--;  // adjust to 0-based index

    if (index < 0 || index >= directory->numEntries) {
        printf("Invalid selection.\n");
        return;
    }

    // Confirm deletion with input validation loop
    int confirm;
    do {
        printf("Are you sure you want to delete '%s'? (1 = Yes / 0 = No): ", directory->entry[index].Name);
        scanf("%d", &confirm);
        getchar(); // clear newline
        
        if (confirm != 0 && confirm != 1) {
            printf("Invalid input! Please enter 1 for Yes or 0 for No.\n");
        }
    } while (confirm != 0 && confirm != 1);

    if (confirm != 1) {
        printf("Deletion canceled.\n");
        return;
    }

    // Shift entries left to overwrite the deleted one  
    for (int i = index; i < directory->numEntries - 1; i++) {
        directory->entry[i] = directory->entry[i + 1];
    }

    directory->numEntries--;
    printf("Establishment deleted successfully.\n");
}

// ----- Filters -----

/* 
 * SingleFilter - Applies a single filter criterion to search establishments and displays matching results
 * @param directory - pointer to the directory structure to search through
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized
 */
void SingleFilter(Directory *directory) {
    char location[MAX_LETTERS] = "", category[MAX_LETTERS] = "";
    char payment[MAX_LETTERS] = "", serving[MAX_LETTERS] = "";
    int userMin = -1, userMax = -1;

    printFilterHints(directory);  // Show available filter options
    promptFilters(location, category, payment, serving, &userMin, &userMax, 1);  // singleMode = 1

    EstablishmentDetails matched[MAX_ENTRIES];
    int matchCount = 0;

    for (int i = 0; i < directory->numEntries; i++) {
        if (filterMatch(&directory->entry[i], location, category, payment, serving, userMin, userMax)) {
            matched[matchCount++] = directory->entry[i];
        }
    }

    if (matchCount > 0)
        DisplayUserTable(matched, matchCount);
    else
        printf("\n  No establishments matched your filter.\n");
}

/* 
 * MultipleFilter - Applies multiple filter criteria simultaneously to search establishments using AND logic
 * @param directory - pointer to the directory structure to search through
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized
 */
void MultipleFilter(Directory *directory) {
    StringName location = "", category = "", payment = "", serving = "";
    int userMin = -1, userMax = -1;

    printFilterHints(directory);  // Show available filter options

    promptFilters(location, category, payment, serving, &userMin, &userMax, 0);

    EstablishmentDetails matched[MAX_ENTRIES];
    int matchCount = 0;

    for (int i = 0; i < directory->numEntries; i++) {
        if (filterMatch(&directory->entry[i], location, category, payment, serving, userMin, userMax)) {
            matched[matchCount++] = directory->entry[i];
        }
    }

    if (matchCount > 0)
        DisplayUserTable(matched, matchCount);
    else
        printf("\n  No establishments matched your filters.\n");
}

// ----- Display -----

/* 
 * DisplayUserTable - Displays establishment data in a formatted table for user viewing with popular items
 * @param matches - array of establishment entries to display
 * @param matchCount - number of entries in the matches array
 * @return void (no return value)
 * Pre-condition: matches array must contain valid establishment data
 */
void DisplayUserTable(EstablishmentDetails matches[], int matchCount) {
    if (matchCount == 0) {
        printf("\nNo establishments matched your filters.\n");
        return;
    }

    printf("\n=================== FILTER RESULTS ===================\n\n");

    printf("+-----+-------------------------+----------------------+----------------------+------------------+------------------+------------------+-------------+\n");
    printf("| No. | Name                    | Location             | Contact Number       | Category         | Payment Option   | Serving Mode     | Price       |\n");
    printf("+-----+-------------------------+----------------------+----------------------+------------------+------------------+------------------+-------------+\n");

    for (int i = 0; i < matchCount; i++) {
        EstablishmentDetails *e = &matches[i];

        printf("| %-3d | %-23s | %-20s | %-20s | %-16s | %-16s | %-16s | %4d - %-5d |\n",
            i + 1,
            e->Name,
            e->Location,
            e->ContactNum,
            e->FoodCateg,
            e->PaymentOpt,
            e->ServingMode,
            e->Price.MinPrice,
            e->Price.MaxPrice
        );

        // Print popular items below the row
        printf("|     Popular Items: ");

        for (int j = 0; j < e->numPopularItems; j++) {
            printf("%s", e->popularItems[j]);
            if (j < e->numPopularItems - 1)
                printf(", ");
        }

        printf("\n");

        // Separator after each entry
        printf("+-----+-------------------------+----------------------+----------------------+------------------+------------------+------------------+-------------+\n");
    }

    printf("\n");
}

/* 
 * DisplayAdminTable - Displays all establishments in the directory in a formatted table for administrative purposes
 * @param directory - pointer to the directory structure containing establishments to display
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized
 */
void DisplayAdminTable(Directory *directory) {
    if (directory->numEntries == 0) {
        printf("\nNo establishments found in the directory.\n");
        return;
    }

    printf("\n=================== ALL ESTABLISHMENTS ===================\n\n");

    printf("+-----+-------------------------+----------------------+----------------------+------------------+------------------+------------------+-------------+\n");
    printf("| No. | Name                    | Location             | Contact Number       | Category         | Payment Option   | Serving Mode     | Price       |\n");
    printf("+-----+-------------------------+----------------------+----------------------+------------------+------------------+------------------+-------------+\n");

    for (int i = 0; i < directory->numEntries; i++) {
        EstablishmentDetails *e = &directory->entry[i];

        printf("| %-3d | %-23s | %-20s | %-20s | %-16s | %-16s | %-16s | %4d - %-5d |\n",
               i + 1,
               e->Name,
               e->Location,
               e->ContactNum,
               e->FoodCateg,
               e->PaymentOpt,
               e->ServingMode,
               e->Price.MinPrice,
               e->Price.MaxPrice
        );

        // Print popular items under the row
        printf("|     Popular Items: ");
        for (int j = 0; j < e->numPopularItems; j++) {
            printf("%s", e->popularItems[j]);
            if (j < e->numPopularItems - 1)
                printf(", ");
        }
        if (e->numPopularItems == 0)
            printf("None");

        printf("\n");

        // Row separator
        printf("+-----+-------------------------+----------------------+----------------------+------------------+------------------+------------------+-------------+\n");
    }

    printf("\n");
}

// ----- File Handling -----

/* 
 * handleSave - Manages the save process by prompting user for filename and handling file selection logic
 * @param directory - pointer to the directory structure to save
 * @param filename - string array containing current filename or empty for new files
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized
 */
void handleSave(Directory *directory, char filename[]) {
    if (strlen(filename) > 0) {
        printf("Current file is: %s\n", filename);
        printf("Do you want to save in the same file? (1 = Yes / 0 = No): ");
        int sameFile;
        scanf("%d", &sameFile);
        getchar();  // Clear leftover newline

        if (sameFile == 0) {
            printf("Enter new filename to save to: ");
            fgets(filename, MAX_LETTERS, stdin);
            stripNewline(filename);
        }
    } else {
        printf("No file currently loaded.\n");
        printf("Enter filename to save to: ");
        fgets(filename, MAX_LETTERS, stdin);
        stripNewline(filename);
    }

    saveToFile(directory, filename);
    printf("Directory saved successfully to '%s'.\n", filename);
}

/* 
 * saveToFile - Writes directory data to a specified file in a structured format
 * @param directory - pointer to the directory structure to save
 * @param filename - string containing the target filename
 * @return void (no return value)
 * Pre-condition: Directory structure must be initialized, filename must be valid
 */
void saveToFile(Directory *directory, char filename[]) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to open file '%s' for writing.\n", filename);
        return;
    }

    fprintf(file, "%d\n", directory->numEntries);  // Save how many entries there are

    for (int i = 0; i < directory->numEntries; i++) {
        EstablishmentDetails *e = &directory->entry[i];

        fprintf(file, "%s\n", e->Name);
        fprintf(file, "%s\n", e->Location);
        fprintf(file, "%s\n", e->ContactNum);
        fprintf(file, "%s\n", e->ServingMode);
        fprintf(file, "%s\n", e->PaymentOpt);
        fprintf(file, "%s\n", e->FoodCateg);
        fprintf(file, "%d %d\n", e->Price.MinPrice, e->Price.MaxPrice);
        fprintf(file, "%d\n", e->numPopularItems);

        for (int j = 0; j < e->numPopularItems; j++) {
            fprintf(file, "%s\n", e->popularItems[j]);
        }
    }

    fclose(file);
}

/* 
 * loadFromFile - Reads directory data from a specified file and populates the directory structure
 * @param directory - pointer to the directory structure to populate
 * @param filename - string containing the source filename
 * @return void (no return value)
 * Pre-condition: filename must point to a valid file with correct format
 */
void loadFromFile(Directory *directory, char filename[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file '%s' for reading. Starting with empty directory.\n", filename);
        directory->numEntries = 0;
        return;
    }

    int numEntries;
    fscanf(file, "%d\n", &numEntries);

    if (numEntries > MAX_ENTRIES) {
        printf("Warning: File contains more entries than allowed (%d). Only loading first %d entries.\n", numEntries, MAX_ENTRIES);
        numEntries = MAX_ENTRIES;
    }

    directory->numEntries = numEntries;

    for (int i = 0; i < numEntries; i++) {
        EstablishmentDetails *e = &directory->entry[i];

        fgets(e->Name, MAX_LETTERS, file); stripNewline(e->Name);
        fgets(e->Location, MAX_LETTERS, file); stripNewline(e->Location);
        fgets(e->ContactNum, MAX_NUM, file); stripNewline(e->ContactNum);
        fgets(e->ServingMode, MAX_LETTERS, file); stripNewline(e->ServingMode);
        fgets(e->PaymentOpt, MAX_LETTERS, file); stripNewline(e->PaymentOpt);
        fgets(e->FoodCateg, MAX_LETTERS, file); stripNewline(e->FoodCateg);

        fscanf(file, "%d %d\n", &e->Price.MinPrice, &e->Price.MaxPrice);

        fscanf(file, "%d\n", &e->numPopularItems);

        if (e->numPopularItems > MAX_ITEMS) e->numPopularItems = MAX_ITEMS;  // safety

        for (int j = 0; j < e->numPopularItems; j++) {
            fgets(e->popularItems[j], MAX_LETTERS, file);
            stripNewline(e->popularItems[j]);
        }
    }

    fclose(file);
}

