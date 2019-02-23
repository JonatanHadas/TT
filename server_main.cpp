#include "network.h"

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
	
	while(true){
		if(serv->is_error()){
			fprintf(stderr, "Error: %s", serv->get_error());
			break;
		}
		
		NetEvent e=serv->get_event();
		switch (e.type)
		{
		case NetEvent::TYPE_CONN:
			printf(	"Connection:    id=%10d\n", e.peer_id);
			break;
		case NetEvent::TYPE_RECV:
			printf(	"Msg received:  id=%10d  len=%5d\n"
					"        data: ", e.peer_id, e.len);
			for(int i = 0; i<e.len; i++){
				printf("%2h",e.data[i]);
			}
			printf("\n");
			delete e.data;
			break;
		case NetEvent::TYPE_DISC:
			printf(	"Disconnection: id=%10d\n", e.peer_id);
			break;
		}
	}
	
	delete serv;
	return 0;
}