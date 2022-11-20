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
#include <signal.h>


#define MAX_LEN 200
#define MAX_COLORS 6

using namespace std;

bool EXIT_FLAG=false;
//Threads for Multithreading
thread t_send, t_recv;

//Client Socket will be used for accepting connection from the server
int client_socket;
string DEFAULT_COL="\033[0m";
string colors[]={"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};

//Cyclically assign colors to the users
string color(int code){
	return colors[code%MAX_COLORS];
}

//Some Utitily Functions
// Erase text from terminal
int eraseText(int cnt){
	char back_space=8;//ASCII
	for(int i=0; i<cnt; i++){
		cout<<back_space;
	}	
	return -1;
}
// Send message to everyone
void send_message(int client_socket){
	while(true){
		cout<<colors[1]<<"You : "<<DEFAULT_COL;
		char str[MAX_LEN];
		cin.getline(str,MAX_LEN);
		send(client_socket,str,sizeof(str),0);
		//If the message is "$exit" then that user leaves the chatroom
		if(strcmp(str,"$exit")==0){
			EXIT_FLAG=true;
			t_recv.detach();	
			//Closing the connection
			close(client_socket);
			return;
		}	
	}		
}
// Receive message
void recv_message(int client_socket){
	while(true){
		if(EXIT_FLAG){
			return;
		}
		char NAME[MAX_LEN], str[MAX_LEN];
		int color_code;
		int bytes_received=recv(client_socket,NAME,sizeof(NAME),0);
		if(bytes_received>0){
			recv(client_socket,&color_code,sizeof(color_code),0);
			recv(client_socket,str,sizeof(str),0);
			eraseText(6);
			if(strcmp(NAME,"#NULL")!=0)
				cout<<color(color_code)<<NAME<<" : "<<DEFAULT_COL<<str<<endl;
			else
				cout<<color(color_code)<<str<<endl;
			cout<<colors[1]<<"You : "<<DEFAULT_COL;
			fflush(stdout);
		}
	}	
}
// Handler for "Ctrl + C"
void CATCH_CTRL_C(int signal) {
	char str[MAX_LEN]="$exit";
	send(client_socket,str,sizeof(str),0);
	EXIT_FLAG=true;
	t_send.detach();
	t_recv.detach();
	close(client_socket);
	exit(signal);
}

int main(){
	//If failed to make a connection
	if((client_socket=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket: ");
		exit(-1);
	}
	//Same as in SERVER
	struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_port=htons(10000); // Port no. of server
	client.sin_addr.s_addr=INADDR_ANY;
	//client.sin_addr.s_addr=inet_addr("127.0.0.1"); // Provide IP address of server
	bzero(&client.sin_zero,0);

	if((connect(client_socket,(struct sockaddr *)&client,sizeof(struct sockaddr_in)))==-1){
		perror("Connect: ");
		exit(-1);
	}
	signal(SIGINT, CATCH_CTRL_C);
	char NAME[MAX_LEN];
	cout<<"What's Your Name? : ";
	cin.getline(NAME,MAX_LEN);
	send(client_socket,NAME,sizeof(NAME),0);
	cout<<colors[MAX_COLORS-1]<<"\n\t        Welcome to the CHATROOM        "<<endl<<DEFAULT_COL;
	thread t1(send_message, client_socket);
	thread t2(recv_message, client_socket);
	t_send=move(t1);
	t_recv=move(t2);
	if(t_send.joinable()){
		t_send.join();
	}
	if(t_recv.joinable()){
		t_recv.join();
	}			
	return 0;
}



