
#include "game_setup.h"
#include <stdio.h>

#define NAME_SIZE 4096

int main(int argc, char* argv[]){
	if(enet_initialize()<0){
		fprintf(stderr, "Error initializing ENet\n");
		return 0;
	}
	atexit(enet_deinitialize);
	
	Server* serv = new Server();
	char name[NAME_SIZE];
	serv->get_address(name, NAME_SIZE);
	printf("%s\n", name);
	
	GameSetup* setup = new GameSetup(serv);
	setup->mainloop();
	
	delete setup;
	delete serv;
	return 0;
}