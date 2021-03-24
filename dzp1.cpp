#include <iostream>
#include <cstdio>
#include <vector>
#include <new>
using namespace std;

#define POSTOJANJE_GRAFA(Graf) if (Graf == nullptr) {\
							cout << "Graf ne postoji\n";\
							return;\
							}

struct Node {
	int id=1;
	Node* next=nullptr;
};

void deleteGraph(vector<Node>* &Graf);

//pravi niz "hedera" koji pokazuju na prazne liste susednosti(prazan graf, bez grana, samo cvorovi)
//listi susednosti i-tog cvora se pristupa preko indeksa u vektoru, tako da se id polje hedera ne koristi za to
void createGraph(int n, vector<Node>* &Graf)
{	
	if (Graf != nullptr)	deleteGraph(Graf);
	try {
		Graf = new vector<Node>(n);
	}
	catch(bad_alloc){
		cout << "Greska pri alociranju memorije\n"; 
	}
}

//dodaje cvor nakon cvora sa najvecim indeksom
void addNode(vector<Node>* Graf)
{
	POSTOJANJE_GRAFA(Graf)
	Graf->push_back(Node{});

}


void addEdge(int id1, int id2, vector<Node>* Graf)
{
	POSTOJANJE_GRAFA(Graf)
		if (id1 >= static_cast<int>(Graf->size()) || id2 >= static_cast<int>(Graf->size()) || Graf->at(id1).id == -1 || Graf->at(id2).id == -1) cout << "Cvor ne postoji";
		else {
			Node *prev = &Graf->at(id1), *tren = prev->next;		//dodavanje grane u listu susednosti prvog cvora
			while (tren != nullptr && id2 >= tren->id)
			{
				if (id2 == tren->id) { cout << "Grana vec postoji\n"; return; }
				prev = tren;
				tren = tren->next;
			}

			try {
				prev->next = new Node{ id2,prev->next };
			}
			catch (bad_alloc) {
				cout << "Greska pri alociranju memorije\n";
			}

			if (id1 == id2) return; //grana se vraca u isti cvor

			prev = &Graf->at(id2);			//dodavanje grane u listu susednosti drugog  cvora
			tren = prev->next;
			while (tren != nullptr && id1 >= tren->id)
			{
				prev = tren;
				tren = tren->next;
			}
			try {
				prev->next = new Node{ id1,prev->next };
			}
			catch (bad_alloc) {
				cout << "Greska pri alociranju memorije\n";
			}
		}
}

void deleteEdge(int id1, int id2, vector<Node>* Graf)
{
	POSTOJANJE_GRAFA(Graf)
	if (id1 >= static_cast<int>(Graf->size()) || id2 >= static_cast<int>(Graf->size()) || Graf->at(id1).id == -1 || Graf->at(id2).id == -1) cout << "Cvor ne postoji"; //GRESKA
	else {
		Node *prev = &Graf->at(id1), *tren = prev->next;
		while (tren != nullptr && tren->id != id2)
		{
			prev = tren;
			tren = tren->next;
		}
		if (tren == nullptr) {				
			cout << "Grana ne postoji\n";
			return;
		}
		else {
			prev->next = tren->next;
			delete tren;
		}

		prev = &Graf->at(id2);
		tren = prev->next;
		while (tren != nullptr && tren->id != id1)
		{
			prev = tren;
			tren = tren->next;
		}
		if (tren == nullptr) { cout << "Grana ne postoji\n";	return; }	
		else {
			prev->next = tren->next;
			delete tren;
		}
	}
}


//uklanja cvor tako sto brise sve njegove grane, i zatim oznaci da je izbrisan tako sto id njegovog hedera postavi na -1
void deleteNode(int idBrisanja, vector<Node>* Graf)
{
	POSTOJANJE_GRAFA(Graf)
	if (idBrisanja >= static_cast<int>(Graf->size())) cout << "Cvor ne postoji\n";
	else if (Graf->at(idBrisanja).id == -1) cout << "Cvor je vec izbrisan\n";
	else {
		Node* iterator1 = Graf->at(idBrisanja).next;
		while (iterator1)
		{
			Node* tren = iterator1;
			iterator1 = iterator1->next;
			deleteEdge(idBrisanja, tren->id, Graf);
		}
		Graf->at(idBrisanja) = { -1,nullptr };
	}
}

void printGraph(vector<Node>* Graf)
{
	POSTOJANJE_GRAFA(Graf)
	for (int i = 0; i < static_cast<int>(Graf->size()); i++)
	{
		if (Graf->at(i).id != -1)
		{
			printf("%3d|", i);
			for (Node* tren = Graf->at(i).next; tren != nullptr; tren = tren->next) printf("-->%3d ", tren->id);
			printf("\n");
		}
	}
}

void deleteGraph(vector<Node>* &Graf)
{
	POSTOJANJE_GRAFA(Graf)
	for (int i = 0; i < static_cast<int>(Graf->size()); i++)
	{
		for (Node* tren = Graf->at(i).next; tren != nullptr;) {
			Node* prim = tren;
			tren = tren->next;
			delete prim;
		}
	}
	Graf->clear();
	delete Graf;//[]
	Graf = nullptr;

}

int main()
{
	vector<Node>* Graf=nullptr;
	while (1)
	{
		printf("~Meni~\n"
			"1.Kreiraj prazan graf\n"
			"2.Dodaj cvor\n"
			"3.Izbrisi cvor\n"
			"4.Dodaj granu\n"
			"5.Ukloni granu\n"
			"6.Prikazi reprezentaciju grafa\n"
			"7.Obrisi graf\n"
			"0.Prekini program\n"
		"Uneti sifru zeljene operacije: ");
		int i = 0;
		cin >> i; cout << endl;
		switch (i)
		{
		case 0:exit(0); break;
		case 1: 
		{
			printf("Kreiranje praznog grafa. Uneti broj cvorova: ");
			int n = 0;
			cin >> n; cout << endl;
			createGraph(n,Graf);
			break; 
		}
		case 2: addNode(Graf); break;
		case 3: 
		{
			cout << "Uneti ID cvora za brisanje: ";
			int id;
			cin >> id;
			deleteNode(id, Graf);
			break;
		}
		case 4:
		{
			cout << "Dodavaje grane izmedju Cvora 1 i  Cvora 2.\nCvor 1: ";
			int idA1, idA2;
			cin >> idA1;
			cout << "Cvor 2: ";
			cin >> idA2;
			cout << endl;
			addEdge(idA1, idA2, Graf);
			break;
		}
		case 5:
		{
			cout << "Uklanjanje grane izmedju Cvora 1 i  Cvora 2.\nCvor 1: ";
			int id1, id2;
			cin >> id1;
			cout << "Cvor 2: ";
			cin >> id2;
			cout << endl;
			deleteEdge(id1, id2, Graf);
			break;
		}
		case 6: printGraph(Graf); break;
		case 7:deleteGraph(Graf); break;
		}
	}
	return 0;

}