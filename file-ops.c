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
   
    int sev=0;
    while(sev<1 || sev>3){
       printf( "Introduceti severitatea(1-3)[1=minor, 2=moderat, 3=sever]:\n");
       scanf("%d", &sev);
       if(sev<1 || sev>3){
        printf("Eroare: severitatea trebuie sa fie intre 1 si 3\n");
       }
       
    }
    new_report.severity=sev;

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
            printf("Inspector: %s\n", r.inspector_name);
            printf("Categorie: %s\n", r.category);
            printf("Severitate: %d\n", r.severity);
            printf("Descriere: %s\n", r.description);
            found = 1;
            break;
        }
    }
    if (!found) printf("Raportul cu ID %d nu a fost găsit.\n", id);
    close(fd);
}
void remove_report(const char *district, int id, Role role) {
    // Doar Managerul are voie să șteargă rapoarte
    if (role != ROLE_MANAGER) {
        printf("Eroare: Doar managerul poate sterge rapoarte!\n");
        return;
    }

    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);

    // Deschidem fișierul pentru citire și scriere (O_RDWR)
    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului");
        return;
    }

    Report r;
    off_t pos;
    int found = 0;

    // Căutăm raportul cu ID-ul cerut
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.report_id == id) {
            found = 1;
            pos = lseek(fd, 0, SEEK_CUR); // Poziția curentă (după elementul găsit)
            
            // Mutăm toate elementele următoare cu o poziție mai sus
            Report next_r;
            while (read(fd, &next_r, sizeof(Report)) == sizeof(Report)) {
                // Mergem înapoi la poziția unde trebuie scris (două structuri în urmă față de unde suntem acum după read)
                lseek(fd, pos - sizeof(Report), SEEK_SET);
                write(fd, &next_r, sizeof(Report));
                
                // Actualizăm pos pentru următoarea scriere și avansăm citirea
                pos = lseek(fd, sizeof(Report), SEEK_CUR);
            }

            // Tăiem fișierul pentru a elimina ultima înregistrare (care e acum duplicată)
            struct stat st;
            fstat(fd, &st);
            if (ftruncate(fd, st.st_size - sizeof(Report)) == -1) {
                perror("Eroare la trunchierea fisierului");
            } else {
                printf("Raportul cu ID %d a fost sters cu succes.\n", id);
            }
            break;
        }
    }

    if (!found) printf("Raportul cu ID %d nu a fost gasit.\n", id);
    close(fd);

   void filter_reports(const char *district, const char *condition) {
    char field[30], op[5], value[50];
    if (!parse_condition(condition, field, op, value)) {
        printf("Format conditie invalid! Folositi camp:op:valoare (ex: severity:>:1)\n");
        return;
    }

    char filepath[256];
    sprintf(filepath, "%s/reports.dat", district);
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) { perror("Eroare"); return; }

    Report r;
    printf("\n--- Rezultate filtrare pentru [%s %s %s] ---\n", field, op, value);
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (match_condition(&r, field, op, value)) {
            printf("ID: %d | Categorie: %s | Severitate: %d\n", 
                   r.report_id, r.category, r.severity);
        }
    }
    close(fd);
}
}