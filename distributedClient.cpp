// Program: TCPClient
// Subject: CMPE 207
// Project - Distributed Learning Management System
// Created by Team 8.


/*****CLIENT PROGRAM*****/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <time.h>
using namespace std;


void error(const char *msg)
{
	perror(msg);
	printf ("%s", strerror(errno));
	exit(1);
}


int userInfo(char* storageBuffer){

	char tempBuf[10];
	char* ID, uniqueID;
	string name;
	int UserType, newOrReturningUser, bufferWriteSuccess, newUnique;
	char *ptr; // for strtol. not used anywhere
	memset(storageBuffer, '\0',BUFSIZ);

	printf("\nHello!!\n"
			"Welcome to the new Distributed Learning Management System\n"
			"To get you started we need a few information about you.\n\n");
	printf("What kind of a user are you? Please choose an option by using numbers\n"
			"1. System Administrator\n"
			"2. Instructor\n"
			"3. Student\n");
	fgets(tempBuf, sizeof tempBuf, stdin);
	tempBuf[strlen(tempBuf) - 1] = '\0';
	UserType = strtol(tempBuf, &ptr, 10);
	if(UserType == 1){
		printf("You are the System Administrator\n\n");
	}
	else if(UserType == 2){
		printf("You are an Instructor\n\n");
	}
	else if(UserType == 3){
		printf("You are a Student\n\n");
	}
	if(UserType > 3){
		printf("Not a valid option\n"
				"Please try again!\n");
		return -1;
	}

	printf("Are you a new user or returning user? Please choose an option by using numbers\n"
			"1. New user\n"
			"2. Returning user\n");
	fgets(tempBuf, sizeof tempBuf, stdin);
	tempBuf[strlen(tempBuf) - 1] = '\0';
	newOrReturningUser = strtol(tempBuf, &ptr, 10);

	if(newOrReturningUser == 1){
		printf("\nPlease enter your name\n");
		fgets(tempBuf, sizeof tempBuf, stdin);
		tempBuf[strlen(tempBuf) - 1] = '\0';
		name = tempBuf;

		printf("\nEnter your ID\n");
		fgets(tempBuf, sizeof tempBuf, stdin);
		tempBuf[strlen(tempBuf) - 1] = 0;
		ID = tempBuf;
		
		bufferWriteSuccess = snprintf(storageBuffer, BUFSIZ, "%d %d %s %s", UserType, newOrReturningUser,name.c_str(), ID);
	}

	else if(newOrReturningUser == 2){
		printf("\nPlease enter the unique ID\n");
		fgets(tempBuf, sizeof tempBuf, stdin);
		tempBuf[strlen(tempBuf) - 1] = 0;
		uniqueID = strtol(tempBuf, &ptr, 10);
		bufferWriteSuccess = snprintf(storageBuffer, BUFSIZ, "%d %d %d", UserType, newOrReturningUser,uniqueID );
	}

	else{
		printf("Not a valid option\n"
				"Please try again!\n\n");
		return -1;
	}
	return 1;
}

int createSocket(){
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket\n");
	return sockfd;

}

int createConnection(int portno, int sockfd, struct sockaddr_in serv_addr){
	int connectSuccess;

	connectSuccess = connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	return connectSuccess;
}


int sendToServer(char* storageBuffer, int sockfd, struct sockaddr_in serv_addr){
	int rwSuccess;
	rwSuccess = sendto(sockfd, storageBuffer, strlen(storageBuffer), 0,
			(struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
//	int receiveSuccess;
//	char data[1024];
//	receiveSuccess = receiveMessage(sockfd, serv_addr, 
	//printf("The sent message is %s and number of bytes sent is %d\n", storageBuffer, rwSuccess);
	if (rwSuccess < 0)
		error("ERROR writing to socket\n");
	return 1;
}

/*
 **********************************************************************************************************
 Function Name : Main
 Purpose	   : Program starts here
 Parameter     : None
 Return Value  : Integer
 *********************************************************************************************************
 */

int main(int argc, char *argv[])
{
	int rwSuccess, reponse_value, connectSuccess, sockfd, sendSuccess, userinfoReceived;
	const char* host = "localhost";
	char storageBuffer[BUFSIZ];
	struct hostent *server;
	struct sockaddr_in serv_addr;

	if(argc < 2){
		error("Invalid arguments");
		printf("Usage: ./client <Server port number>\n");
		exit(0);
	}

	int portno = atoi(argv[1]);

	for(int i=0; i<3; i++){ //Give 3 tries to the user to enter the right info
		userinfoReceived = userInfo(storageBuffer);
		if(userinfoReceived == -1){
			continue;
		}
		else{
		//	printf("Got the user info\n");
			break;
		}
	}

	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	sockfd = createSocket();

	connectSuccess= createConnection(portno, sockfd, serv_addr);

	if(connectSuccess != 0){
		printf("Error connecting\n");
	}

	sendSuccess = sendToServer(storageBuffer,sockfd, serv_addr);

	cout<<"Welcome suriya\n";
	cout<<"Your role is System Administrator\n";
}
