#include <iostream>
#include <string.h>
#include <cstdlib>
#include <math.h>

#define MAX_LENGTH 255

using namespace std;

void clear() {
	system("cls");
	cout << "\t               Calculadura de VLSM\n\t Elaborada por Angello Jaimes - Nicolas Cubillos - Esteban Rojas\n";
}

void print_ip (unsigned char* IP) {
	int i;
	for (i = 0; i < 4; i++) {
		cout << (int)IP[i];
		if (i < 3)
			cout << ".";
	}
}

bool validar_red (unsigned char* IP, int mask) {
	
	int i, j, contador = 0;
	
	for (i = 3; i >= 0; i--) {
		for (j = 0; j < 8; j++) {
			if (IP[i] & (1 << j)) return false;
			contador++;
			if (contador == 32 - mask) return true;
		}
	}
	
	return true;
}

unsigned char* convertir_dir (char* str, int &mask) {
	unsigned char* IP = new unsigned char [4];
	char* token = NULL;
	int i;
	
	for (i = 0; i < 4; i++) {
		if (!i) token = strtok (str, ".");
		else token = strtok (NULL, ".");
		
		if (token == NULL || atoi(token) > 255 || atoi(token) < 0)
			return NULL;
		
		IP[i] = atoi(token);
	}
	
	if ((token = strtok(token, "/")) == NULL)
		return NULL;
		
	token = strtok(NULL, "");
	
	mask = atoi(token);
	
	if(!validar_red(IP, mask)) return NULL;
	
	delete[] str;
	return IP;
}

bool validar_capacidad(int* cantidadHost, int subredes, int mask) {
	int i;
	long int suma = 0; // 2^31
	
	for (i = 0; i < subredes; i++) 
		suma += cantidadHost[i];
	
	return (suma < pow(2, 32 - mask)); // bits de host
}

void ordenar_cantidad(int* &cantidadHost, int subredes) {
	int i, j, aux;
	
	for (i = 0; i < subredes - 1; i++) {
		for (j = 0; j < subredes - 1; j++) {
			if (cantidadHost[j] < cantidadHost[j + 1]) {
				aux = cantidadHost[j];
				cantidadHost[j] = cantidadHost[j + 1];
				cantidadHost[j + 1] = aux;
			}
		}
	}
}

void imprimir_cantidad(int* cantidadHost, int subredes) {
	int i;
	cout << "\n\t\t   Requerimientos de la red:\n\n";
	for (i = 0; i < subredes; i++) {
		cout << "\t\t\tRed " << i + 1 << ": " << cantidadHost[i] << " redes.\n";
	}
	cout << "\n";
}

void calcular_subnet(unsigned char* dirRed, int* &cantidadHost, int subredes, int mask) {
	clear();
	int nbits, i, j, salto, ultimo_octeto = 0, maskAnterior;
	
	ordenar_cantidad(cantidadHost, subredes);
	imprimir_cantidad(cantidadHost, subredes);
	
	cout << "\t\tSubred\tHosts\t   Dir. de red\n";
	
	for (i = 0; i < subredes; i++) {
		nbits = ceil(log(cantidadHost[i] + 2) / log(2));
		nbits == 1 ? nbits = 2 : nbits;
		
		maskAnterior = mask;
		mask += 32 - (mask + nbits);
		
		if (i) {
			salto = 256 - ultimo_octeto;
			dirRed[(maskAnterior - 1) / 8] += salto;
		}
		
		cout << "\t\t   " << i + 1 << "\t  " << cantidadHost[i] << "\t";
		print_ip(dirRed);
		cout << "/" << mask << "\n\n";
		
		for (j = 0, ultimo_octeto = 0, salto = 0; j < mask % 8; j++)
			ultimo_octeto += pow (2, 7 - j);
			
		ultimo_octeto = ultimo_octeto > 0 ? ultimo_octeto : 255;
	}
}

void menu() {
	unsigned char* dirRed = NULL;
	char* buffer = NULL;
	int subredes, i, mask;
	int* cantidadHost = NULL;
	
	do {
		clear();
		cout << "\t\t\t[!] Para salir, ingrese -1\n\n";
		cout << "\t Ingrese la direccion de red a la que se hara subnetting\n";
		cout << "\t\t\t ej.: 192.168.10.0/24\n\n\t\t\t ";
		
		buffer = new char [MAX_LENGTH];
		if (dirRed != NULL) cin.ignore();
		cin.getline(buffer, MAX_LENGTH);
		
		if ((dirRed = convertir_dir (buffer, mask)) == NULL) {
			cout << "\n\t [!] La direccion ingresada no es una direccion de red.\n";
			system("Pause");
			continue;
		} else {
			cout << "\t[!] Direccion recibida: ";
			print_ip (dirRed);
			cout << " con mascara de red de [" << mask << "] bits. \n";
			cout << "\tIngrese la cantidad de subredes que desea tener en la red: ";
			cin >> subredes;
			
			if (subredes < 1) {
				cout << "\n\t [!] Error con el numero de subredes. Tiene que ser mayor que cero (0).\n";
				system("Pause");
				continue;
			}
			
			cantidadHost = new int [subredes];
			
			for (i = 0; i < subredes; i++) {
				cantidadHost[i] = -1;
				do {
					cout << "\tIngrese la cantidad de host que quiere para la red " << i+1 << ": ";
					cin >> cantidadHost[i];
				} while (cantidadHost[i] < 0);
			}
			
			if (!validar_capacidad(cantidadHost, subredes, mask)) {
				cout << "\n\t [!] Error. El numero de direcciones solicitadas no se pueden obtener desde esta red.\n";
				system("Pause");
				continue;
			}
			
			calcular_subnet(dirRed, cantidadHost, subredes, mask);
		}
		
		system("pause");
		
		delete[] cantidadHost;
		
	} while (strcmp(buffer, "-1"));
}

int main () {
	menu();
}
