/*
 * ============================================================
 *  STUDENT MANAGEMENT SYSTEM
 *  Features: Add, Delete, Update, Search, Display
 *  Uses:     Structures + File Handling (permanent storage)
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ─── Constants ─────────────────────────────────────────── */
#define FILE_NAME   "students.dat"
#define MAX_NAME    50
#define MAX_BRANCH  30
#define DIVIDER     "────────────────────────────────────────────────────────\n"

/* ─── Structure ──────────────────────────────────────────── */
typedef struct {
    int    roll;
    char   name[MAX_NAME];
    char   branch[MAX_BRANCH];
    int    year;
    float  cgpa;
} Student;

/* ─── Utility: Clear input buffer ───────────────────────── */
void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ─── Utility: Print a single student row ───────────────── */
void printHeader(void) {
    printf("\n%-6s  %-20s  %-15s  %-4s  %-5s\n",
           "Roll", "Name", "Branch", "Year", "CGPA");
    printf(DIVIDER);
}

/* ─── Utility: Print student details ────────────────────── */
void printStudent(const Student *s) {
    printf("%-6d  %-20s  %-15s  %-4d  %.2f\n",
           s->roll, s->name, s->branch, s->year, s->cgpa);
}

/* ─── Utility: Check if roll already exists ─────────────── */
int rollExists(int roll) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return 0;

    Student s;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.roll == roll) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* ─── 1. ADD STUDENT ─────────────────────────────────────── */
void addStudent(void) {
    Student s;

    printf("\n╔══════════════════════════════╗\n");
    printf("║      ADD NEW STUDENT         ║\n");
    printf("╚══════════════════════════════╝\n");

    printf("  Enter Roll Number  : ");
    scanf("%d", &s.roll);
    clearBuffer();

    if (rollExists(s.roll)) {
        printf("  [!] Roll %d already exists. Aborting.\n", s.roll);
        return;
    }

    printf("  Enter Name         : ");
    fgets(s.name, MAX_NAME, stdin);
    s.name[strcspn(s.name, "\n")] = '\0';

    printf("  Enter Branch       : ");
    fgets(s.branch, MAX_BRANCH, stdin);
    s.branch[strcspn(s.branch, "\n")] = '\0';

    printf("  Enter Year (1-4)   : ");
    scanf("%d", &s.year);

    printf("  Enter CGPA (0-10)  : ");
    scanf("%f", &s.cgpa);
    clearBuffer();

    /* Validate inputs */
    if (s.year < 1 || s.year > 4) { printf("  [!] Invalid year.\n"); return; }
    if (s.cgpa < 0.0f || s.cgpa > 10.0f) { printf("  [!] Invalid CGPA.\n"); return; }

    FILE *fp = fopen(FILE_NAME, "ab");   /* append-binary */
    if (!fp) { perror("  [!] Cannot open file"); return; }

    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);

    printf("\n  [✓] Student (Roll: %d) added successfully!\n", s.roll);
}

/* ─── 2. DISPLAY ALL STUDENTS ────────────────────────────── */
void displayAll(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        printf("\n  [!] No records found (file not yet created).\n");
        return;
    }

    printf("\n╔══════════════════════════════╗\n");
    printf("║     ALL STUDENT RECORDS      ║\n");
    printf("╚══════════════════════════════╝\n");

    printHeader();

    Student s;
    int count = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        printStudent(&s);
        count++;
    }
    fclose(fp);

    if (count == 0)
        printf("  (No records found)\n");
    else
        printf(DIVIDER);
    printf("  Total records: %d\n", count);
}

/* ─── 3. SEARCH STUDENT ──────────────────────────────────── */
void searchStudent(void) {
    int roll;
    printf("\n╔══════════════════════════════╗\n");
    printf("║       SEARCH STUDENT         ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("  Enter Roll Number to search : ");
    scanf("%d", &roll);
    clearBuffer();

    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) { printf("  [!] No records found.\n"); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.roll == roll) {
            printf("\n  ── Record Found ──\n");
            printf("  Roll   : %d\n",   s.roll);
            printf("  Name   : %s\n",   s.name);
            printf("  Branch : %s\n",   s.branch);
            printf("  Year   : %d\n",   s.year);
            printf("  CGPA   : %.2f\n", s.cgpa);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found)
        printf("  [!] No student with Roll %d found.\n", roll);
}

/* ─── 4. DELETE STUDENT ──────────────────────────────────── */
void deleteStudent(void) {
    int roll;
    printf("\n╔══════════════════════════════╗\n");
    printf("║       DELETE STUDENT         ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("  Enter Roll Number to delete : ");
    scanf("%d", &roll);
    clearBuffer();

    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) { printf("  [!] No records found.\n"); return; }

    FILE *tmp = fopen("temp.dat", "wb");
    if (!tmp) { fclose(fp); perror("  [!] Temp file error"); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.roll == roll) {
            found = 1;          /* skip writing this record */
        } else {
            fwrite(&s, sizeof(Student), 1, tmp);
        }
    }
    fclose(fp);
    fclose(tmp);

    if (found) {
        remove(FILE_NAME);
        rename("temp.dat", FILE_NAME);
        printf("\n  [✓] Student (Roll: %d) deleted successfully!\n", roll);
    } else {
        remove("temp.dat");
        printf("  [!] No student with Roll %d found.\n", roll);
    }
}

/* ─── 5. UPDATE STUDENT ──────────────────────────────────── */
void updateStudent(void) {
    int roll;
    printf("\n╔══════════════════════════════╗\n");
    printf("║       UPDATE STUDENT         ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("  Enter Roll Number to update : ");
    scanf("%d", &roll);
    clearBuffer();

    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) { printf("  [!] No records found.\n"); return; }

    FILE *tmp = fopen("temp.dat", "wb");
    if (!tmp) { fclose(fp); perror("  [!] Temp file error"); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp)) {
        if (s.roll == roll) {
            found = 1;
            printf("\n  Current Record:\n");
            printHeader();
            printStudent(&s);
            printf(DIVIDER);
            printf("\n  Enter NEW details:\n");

            printf("  Name         : ");
            fgets(s.name, MAX_NAME, stdin);
            s.name[strcspn(s.name, "\n")] = '\0';

            printf("  Branch       : ");
            fgets(s.branch, MAX_BRANCH, stdin);
            s.branch[strcspn(s.branch, "\n")] = '\0';

            printf("  Year (1-4)   : ");
            scanf("%d", &s.year);

            printf("  CGPA (0-10)  : ");
            scanf("%f", &s.cgpa);
            clearBuffer();
        }
        fwrite(&s, sizeof(Student), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);

    if (found) {
        remove(FILE_NAME);
        rename("temp.dat", FILE_NAME);
        printf("\n  [✓] Student (Roll: %d) updated successfully!\n", roll);
    } else {
        remove("temp.dat");
        printf("  [!] No student with Roll %d found.\n", roll);
    }
}

/* ─── MENU ───────────────────────────────────────────────── */
void showMenu(void) {
    printf("\n");
    printf("  ╔══════════════════════════════════╗\n");
    printf("  ║    STUDENT MANAGEMENT SYSTEM     ║\n");
    printf("  ╠══════════════════════════════════╣\n");
    printf("  ║  1. Add Student                  ║\n");
    printf("  ║  2. Display All Students         ║\n");
    printf("  ║  3. Search Student               ║\n");
    printf("  ║  4. Delete Student               ║\n");
    printf("  ║  5. Update Student               ║\n");
    printf("  ║  0. Exit                         ║\n");
    printf("  ╚══════════════════════════════════╝\n");
    printf("  Enter your choice : ");
}

/* ─── MAIN ───────────────────────────────────────────────── */
int main(void) {
    int choice;

    printf("\n  Welcome to Student Management System\n");
    printf("  Data stored in: %s\n", FILE_NAME);

    do {
        showMenu();
        if (scanf("%d", &choice) != 1) {
            clearBuffer();
            printf("  [!] Invalid input. Try again.\n");
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1: addStudent();     break;
            case 2: displayAll();     break;
            case 3: searchStudent();  break;
            case 4: deleteStudent();  break;
            case 5: updateStudent();  break;
            case 0: printf("\n  Goodbye! Data saved to '%s'.\n\n", FILE_NAME); break;
            default: printf("  [!] Invalid choice. Enter 0–5.\n");
        }

    } while (choice != 0);

    return 0;
}
