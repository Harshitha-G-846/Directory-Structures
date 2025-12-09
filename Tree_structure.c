#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct treeDir {
    char dname[20];
    char fname[10][20];
    int fcount;
    struct treeDir *subdir[10];
    int subcount;
    struct treeDir *parent; // Added to track parent directory
};

// Function to create a new directory node
struct treeDir* createDir(char name[], struct treeDir* parentDir) {
    struct treeDir* newdir = (struct treeDir*)malloc(sizeof(struct treeDir));
    strcpy(newdir->dname, name);
    newdir->fcount = 0;
    newdir->subcount = 0;
    newdir->parent = parentDir;
    return newdir;
}

// Recursive function to display the directory structure
void display(struct treeDir* d, int level) {
    // Print indentation based on level
    for (int i = 0; i < level; i++) printf("  ");
    printf("Directory: %s\n", d->dname);

    // Display files in the current directory
    for (int i = 0; i < d->fcount; i++) {
        for (int j = 0; j < level + 1; j++) printf("  ");
        printf("File: %s\n", d->fname[i]);
    }

    // Recursively call display for all subdirectories
    for (int i = 0; i < d->subcount; i++) {
        display(d->subdir[i], level + 1);
    }
}

// Function to find a directory by name (searches recursively)
struct treeDir* findDirectory(struct treeDir* d, char name[]) {
    if (strcmp(d->dname, name) == 0) {
        return d;
    }
    
    for (int i = 0; i < d->subcount; i++) {
        struct treeDir* result = findDirectory(d->subdir[i], name);
        if (result != NULL) {
            return result;
        }
    }
    
    return NULL;
}

// Function to list all directories (for easier navigation)
void listAllDirectories(struct treeDir* d, int level) {
    for (int i = 0; i < level; i++) printf("  ");
    printf("- %s\n", d->dname);
    
    for (int i = 0; i < d->subcount; i++) {
        listAllDirectories(d->subdir[i], level + 1);
    }
}

// Function to add file to a specific directory
void addFileToDirectory(struct treeDir* root) {
    char dirName[20], fileName[20];
    
    printf("\nAvailable directories:\n");
    listAllDirectories(root, 0);
    
    printf("\nEnter directory name where you want to add the file: ");
    scanf("%s", dirName);
    
    struct treeDir* targetDir = findDirectory(root, dirName);
    
    if (targetDir == NULL) {
        printf("Directory '%s' not found!\n", dirName);
        return;
    }
    
    if (targetDir->fcount >= 10) {
        printf("Directory is full! Cannot add more files.\n");
        return;
    }
    
    printf("Enter file name: ");
    scanf("%s", fileName);
    strcpy(targetDir->fname[targetDir->fcount], fileName);
    targetDir->fcount++;
    printf("File '%s' added to directory '%s' successfully!\n", fileName, dirName);
}

// Function to add subdirectory to a specific directory
void addSubdirectory(struct treeDir* root) {
    char parentName[20], subdirName[20];
    
    printf("\nAvailable directories:\n");
    listAllDirectories(root, 0);
    
    printf("\nEnter parent directory name: ");
    scanf("%s", parentName);
    
    struct treeDir* parentDir = findDirectory(root, parentName);
    
    if (parentDir == NULL) {
        printf("Directory '%s' not found!\n", parentName);
        return;
    }
    
    if (parentDir->subcount >= 10) {
        printf("Directory is full! Cannot add more subdirectories.\n");
        return;
    }
    
    printf("Enter new subdirectory name: ");
    scanf("%s", subdirName);
    
    parentDir->subdir[parentDir->subcount] = createDir(subdirName, parentDir);
    parentDir->subcount++;
    printf("Subdirectory '%s' created under '%s' successfully!\n", subdirName, parentName);
}

// Function to delete a file from a directory
void deleteFile(struct treeDir* root) {
    char dirName[20], fileName[20];
    
    printf("\nAvailable directories:\n");
    listAllDirectories(root, 0);
    
    printf("\nEnter directory name: ");
    scanf("%s", dirName);
    
    struct treeDir* targetDir = findDirectory(root, dirName);
    
    if (targetDir == NULL) {
        printf("Directory '%s' not found!\n", dirName);
        return;
    }
    
    if (targetDir->fcount == 0) {
        printf("No files in directory '%s'!\n", dirName);
        return;
    }
    
    printf("\nFiles in '%s':\n", dirName);
    for (int i = 0; i < targetDir->fcount; i++) {
        printf("%d. %s\n", i + 1, targetDir->fname[i]);
    }
    
    printf("Enter file name to delete: ");
    scanf("%s", fileName);
    
    int found = -1;
    for (int i = 0; i < targetDir->fcount; i++) {
        if (strcmp(targetDir->fname[i], fileName) == 0) {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        printf("File '%s' not found in directory '%s'!\n", fileName, dirName);
        return;
    }
    
    // Shift files to fill the gap
    for (int i = found; i < targetDir->fcount - 1; i++) {
        strcpy(targetDir->fname[i], targetDir->fname[i + 1]);
    }
    targetDir->fcount--;
    printf("File '%s' deleted successfully!\n", fileName);
}

// Function to search for a file in the entire tree
void searchFile(struct treeDir* d, char fileName[], int level) {
    for (int i = 0; i < d->fcount; i++) {
        if (strcmp(d->fname[i], fileName) == 0) {
            printf("Found in directory: %s\n", d->dname);
        }
    }
    
    for (int i = 0; i < d->subcount; i++) {
        searchFile(d->subdir[i], fileName, level + 1);
    }
}

// Function to count total files in the tree
int countTotalFiles(struct treeDir* d) {
    int total = d->fcount;
    
    for (int i = 0; i < d->subcount; i++) {
        total += countTotalFiles(d->subdir[i]);
    }
    
    return total;
}

// Function to count total directories in the tree
int countTotalDirectories(struct treeDir* d) {
    int total = 1; // Count current directory
    
    for (int i = 0; i < d->subcount; i++) {
        total += countTotalDirectories(d->subdir[i]);
    }
    
    return total;
}

// Function to free all allocated memory (cleanup)
void freeTree(struct treeDir* d) {
    for (int i = 0; i < d->subcount; i++) {
        freeTree(d->subdir[i]);
    }
    free(d);
}

// Function to display statistics
void displayStats(struct treeDir* root) {
    printf("\n========== Directory Statistics ==========\n");
    printf("Total directories: %d\n", countTotalDirectories(root));
    printf("Total files: %d\n", countTotalFiles(root));
    printf("==========================================\n\n");
}

int main() {
    // Initialize the root directory
    struct treeDir* root = createDir("root", NULL);
    int ch;
    char fileName[20];

    while (1) {
        printf("\n======================================\n");
        printf("     Tree Directory System\n");
        printf("======================================\n");
        printf("1. Create File\n");
        printf("2. Create Subdirectory\n");
        printf("3. Display Directory Tree\n");
        printf("4. Delete File\n");
        printf("5. Search for File\n");
        printf("6. Display Statistics\n");
        printf("7. List All Directories\n");
        printf("8. Exit\n");
        printf("======================================\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &ch) != 1) {
            printf("Invalid input!\n");
            while(getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch(ch) {
            case 1:
                addFileToDirectory(root);
                break;
                
            case 2:
                addSubdirectory(root);
                break;
                
            case 3:
                printf("\n========== Directory Structure ==========\n");
                display(root, 0);
                printf("=========================================\n");
                break;
                
            case 4:
                deleteFile(root);
                break;
                
            case 5:
                printf("Enter file name to search: ");
                scanf("%s", fileName);
                printf("\nSearch results for '%s':\n", fileName);
                searchFile(root, fileName, 0);
                break;
                
            case 6:
                displayStats(root);
                break;
                
            case 7:
                printf("\n========== All Directories ==========\n");
                listAllDirectories(root, 0);
                printf("=====================================\n");
                break;
                
            case 8:
                printf("Cleaning up memory...\n");
                freeTree(root);
                printf("Exiting program.\n");
                return 0;
                
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    
    return 0;
}