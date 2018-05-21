#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cmath>
#include <time.h>
#include <winsock.h>
#include <winsock2.h>
#include <Windows.h>  
#include <algorithm>
#include "2dtree.h"

using namespace std;
#pragma comment(lib,"winsock.lib")
#pragma comment(lib,"ws2_32.lib")
//x -> [115.375000, 117.488289]
//y -> [39.416672, 41.083328]
//0 30482
//1 31061
//2 19856
//3 9580

int main(int argc, char** argv) {
	ifstream fin("taxi.txt");
	string s;
	
	while(getline(fin, s)) {
		stringstream ss;
		ss << s;
		int id, number;
		float x1, y1;
		float x2, y2;
		ss >> id >> number >> x1 >> y1;
		if (number < 4) {
			Node node;
			node.id = id;
			node.x = x1;
			node.y = y1;
			node.number = number;
			for (int i = 0; i < number; i++)
				ss >> node.X[i] >> node.Y[i];
			insert(number, node);
		}
	}
    fin.close();

	WORD socketVersion = MAKEWORD(2,2);    
    WSADATA wsaData;    
    if (WSAStartup(socketVersion, &wsaData)!=0)
        return 0;        
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
    if(slisten == INVALID_SOCKET) {    
        printf("socket error !");    
        return 0;    
    }
    
    sockaddr_in sin;    
    sin.sin_family = AF_INET;    
    sin.sin_port = htons(9000);    
    sin.sin_addr.S_un.S_addr = INADDR_ANY;     
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)      
        printf("bind error !");    
        
    if(listen(slisten, 5) == SOCKET_ERROR) {    
        printf("listen error !");  
        return 0;    
    }    
       
    SOCKET sClient;    
    sockaddr_in remoteAddress;    
    int nAddrlen = sizeof(remoteAddress);    
    char receive_data[255];     

    printf("waiting for connection...\n");
    while (true) {   
        sClient = accept(slisten, (SOCKADDR *)&remoteAddress, &nAddrlen);    
        if(sClient == INVALID_SOCKET) {    
            printf("accept error !");    
            continue;
        }    
        printf("get a new connection: %s\n", inet_ntoa(remoteAddress.sin_addr));    
  
        int value = recv(sClient, receive_data, 255, 0);           
        
        if(value > 0) {    
            receive_data[value] = 0x00;    
            // printf(receive_data);
            stringstream query;
            query << receive_data;
            float x1, y1, x2, y2;
            query >> x1 >> y1 >> x2 >> y2;
            cout << x1 << " " << y1 << " " << x2 << " " << y2;
            
            clock_t start = clock();
            vector<Node> r;
            knn_search(x1, y1, x2, y2, &r);
            clock_t ends = clock();
            
            cout <<"Running Time : "<<(double)(ends - start)/ CLOCKS_PER_SEC << endl;
            // cout << r.size() <<" " << r[0].number <<" " << r[1].number<<" "<<r[2].number;
            // cout <<" " << r[3].number<<" " << r[4].number;

            stringstream reply;
            int n = 5;
            if (r.size() > 0) {
                if (r.size() < 5)
                    n = r.size();
                reply << n << '\n';
                for (int i = 0; i < n; i++) {
                    reply << r[i].id << '_' << r[i].x << '_' << r[i].y << ' ';
                    for (int j = 0; j < r[i].number; j++)
                        reply << r[i].X[j] << '_' << r[i].Y[j] << '_';
                    reply << '\n';
                }
            } else 
                reply << 0;

            string send_data = reply.str();
            send(sClient, send_data.c_str(), send_data.size(), 0);    
            closesocket(sClient); 
        } else
            continue;     
    }
    closesocket(slisten);    
    WSACleanup();    
}

