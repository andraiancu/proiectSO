//functia main, citire, switch-uri

#include "common.h"

int main(int argc, char *argv[]) {
    // 1. Verificăm dacă avem destule argumente
    if (argc < 2) {
        printf("Utilizare: %s --role <role> --user <name> <comanda> <district>\n", argv[0]);
        return 1;
    }

    //aici o sa stochez datele introduse de utilizator
    Role role = ROLE_NONE;
    char *user = NULL;
    char *command = NULL;
    char *district = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--role") == 0 && i + 1 < argc) {
            if (strcmp(argv[i+1], "manager") == 0) role = ROLE_MANAGER;
            else if (strcmp(argv[i+1], "inspector") == 0) role = ROLE_INSPECTOR;
            i++; // Sărim peste valoarea rolului
        } 
        else if (strcmp(argv[i], "--user") == 0 && i + 1 < argc) {
            user = argv[i+1];
            i++; 
        }
        else {
            // Dacă nu e --role sau --user, înseamnă că am ajuns la comandă
            if (command == NULL) {
                command = argv[i];
            } else if (district == NULL) {
                district = argv[i];
            }
        }
    }
    //verif daca am primit tot de la user
    if (role == ROLE_NONE || user == NULL || command == NULL || district == NULL) {
        printf("Error: ai uitat argumente.\n");
        return 1;
    }
    if (strcmp(command, "add") == 0) {
        add_report(district, user, role);
    } 
    else if (strcmp(command, "list") == 0) {
        list_reports(district, role);
    }
    else {
        printf("Nu exista comanda asta !!! %s\n", command);
    }

}