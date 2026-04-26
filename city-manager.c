//functia main, citire, switch-uri
#include "common.h"

int main(int argc, char *argv[]) {
    // 1. Verificăm dacă user-ul a scris măcar ceva după numele programului
    if (argc < 2) {
        printf("Utilizare: %s --role <role> --user <name> <comanda> <district> [argument_extra]\n", argv[0]);
        return 1;
    }

    // Variabile unde "parcăm" datele găsite în linia de comandă
    Role role = ROLE_NONE;
    char *user = NULL;
    char *command = NULL;
    char *district = NULL;

    // 2. Bucla magică: parcurgem toate cuvintele scrise în terminal (argv)
    for (int i = 1; i < argc; i++) {
        // Dacă dăm peste "--role", următorul cuvânt (i+1) e manager sau inspector
        if (strcmp(argv[i], "--role") == 0 && i + 1 < argc) {
            if (strcmp(argv[i+1], "manager") == 0) role = ROLE_MANAGER;
            else if (strcmp(argv[i+1], "inspector") == 0) role = ROLE_INSPECTOR;
            i++; // Sărim peste cuvântul "manager"/"inspector" ca să nu-l procesăm iar
        } 
        // Dacă dăm peste "--user", următorul cuvânt e numele celui care rulează
        else if (strcmp(argv[i], "--user") == 0 && i + 1 < argc) {
            user = argv[i+1];
            i++; 
        }
        else {
            // Dacă nu e un flag (cu --), înseamnă că e comanda sau districtul
            // Le salvăm în ordinea în care apar
            if (command == NULL) {
                command = argv[i]; // Primul cuvânt "liber" e comanda (ex: add, list)
            } else if (district == NULL) {
                district = argv[i]; // Al doilea e districtul (ex: centru, buziasului)
            }
        }
    }

    // 3. Verificăm dacă am strâns toate "ingredientele" necesare
    if (role == ROLE_NONE || user == NULL || command == NULL || district == NULL) {
        printf("Error: Ai uitat argumente obligatorii! Verifica --role, --user, comanda si district.\n");
        return 1;
    }

    // 4. "Switch-ul" de comenzi: aici decidem ce funcție apelăm
    if (strcmp(command, "add") == 0) {
        // Trimitem datele către file_ops.c pentru a crea un raport nou
        add_report(district, user, role);
    } 
    else if (strcmp(command, "list") == 0) {
        // Citim tot fișierul binar și-l afișăm ca tabel
        list_reports(district, role);
    }
    else if (strcmp(command, "view") == 0) {
        
        if (argc < 6) { printf("Lipsește ID-ul raportului!\n"); return 1; }
      
        view_report(district, atoi(argv[argc-1]), role);
    }
    else if (strcmp(command, "remove_report") == 0) {
        if (argc < 6) { printf("Lipsește ID-ul raportului!\n"); return 1; }
        // Managerul vrea să șteargă ceva definitiv prin lseek și ftruncate
        remove_report(district, atoi(argv[argc-1]), role);
    }
    else if (strcmp(command, "filter") == 0) {
        //parse and match
        if (argc < 6) {
            printf("Utilizare: filter <district> <camp:op:valoare> (ex: severity:>:1)\n");
        } else {
            // Trimitem condiția către motorul de filtrare
            filter_reports(district, argv[argc-1]);
        }
    }
    else {
        // Dacă user-ul scrie altcv
        printf("Comanda necunoscuta: %s\n", command);
    }

    return 0;
}
