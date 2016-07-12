#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#define PATH_LENGTH 256

int main(int argc, char * argv[])
{
	char path[PATH_LENGTH], * msg = NULL;
	int (*my_entry)();
	void * module;

	/* contruisez le nom de chemin du module */
	getcwd(path, PATH_LENGTH);
	strcat(path, "/");
	strcat(path, argv[1]);

	/* chargez le module, et résolvez les symboles maintenant */
	module = dlopen(path, RTLD_NOW);
	if(!module) {
		msg = dlerror();
		if(msg != NULL) {
			dlclose(module);
			exit(1);
		}
	}

	/* recherchez l'adresse du point d'entrée */
	my_entry = dlsym(module, "entry");
	msg = dlerror();
	if(msg != NULL) {
		perror(msg);
		dlclose(module);
		exit(1);
	}

	/* appelez le point d'entrée du module */
	my_entry();

	/* fermez le module */
	if(dlclose(module)) {
		perror("error");
		exit(1);
	}

	return 0;
}
