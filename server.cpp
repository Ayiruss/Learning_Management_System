
// Program: TCPServer
// Subject: CMPE 207
// Project: Distributed Learning Management System
// Created by Team 8.


/*****SERVER PROGRAM*****/
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <sstream>
//#include "dbaccess.h"
using namespace std;


char buffer[BUFSIZ];

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

string quotesql( const string& s){
	return string("'") + s + string("'");
}

void dbinsert(int id, string name, string user_type)
{
	sqlite3 *db;
	string stmt;
	sqlite3_stmt *sqlstmt;
	string s = to_string(id);
	stmt = "INSERT INTO USERS(ID,USER_TYPE,NAME) VALUES("
		+ s + "," + quotesql(user_type)+ "," + quotesql(name) + ");";
	string str = string(stmt);
	if(sqlite3_open("test.db", &db) == SQLITE_OK){
		sqlite3_prepare( db, str.c_str(), -1, &sqlstmt, NULL);
		sqlite3_step(sqlstmt);
		cout<<"New User Raj Inserted into Database";
	}
	else{
		cout<< "Failed to open db\n";
	}
	sqlite3_finalize(sqlstmt);
	sqlite3_close(db);
}
string dbretrieve(int id)
{
	cout<<"came inside dbretreive" <<endl;
	sqlite3 *db;
	sqlite3_stmt *stmt;
	string result;
	string statement  = "SELECT name, user_type from USERS where id="+ to_string(id)+ ";";
	if(sqlite3_open("test.db",&db) == SQLITE_OK){
		sqlite3_prepare(db, statement.c_str(), -1, &stmt, NULL);
		sqlite3_step(stmt);
		string result = string((char *)sqlite3_column_text(stmt,0));
//		for(int i=0;i<3;i++){
			cout<<((char *)sqlite3_column_text(stmt,0))<<"\t";
//		}	
	
	}
	cout<<"The result inside dbretireve is:"<<result<<endl;
return result;
}
		
void dbcreate()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;

	rc = sqlite3_open("test.db", &db);

	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		//    return(0);
	}else{
		fprintf(stderr, "Opened database successfully\n");
	}
	sql = "CREATE TABLE USERS("  \
			"ID INT PRIMARY KEY     NOT NULL," \
			"USER_TYPE      TEXT    NOT NULL," \
			"NAME           TEXT    NOT NULL);"; 

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db);
}


void error(const char *msg)
{
	perror(msg);
	printf ("%s\n", strerror(errno));
	exit(1);
}



int decodeBuffer(char* buffer){
	char* parsing;
	char tempbuf[20];
	//	printf("The message received by the decode buffer is %s\n", buffer);
	string temp;
	istringstream words(buffer);
	vector<string> parsed;
	while(words)
	{
		words >> temp;
		parsed.push_back(temp);
	}

	if(parsed[0] == "1"){
		printf("The user is a System Admin\n");
	}
	else if(parsed[0] == "2"){
		printf("The user is an Instructor\n");
	}
	else if(parsed[0] == "3"){
		printf("The user is a student\n");
	}

	if(parsed[1] == "1"){
		printf("The user is new\n");

	}
	else if(parsed[1] == "2"){
		printf("The user is returning\n");
	}


	return 1;
}

int receiveMessage(int newsockfd, struct sockaddr_in cli_addr, socklen_t client_length, char* buffer){
	int receiveSuccess;
	char* subchar;
	int i=0;
	bzero(buffer,BUFSIZ);
	receiveSuccess = recvfrom(newsockfd, buffer, BUFSIZ, 0, (struct sockaddr *) &cli_addr, &client_length);
	if (receiveSuccess < 0) error("ERROR reading from socket\n");
	istringstream buf(buffer);
	istream_iterator<string> beg(buf),end;
	vector<string> tokens(beg,end);
	string str = tokens.at(i++);	
	if(!str.compare("1")){
		str = tokens.at(i++);
		if(!str.compare("1")){
				string name = tokens.at(i++);
				int id = stoi(tokens.at(i));
				string user_type = "System Administrator";
				dbinsert(id, name, user_type);
			}
		
		else if(!str.compare("2")){
				int id = stoi(tokens.at(i));
				string result = dbretrieve(id);
				cout<<result;
		}
	}
	return receiveSuccess;
}


int createSocket(){
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket\n");

	return sockfd;
}

int binding(int sockfd,struct sockaddr_in serv_addr ){
	int bindSuccess;
	bindSuccess = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (bindSuccess < 0)
		error("Binding Error!\n");

	return bindSuccess;
}

int listenNaccept(int sockfd, int portno, char* buffer){
	pid_t childpid;
	struct sockaddr_in serv_addr, cli_addr;
	int newsockfd, receiveSuccess, decodeSuccess;
	socklen_t client_length;


	listen(sockfd, 5);
	printf("The server is listening on the port %d\n\n\n", portno);


	for(;;){
		memset(&cli_addr, 0, sizeof(cli_addr));
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &client_length);
		if (newsockfd < 0)
			error("ERROR on accept\n");

		childpid = fork();
		if (childpid == -1) {
			close(newsockfd);
		}

		else if(childpid > 0){
			close(newsockfd);
		}

		else if(childpid == 0){
			receiveSuccess = receiveMessage(newsockfd, cli_addr, client_length, buffer);
			decodeSuccess = decodeBuffer(buffer);
			return receiveSuccess;
		}
	}

}

int main(int argc, char *argv[])
{
	int sockfd, portno, bindSuccess, receiveSuccess;
	size_t lengthOfMessageReceived;
	socklen_t client_length, server_length;
	struct sockaddr_in serv_addr;
	if(argc < 2){
		error("Invalid arguments\n");

		printf("Usage: ./server <Server port number>\n");
		exit(0);
	}

	//dbcreate();

	printf("\nHello! \n");

	portno = atoi(argv[1]);
	sockfd = createSocket();

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port =  htons(portno);

	bindSuccess = binding(sockfd, serv_addr );

	receiveSuccess = listenNaccept(sockfd, portno, buffer);
	cout<<receiveSuccess;
	close(sockfd);
	return 0;
}
