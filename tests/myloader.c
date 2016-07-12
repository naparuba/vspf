#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#define PATH_LENGTH 256

int main(int argc, char * argv[])
{
	char path[PATH_LENGTH], * msg = NULL;
	int (*my_entry1)();
	int (*my_entry2)();
	void * module1;
	void * module2;

	/* chargez le module, et résolvez les symboles maintenant */
	module1 = dlopen("./module1.so", RTLD_NOW);
	if(!module1) {
		msg = dlerror();
		if(msg != NULL) {
			dlclose(module1);
			exit(1);
		}
	}

	module2 = dlopen("./module2.so", RTLD_NOW);
	if(!module2) {
		msg = dlerror();
		if(msg != NULL) {
			dlclose(module2);
			exit(1);
		}
	}
	
	/* recherchez l'adresse du point d'entrée */
	my_entry1 = dlsym(module1, "entry");
	msg = dlerror();
	if(msg != NULL) {
		perror(msg);
		dlclose(module1);
		exit(1);
	}

	my_entry2 = dlsym(module2, "entry");
	msg = dlerror();
	if(msg != NULL) {
		perror(msg);
		dlclose(module2);
		exit(1);
	}

	/* appelez le point d'entrée du module */
	my_entry1();
	my_entry2();

	/* fermez le module */
	if(dlclose(module1)) {
		perror("error");
		exit(1);
	}

	if(dlclose(module2)) {
		perror("error");
		exit(1);
	}

	return 0;
}
