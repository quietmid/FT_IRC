#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

int main (int argc, char *argv[]) 
{
	if (argc == 3)
	{
		int port = std::stoi(argv[1]);
		std::string password = argv[2];

		//create a socket
		int listening = socket(AF_INET, SOCK_STREAM, 0);
		if (listening < 0) {
			std::cerr << "Can't create a socket! Quitting" << std::endl;
			return -1;
		}
		//bind the socket to an address
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port); //htons = host to network short
		if (inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr) <= 0) {
			std::cerr << "Invalid address/ Address not supported" << std::endl;
			return -1;
		}
		if (bind(listening, (struct sockaddr*)&hint, sizeof(hint)) == -1) {
			std::cerr << "Can't bind to IP/Port" << std::endl;
			return -2;
		}
		//Mark the socket for listening in
		if (listen(listening, SOMAXCONN) == -1) {
			std::cerr << "Can't listen!" << std::endl;
			return -3;
		}
		//Accept a connection
		sockaddr_in client;
		socklen_t clientSize = sizeof(client);
		char host[NI_MAXHOST];
		char svc[NI_MAXSERV];

		int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
		if (clientSocket == -1) {
			std::cerr << "Problem with client connecting!" << std::endl;
			return -4;
		}
		// close the listening socket
		close(listening);
		memset(host, 0, NI_MAXHOST);
		memset(svc, 0, NI_MAXSERV);
		int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
		if (result) {
			std::cout << host << " connected on " << svc << std::endl;
		}
		else {
			// numeric to string
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
		}
		//while receiving : display message, echo message
		char buf[4096];
		while (true)
		{
			//clear the buffer
			memset(buf, 0, 4096);
			//wait for the message
			int bytesRecv = recv(clientSocket, buf, 4096, 0);
			if (bytesRecv == -1) {
				std::cerr << "There was a connection issue" << std::endl;
				break;
			}
			if (bytesRecv == 0) {
				std::cout << "The client disconnected" << std::endl;
				break;
			}
			//display message
			std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;
			//resend message
			send(clientSocket, buf, bytesRecv + 1, 0);

		}
		//close the connection/socket
		close(clientSocket);
		std::cout << "Closing the connection" << std::endl;
	}
	else
	{
		std::cout << "To start: ./ircserv <port> <password>" << std::endl;
		std::cout << "eg: ./ircserv 8080 yourPASS" << std::endl;
		return 1;
	}
	return 0;
}