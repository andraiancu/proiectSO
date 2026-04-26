#include "common.h"

//1 add raport
void add_report(const char *district, const char *user, Role role) {
    // Creăm folderul districtului dacă nu există
#ifdef _WIN32
    mkdir(district);
#else
    mkdir(district, 0750); // rwxr-x--- 
#endif

    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);

    // O_APPEND e critic: asigură că scriem mereu la finalul fișierului, fără să ștergem ce era înainte
    int fd = open(filepath, O_WRONLY | O_CREAT | O_APPEND, 0664);
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului");
        return;
    }

    Report new_report;
    memset(&new_report, 0, sizeof(Report)); // Curățăm gunoiul din memorie
    
    new_report.report_id = (int)time(NULL); // ID unic bazat pe secunde
    strncpy(new_report.inspector_name, user, 49);
    new_report.timestamp = time(NULL);

    printf("Introduceti categoria (ex: drumuri, iluminat): ");
    scanf("%s", new_report.category);
   
    // Bucla de validare: nu lăsăm user-ul să bage prostii
    int sev = 0;
    while(sev < 1 || sev > 3) {
       printf("Introduceti severitatea (1-3) [1=minor, 2=moderat, 3=sever]: ");
       if (scanf("%d", &sev) != 1) {
           while (getchar() != '\n'); // Curățăm buffer-ul în caz de litere
           continue;
       }
       if(sev < 1 || sev > 3) printf("Eroare: severitatea trebuie sa fie intre 1 si 3\n");
    }
    new_report.severity = sev;

    // Citirea descrierii fgets ne permite să avem spații în text
    printf("Introduceti descrierea defectiunii: ");
    getchar(); // Consumăm \n rămas de la scanf
    fgets(new_report.description, sizeof(new_report.description), stdin);
    new_report.description[strcspn(new_report.description, "\n")] = 0; // Scoatem enter-ul de la final

    // Scriem toată structura 
    if (write(fd, &new_report, sizeof(Report)) == sizeof(Report)) {
        printf("Raport adaugat cu succes in %s\n", filepath);
    } else {
        printf("Eroare la scrierea raportului.\n");
    }

    close(fd); 
} 

// 2 listare
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
    printf("Fisier: %s | Permisiuni: %s | Dimensiune: %ld bytes\n", filepath, perms, st.st_size);

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) return;

    Report r;
    printf("\n Lista Rapoarte District: %s \n", district);
    printf("%-10s | %-15s | %-10s | %-5s\n", "ID", "Inspector", "Categorie", "Sev");
    

    // Citim până când read returnează 0 (Final de fișier)
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        printf("%-10d | %-15s | %-10s | %-5d\n", r.report_id, r.inspector_name, r.category, r.severity);
    }
    close(fd);
}
//3 vizualizare
void view_report(const char *district, int id, Role role) {
    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);
    
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) { perror("Eroare"); return; }

    Report r;
    int found = 0;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.report_id == id) {
            printf("\n--- Detalii Raport [%d] ---\n", r.report_id);
            printf("Inspector: %s | Categorie: %s | Severitate: %d\n", r.inspector_name, r.category, r.severity);
            printf("Descriere: %s\n", r.description);
            found = 1;
            break;
        }
    }
    if (!found) printf("Raportul cu ID %d nu a fost găsit.\n", id);
    close(fd);
}

//stergere cu shift and truncate
void remove_report(const char *district, int id, Role role) {
    if (role != ROLE_MANAGER) {
        printf("Eroare: Doar managerul poate sterge rapoarte!\n");
        return;
    }

    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);

    int fd = open(filepath, O_RDWR);
    if (fd == -1) { perror("Eroare"); return; }

    Report r;
    off_t pos;
    int found = 0;

    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.report_id == id) {
            found = 1;
            pos = lseek(fd, 0, SEEK_CUR); // Salvăm unde suntem acum
            
            Report next_r;
            // shiftam restul rap peste cel sters
            while (read(fd, &next_r, sizeof(Report)) == sizeof(Report)) {
                lseek(fd, pos - sizeof(Report), SEEK_SET);
                write(fd, &next_r, sizeof(Report));
                pos = lseek(fd, sizeof(Report), SEEK_CUR);
            }

            // Tăiem fizic fișierul de pe disk
            struct stat st;
            fstat(fd, &st);
            ftruncate(fd, st.st_size - sizeof(Report));
            printf("Raportul %d a fost eliminat.\n", id);
            break;
        }
    }
    if (!found) printf("Raport negasit.\n");
    close(fd);
}

//filtarrea 
void filter_reports(const char *district, const char *condition) {
    char field[30], op[5], value[50];
    if (!parse_condition(condition, field, op, value)) {
        printf("Format invalid! Exemplu: severity:>:1\n");
        return;
    }

    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) return;

    Report r;
    printf("\n--- Rezultate filtrare pentru [%s %s %s] ---\n", field, op, value);
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        // Aici apelăm logica generată de AI din security_ai.c
        if (match_condition(&r, field, op, value)) {
            printf("ID: %d | Categorie: %s | Severitate: %d\n", r.report_id, r.category, r.severity);
        }
    }
    close(fd);
}
