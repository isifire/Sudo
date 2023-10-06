#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>
#include <cstring>
#include <string>
#include <pwd.h>

using namespace std;

bool compruebaSudoers(){

	//
	uid_t user = getuid();

	ifstream fichero("sudoersfinal.txt");

		if(!fichero.is_open()){
			cout << "El fichero no existe\n";
			exit(1);
		}

	int uid;
	string username;

	while(fichero >> username){
		uid = getpwnam(username.c_str())->pw_uid;
		if(uid == user){
			return true;
		}
	}
	return false;
}

int main (int argc, char * argv[]){

	if(argc<2){
	   cout << "No has introducido ningun programa a ejecutar por el sudo\n";
	   exit(1);
	}

	if(strcmp(argv[1],"-u")== 0){
		if(geteuid() == 0){
			int uid_login = getpwnam(argv[2])->pw_uid;
			seteuid(uid_login);
			execvp(argv[3],argv+3);
			cout << "ERROR: el comando no es correcto \n";
			exit(1);
		}

		else{
		cout << "ERROR: el usuario no tiene uid 0\n";
		exit(1);
		}

	}

	uid_t user;
	user = getuid();

	if(compruebaSudoers()){
	        seteuid(0);
		   execvp(argv[1],argv+1);
			cout << "Error el comando no existe \n";
			exit(1);
	}

	else{
	   cout << "El usuario con uid "<< user << " no esta en el fichero sudoers\n";
	   exit(1);
	}

return 0;

}
