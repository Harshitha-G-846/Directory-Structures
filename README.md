# Campus Intranet Tree Directory System 

This project is a C program that simulates a **tree-structured directory** for a campus intranet, with basic **role-based access control (RBAC)** for teachers and students.  
It models directories, files, and permissions in memory using a tree data structure and a menu-driven console interface.

## Features

- Tree-structured directory with a single `root` directory at the top
- Create subdirectories and files under any directory
- Three directory types with default permissions:
  - **Normal**: Students can view, teachers can view and modify
  - **Restricted**: Only teachers can view and modify
  - **Submission**: Students can submit files; teachers can view and manage them
- Login as:
  - **Student**: Limited read/write access based on directory permissions
  - **Teacher**: Extended access to course and restricted folders
- Core operations:
  - Create file in a chosen directory
  - Create subdirectory with a chosen type (Normal / Restricted / Submission)
  - Display the directory tree (only folders visible to current role)
  - Delete files (if role has write permission)
  - Search for a file across the visible tree
  - Show global statistics: total directories and total files
  - List all directories visible to current role
- Clean shutdown with recursive memory cleanup

## Directory and Permission Model

Each directory node stores:
- Directory name
- Files (names only)
- Subdirectories (child pointers)
- Parent pointer
- Permission flags:
  - Student read / write
  - Teacher read / write

At runtime:
- User selects **Student** or **Teacher** at login
- Every operation checks permissions before reading or writing:
  - Read operations (e.g., display, list, search) only traverse directories the role can read
  - Write operations (e.g., create file, create subdirectory, delete file) are allowed only where the role has write permission

This models a simplified campus intranet where:
- Teachers can manage course content and restricted staff folders
- Students can read course materials and upload to submission folders
- Some folders are hidden completely from students

# Sample Output
```
Login as: 1) Student  2) Teacher : 1

======================================
     Tree Directory System (RBAC)
======================================
Current role: Student
1. Create File
2. Create Subdirectory
3. Display Directory Tree
4. Delete File
5. Search for File
6. Display Statistics
7. List All Directories (visible)
8. Exit
======================================
Enter your choice: 3

========== Directory Structure ==========
Directory: root
  Directory: CSE
    Directory: materials
      File: intro.pdf
=========================================

```
## Author
```
1. Harshitha G- 1RN24CS094
2. Harshika N- 1RN24CS091
3. Harshitha S- 1RN24CS096
4. Haripriya S M- 1RN24CS094

```
