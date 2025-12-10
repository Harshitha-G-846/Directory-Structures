#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES      10
#define MAX_SUBDIRS    10
#define NAME_LEN       20

/* Roles */
#define ROLE_STUDENT   1
#define ROLE_TEACHER   2

/* Directory types (for default permissions) */
#define DIR_NORMAL         0   // default: students can read, teacher full
#define DIR_RESTRICTED     1   // only teachers
#define DIR_SUBMISSION     2   // students can write submissions, teacher full

int currentRole = ROLE_STUDENT;   // set at login

struct permissions {
    int allowStudentRead;
    int allowStudentWrite;
    int allowTeacherRead;
    int allowTeacherWrite;
};

struct treeDir {
    char dname[NAME_LEN];
    char fname[MAX_FILES][NAME_LEN];
    int  fcount;
    struct treeDir *subdir[MAX_SUBDIRS];
    int  subcount;
    struct treeDir *parent;
    struct permissions perms;
};

/* Permission helpers */
int canRead(struct treeDir *d, int role) {
    if (role == ROLE_TEACHER) return d->perms.allowTeacherRead;
    else return d->perms.allowStudentRead;
}

int canWrite(struct treeDir *d, int role) {
    if (role == ROLE_TEACHER) return d->perms.allowTeacherWrite;
    else return d->perms.allowStudentWrite;
}

/* Initialize permissions based on directory type */
void initPermissions(struct treeDir *d, int type) {
    if (type == DIR_RESTRICTED) {
        d->perms.allowStudentRead  = 0;
        d->perms.allowStudentWrite = 0;
        d->perms.allowTeacherRead  = 1;
        d->perms.allowTeacherWrite = 1;
    } else if (type == DIR_SUBMISSION) {
        d->perms.allowStudentRead  = 1;   // adjust if you want them to see only their files
        d->perms.allowStudentWrite = 1;
        d->perms.allowTeacherRead  = 1;
        d->perms.allowTeacherWrite = 1;
    } else { // DIR_NORMAL
        d->perms.allowStudentRead  = 1;
        d->perms.allowStudentWrite = 0;
        d->perms.allowTeacherRead  = 1;
        d->perms.allowTeacherWrite = 1;
    }
}

/* Create a new directory node */
struct treeDir* createDir(char name[], struct treeDir* parentDir, int type) {
    struct treeDir* newdir = (struct treeDir*)malloc(sizeof(struct treeDir));
    if (!newdir) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strncpy(newdir->dname, name, NAME_LEN - 1);
    newdir->dname[NAME_LEN - 1] = '\0';
    newdir->fcount   = 0;
    newdir->subcount = 0;
    newdir->parent   = parentDir;
    for (int i = 0; i < MAX_SUBDIRS; i++) newdir->subdir[i] = NULL;
    initPermissions(newdir, type);
    return newdir;
}

/* Display directory tree (skip dirs current role cannot read) */
void display(struct treeDir* d, int level) {
    if (!canRead(d, currentRole)) return;

    for (int i = 0; i < level; i++) printf("  ");
    printf("Directory: %s\n", d->dname);

    for (int i = 0; i < d->fcount; i++) {
        for (int j = 0; j < level + 1; j++) printf("  ");
        printf("File: %s\n", d->fname[i]);
    }

    for (int i = 0; i < d->subcount; i++) {
        display(d->subdir[i], level + 1);
    }
}

/* Find directory by name (no permission check – used internally) */
struct treeDir* findDirectory(struct treeDir* d, char name[]) {
    if (strcmp(d->dname, name) == 0) {
        return d;
    }
    for (int i = 0; i < d->subcount; i++) {
        struct treeDir* result = findDirectory(d->subdir[i], name);
        if (result != NULL) return result;
    }
    return NULL;
}

/* List all directories visible to current role */
void listAllDirectories(struct treeDir* d, int level) {
    if (!canRead(d, currentRole)) return;

    for (int i = 0; i < level; i++) printf("  ");
    printf("- %s\n", d->dname);

    for (int i = 0; i < d->subcount; i++) {
        listAllDirectories(d->subdir[i], level + 1);
    }
}

/* Add file to directory (write permission required) */
void addFileToDirectory(struct treeDir* root) {
    char dirName[NAME_LEN], fileName[NAME_LEN];

    printf("\nAvailable directories (you can read):\n");
    listAllDirectories(root, 0);

    printf("\nEnter directory name where you want to add the file: ");
    scanf("%19s", dirName);

    struct treeDir* targetDir = findDirectory(root, dirName);

    if (targetDir == NULL || !canWrite(targetDir, currentRole)) {
        printf("Access denied or directory '%s' not found!\n", dirName);
        return;
    }

    if (targetDir->fcount >= MAX_FILES) {
        printf("Directory is full! Cannot add more files.\n");
        return;
    }

    printf("Enter file name: ");
    scanf("%19s", fileName);

    strcpy(targetDir->fname[targetDir->fcount], fileName);
    targetDir->fcount++;
    printf("File '%s' added to directory '%s' successfully!\n", fileName, dirName);
}

/* Add subdirectory (write permission required on parent) */
void addSubdirectory(struct treeDir* root) {
    char parentName[NAME_LEN], subdirName[NAME_LEN];
    int typeChoice, type;

    printf("\nAvailable parent directories (you can write in those that allow it):\n");
    listAllDirectories(root, 0);

    printf("\nEnter parent directory name: ");
    scanf("%19s", parentName);

    struct treeDir* parentDir = findDirectory(root, parentName);

    if (parentDir == NULL || !canWrite(parentDir, currentRole)) {
        printf("Access denied or directory '%s' not found!\n", parentName);
        return;
    }

    if (parentDir->subcount >= MAX_SUBDIRS) {
        printf("Directory is full! Cannot add more subdirectories.\n");
        return;
    }

    printf("Enter new subdirectory name: ");
    scanf("%19s", subdirName);

    printf("Select directory type:\n");
    printf("0. Normal (students read, teacher full)\n");
    printf("1. Restricted (only teachers)\n");
    printf("2. Submission (students submit, teacher full)\n");
    printf("Enter choice: ");
    scanf("%d", &typeChoice);

    if (typeChoice == 1) type = DIR_RESTRICTED;
    else if (typeChoice == 2) type = DIR_SUBMISSION;
    else type = DIR_NORMAL;

    parentDir->subdir[parentDir->subcount] = createDir(subdirName, parentDir, type);
    parentDir->subcount++;
    printf("Subdirectory '%s' created under '%s' successfully!\n", subdirName, parentName);
}

/* Delete file (write permission required) */
void deleteFile(struct treeDir* root) {
    char dirName[NAME_LEN], fileName[NAME_LEN];

    printf("\nAvailable directories (you can read):\n");
    listAllDirectories(root, 0);

    printf("\nEnter directory name: ");
    scanf("%19s", dirName);

    struct treeDir* targetDir = findDirectory(root, dirName);

    if (targetDir == NULL || !canWrite(targetDir, currentRole)) {
        printf("Access denied or directory '%s' not found!\n", dirName);
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
    scanf("%19s", fileName);

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

    for (int i = found; i < targetDir->fcount - 1; i++) {
        strcpy(targetDir->fname[i], targetDir->fname[i + 1]);
    }
    targetDir->fcount--;
    printf("File '%s' deleted successfully!\n", fileName);
}

/* Search file (read permission required on directory to see results) */
void searchFile(struct treeDir* d, char fileName[], int *foundAny) {
    if (!canRead(d, currentRole)) {
        // cannot even see this directory
        return;
    }

    for (int i = 0; i < d->fcount; i++) {
        if (strcmp(d->fname[i], fileName) == 0) {
            printf("Found in directory: %s\n", d->dname);
            *foundAny = 1;
        }
    }

    for (int i = 0; i < d->subcount; i++) {
        searchFile(d->subdir[i], fileName, foundAny);
    }
}

/* Count total files (no permission filter – global stats) */
int countTotalFiles(struct treeDir* d) {
    int total = d->fcount;
    for (int i = 0; i < d->subcount; i++) {
        total += countTotalFiles(d->subdir[i]);
    }
    return total;
}

/* Count total directories (no permission filter – global stats) */
int countTotalDirectories(struct treeDir* d) {
    int total = 1;
    for (int i = 0; i < d->subcount; i++) {
        total += countTotalDirectories(d->subdir[i]);
    }
    return total;
}

/* Free all allocated memory */
void freeTree(struct treeDir* d) {
    for (int i = 0; i < d->subcount; i++) {
        freeTree(d->subdir[i]);
    }
    free(d);
}

/* Display statistics */
void displayStats(struct treeDir* root) {
    printf("\n========== Directory Statistics ==========\n");
    printf("Total directories: %d\n", countTotalDirectories(root));
    printf("Total files: %d\n", countTotalFiles(root));
    printf("==========================================\n\n");
}

int main() {
    struct treeDir* root = createDir("root", NULL, DIR_NORMAL);
    int ch;
    char fileName[NAME_LEN];

    printf("Login as: 1) Student  2) Teacher : ");
    scanf("%d", &ch);
    if (ch == 2) currentRole = ROLE_TEACHER;
    else currentRole = ROLE_STUDENT;

    while (1) {
        printf("\n======================================\n");
        printf("     Tree Directory System (RBAC)\n");
        printf("======================================\n");
        printf("Current role: %s\n", currentRole == ROLE_TEACHER ? "Teacher" : "Student");
        printf("1. Create File\n");
        printf("2. Create Subdirectory\n");
        printf("3. Display Directory Tree\n");
        printf("4. Delete File\n");
        printf("5. Search for File\n");
        printf("6. Display Statistics\n");
        printf("7. List All Directories (visible)\n");
        printf("8. Exit\n");
        printf("======================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &ch) != 1) {
            printf("Invalid input!\n");
            while(getchar() != '\n');
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

            case 5: {
                int foundAny = 0;
                printf("Enter file name to search: ");
                scanf("%19s", fileName);
                printf("\nSearch results for '%s':\n", fileName);
                searchFile(root, fileName, &foundAny);
                if (!foundAny) {
                    printf("No visible match found for you.\n");
                }
                break;
            }

            case 6:
                displayStats(root);
                break;

            case 7:
                printf("\n========== All Visible Directories ==========\n");
                listAllDirectories(root, 0);
                printf("=============================================\n");
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
