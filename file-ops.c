#include "common.h"

void add_report(const char *district, const char *user, Role role) {
#ifdef _WIN32
    mkdir(district);
#else
    mkdir(district, 0750);
#endif
    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);

    int fd = open(filepath, O_WRONLY | O_CREAT | O_APPEND, 0664);
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului");
        return;
    }

    Report new_report;
    memset(&new_report, 0, sizeof(Report));
    new_report.report_id = (int)time(NULL); 
    strncpy(new_report.inspector_name, user, 49);
    new_report.timestamp = time(NULL);

    printf("Introduceti categoria (ex: road, lighting): ");
    scanf("%s", new_report.category);
    printf("Introduceti severitatea (1-3): ");
    scanf("%d", &new_report.severity);

    if (write(fd, &new_report, sizeof(Report)) == sizeof(Report)) {
        printf("Raport adaugat cu succes in %s\n", filepath);
    } else {
        printf("Eroare la scrierea raportului.\n");
    }

    close(fd); // <--- AI UITAT SĂ ÎNCHIZI FIȘIERUL
} // <--- ACEASTĂ ACOLADĂ LIPSEA! Ea termină funcția add_report.

void list_reports(const char *district, Role role) {
    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);

    struct stat st;
    if (stat(filepath, &st) == -1) {
        perror("Eroare: Nu pot accesa fisierul de rapoarte");
        return;
    }

    char perms[10];
    get_mode_string(st.st_mode, perms); 
    printf("Fisier: %s | Permisiuni: %s | Dimensiune: %ld bytes | Ultimul acces: %s", 
            filepath, perms, st.st_size, ctime(&st.st_mtime));

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la deschiderea pentru citire");
        return;
    }

    Report r;
    printf("\n--- Lista Rapoarte District: %s ---\n", district);
    printf("%-10s | %-15s | %-10s | %-5s\n", "ID", "Inspector", "Categorie", "Sev");
    printf("------------------------------------------------------------\n");

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        printf("%-10d | %-15s | %-10s | %-5d\n", 
               r.report_id, r.inspector_name, r.category, r.severity);
    }

    close(fd);
}