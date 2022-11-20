#include <bits/stdc++.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>


using namespace std;

#define MAX_LEN 200 //Maximum length of message which can be sent
#define MAX_COLORS 6//Different Colors will be assigned to different users
#define BACKLOG 8 //in listen function
struct Terminal{
	int ID;
	string Name;
	int socket;
	thread th;
};



vector<Terminal> CLIENTS;
string DEFAULT_COL="\033[0m";
string colors[]={"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m"};
int seed=0;
mutex COUT_MTX,CLIENTS_MTX;


//Some Utitily Functions

// Set Name of client
void set_name(int ID, char Name[]){
	for(int i=0; i<CLIENTS.size(); i++){
			if(CLIENTS[i].ID==ID)	{
				CLIENTS[i].Name=string(Name);
			}
	}	
}
// For synchronisation of cout statements
void shared_print(string str, bool endLine=true){	
	lock_guard<mutex> guard(COUT_MTX);
	cout<<str;
	if(endLine){
		cout<<endl;
	}
}
// Broadcast message to all CLIENTS except the sender
int send_message(string message, int sender_id){
	char temp[MAX_LEN];
	//Copying the message to temp
	strcpy(temp,message.c_str());
	for(int i=0; i<CLIENTS.size(); i++){
		if(CLIENTS[i].ID!=sender_id){
			send(CLIENTS[i].socket,temp,sizeof(temp),0);
		}
	}		
	return -1;
}
// Broadcast a number to all CLIENTS except the sender
int send_message(int num, int sender_id){
	for(int i=0; i<CLIENTS.size(); i++){
		if(CLIENTS[i].ID!=sender_id){
			send(CLIENTS[i].socket,&num,sizeof(num),0);
		}
	}
	return -1;		
}
void end_connection(int ID){
	for(int i=0; i<CLIENTS.size(); i++){
		if(CLIENTS[i].ID==ID){
			lock_guard<mutex> guard(CLIENTS_MTX);
			CLIENTS[i].th.detach();
			CLIENTS.erase(CLIENTS.begin()+i);
			close(CLIENTS[i].socket);
			break;
		}
	}				
}
string color(int code){
	//Colors will be assigned int cyclic order of joining 
	return colors[code%MAX_COLORS];
}
//Utility Function for handling individua CLIENTS
void handle_client(int client_socket, int ID){
	char Name[MAX_LEN],str[MAX_LEN];
	recv(client_socket,Name,sizeof(Name),0);
	set_name(ID,Name);	

	// Display welcome message
	string welcome_message=string(Name)+string(" has joined!!");
	send_message("#NULL",ID);	
	send_message(ID,ID);								
	send_message(welcome_message,ID);	
	shared_print(color(ID)+welcome_message+DEFAULT_COL);
	
	while(true){
		int bytes_received=recv(client_socket,str,sizeof(str),0);
		if(bytes_received<=0)
			return;
		if(strcmp(str,"$exit")==0){
			// Display leaving message
			string message=string(Name)+string(" has left :(");		
			send_message("#NULL",ID);			
			send_message(ID,ID);						
			send_message(message,ID);
			shared_print(color(ID)+message+DEFAULT_COL);
			end_connection(ID);							
			return;
		}
		send_message(string(Name),ID);					
		send_message(ID,ID);		
		send_message(string(str),ID);
		shared_print(color(ID)+Name+" : "+DEFAULT_COL+str);		
	}	
}


int main(){
	int server_socket;
	//Creating a SOCKET
	if((server_socket=socket(AF_INET,SOCK_STREAM,0))==-1){
		//Catching Error is scoket reutrns -1 then this means it has failed to create a SOCKET
		perror("socket: ");
		exit(-1);
	}
	//STEP-2
	//Binding our SOCKET(to assign an IP and PORT to the Socket which is necessary for connection)
	struct sockaddr_in server;
	server.sin_family=AF_INET;//Address Family: AF_NET
	server.sin_port=htons(10000);//Port in Network Byte Order
	server.sin_addr.s_addr=INADDR_ANY;//Internet Address
	bzero(&server.sin_zero,0);

	//binding 
	if((bind(server_socket,(struct sockaddr *)&server,sizeof(struct sockaddr_in)))==-1){
		//If bind failed
		perror("bind error: ");
		exit(-1);
	}
	//Listen marks socket as Passive i.e The socket will be used to accept connections
	//BACKLOG is the maximum number of connections that will be queued before connections are refused
	if((listen(server_socket,BACKLOG))==-1){
		//If listen failed
		perror("listen error: ");
		exit(-1);
	}
    

	//Accepting Connections from Client
	struct sockaddr_in client;
	int client_socket;
	unsigned int len=sizeof(sockaddr_in);

	cout<<colors[MAX_COLORS-1]<<"\n\t";
	cout<<"          Welcome to the CHATROOM         "<<endl;
	cout<<DEFAULT_COL;

	while(1){
		if((client_socket=accept(server_socket,(struct sockaddr *)&client,&len))==-1){
			//Failed to accept connection
			perror("accept error: ");
			exit(-1);
		}
		//SYNCHRONIZATION
		seed++;
		thread t(handle_client,client_socket,seed);
		lock_guard<mutex> guard(CLIENTS_MTX);
		CLIENTS.push_back({seed, string("Anonymous"),client_socket,(move(t))});
	}

	for(int i=0; i<CLIENTS.size(); i++){
		//Check if the current client is Joinable (From the client side)
		if(CLIENTS[i].th.joinable()){
			CLIENTS[i].th.join();
		}
	}
	//Closing the server
	close(server_socket);
	return 0;
}
