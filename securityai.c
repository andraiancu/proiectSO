//permissions
#include "common.h"
void get_mode_string(mode_t mode, char *str) {
    // Prima poziție e pentru tipul de fișier (aici punem '-' că e fișier normal)
    str[0] = (S_ISDIR(mode)) ? 'd' : '-';
    //drepturi de manageer
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    //drepturi de inspector
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';

    // Drepturi Others
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    
    str[10] = '\0';
}
int check_permission(const char *path, Role role, mode_t required_bit) {
    struct stat st;
    if (stat(path, &st) == -1) return 0; 

    if (role == ROLE_MANAGER) {
        if (st.st_mode & (required_bit << 6)) return 1;
    } else if (role == ROLE_INSPECTOR) {
        if (st.st_mode & (required_bit << 3)) return 1;
    }
    return 0;
}
    // Funcții temporare (Stubs) ca să putem compila
int parse_condition(const char *input, char *field, char *op, char *value) {
    return 0; 
}

int match_condition(Report *r, const char *field, const char *op, const char *value) {
    return 0;
}

