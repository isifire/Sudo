#include <iostream>
#include <fstream>
#include <cstring>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

bool compruebaSudoers(const string& username) {
    ifstream fichero("sudoersfinal.txt");
    if (!fichero.is_open()) {
        cerr << "Error: No se pudo abrir el archivo sudoersfinal.txt\n";
        exit(EXIT_FAILURE);
    }

    string line;
    while (getline(fichero, line)) {
        if (line == username) {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " [-u usuario] comando [argumentos...]\n";
        exit(EXIT_FAILURE);
    }

    string username;
    if (strcmp(argv[1], "-u") == 0) {
        if (argc < 4) {
            cerr << "Error: Se esperaba el nombre de usuario después de '-u'\n";
            exit(EXIT_FAILURE);
        }
        username = argv[2];
        argv += 2; // Avanzar los argumentos para ejecutar el comando correcto
    } else {
        // Obtener el nombre de usuario actual
        struct passwd *pw = getpwuid(getuid());
        if (pw == nullptr) {
            cerr << "Error: No se pudo obtener el nombre de usuario\n";
            exit(EXIT_FAILURE);
        }
        username = pw->pw_name;
    }

    // Verificar si el usuario tiene permisos de sudo
    if (!compruebaSudoers(username)) {
        cerr << "Error: El usuario " << username << " no tiene permisos para usar sudo\n";
        exit(EXIT_FAILURE);
    }

    // Cambiar el ID de usuario efectivo a 0 solo si se usa la opción -u
    if (strcmp(argv[1], "-u") == 0) {
        struct passwd *pw = getpwnam(username.c_str());
        if (pw == nullptr) {
            cerr << "Error: No se encontró el usuario " << username << endl;
            exit(EXIT_FAILURE);
        }
        if (seteuid(pw->pw_uid) == -1) {
            perror("Error al cambiar el ID de usuario efectivo");
            exit(EXIT_FAILURE);
        }
    }

    // Ejecutar el comando
    if (execvp(argv[1], argv + 1) == -1) {
        perror("Error al ejecutar el comando");
        exit(EXIT_FAILURE);
    }

    return 0;
}
