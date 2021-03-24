#include <cstdio>
#include <vector>
#include <iostream>
#include <new>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <stack>
#include <string>

using namespace std;

#define POSTOJANJE_GRAFA(Graf) if (Graf == nullptr) {\
							cout << "Graf ne postoji\n";\
							return;\
							}

struct Node {
	int id = 1;
	int cena = 0;
	bool izgradjen = true;
	Node* next = nullptr;
};


struct Grana {
	int id1, id2, tezina;
};



struct CvorDrumskog {
	int id=0;
	int duzina = INT_MAX;
};

struct prethodnik {
	int id_preth=-1;
	bool izgradjen = true;
};


struct Putevi {
	vector<int> daljina;
	vector<vector<prethodnik>> trag;
};

struct UporediGrane {
	bool operator()(Grana l, Grana d)
	{
		return l.tezina > d.tezina;
	}
};

struct porediDaljineDrumskog {
	bool operator()(CvorDrumskog l, CvorDrumskog d)
	{
		return l.duzina > d.duzina;
	}
};

bool poredjenjeBrojaNeizgradjenihPuteva(vector<int> l, vector<int> d)
{
	return l[0] < d[0];
}

void deleteGraph(vector<Node>* &Graf);

//pravi niz "hedera" koji pokazuju na prazne liste susednosti(prazan graf, bez grana, samo cvorovi)
//listi susednosti i-tog cvora se pristupa preko indeksa u vektoru, tako da se id polje hedera ne koristi za to
void createGraph(int n, vector<Node>* &Graf)
{
	if (Graf != nullptr)	deleteGraph(Graf);
	try {
		Graf = new vector<Node>(n);
	}
	catch (bad_alloc) {
		cout << "Greska pri alociranju memorije\n"; //GRESKA
	}
}



//dodaje cvor nakon cvora sa najvecim indeksom
void addNode(vector<Node>* Graf)
{
	POSTOJANJE_GRAFA(Graf)
	Graf->push_back(Node{});
}



void addEdge(int id1, int id2, int cena, bool izgradjen, vector<Node>* Graf)
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
				prev->next = new Node{ id2, cena, izgradjen, prev->next };
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
				prev->next = new Node{ id1, cena, izgradjen, prev->next };
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
			Graf->at(idBrisanja) = { -1, 0, 0, nullptr };
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
				for (Node* tren = Graf->at(i).next; tren != nullptr; tren = tren->next) {
					if(tren->izgradjen==true)
						printf("--->%3d ", tren->id);
					else
						printf("- ->%3d ", tren->id);
				}
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

//cena minimalnog stabla bice zapisana u prvom elementu rezultujuceg vektora, u polju "cena"
vector<Node>* minimalTree(vector<Node>* Graf)	
{
	vector<Node>* mst = nullptr;
	try {
		mst = new vector<Node>(Graf->size());
	}
	catch(bad_alloc){
		cout << "Greska pri alociranju memorije\n";
	}

	//primov alg
	int poseceniCvorovi = 0;
	srand((int)time(0));
	priority_queue <Grana, vector<Grana>, UporediGrane> putevi;
	vector<bool> visited(mst->size(), false);
	int randid;				//random prvi cvor
	do { randid = rand() % Graf->size(); } while (Graf->at(randid).id == -1);
	visited[randid] = true;
	poseceniCvorovi++;
	for (Node* j = Graf->at(randid).next; j != nullptr; j = j->next)
	{
		if (!visited[j->id]) {
			putevi.push(Grana{ randid, j->id, j->cena });	
		}
	}

	while (putevi.size() > 0 && poseceniCvorovi < static_cast<int>(mst->size())) {
		Grana g;
		do {
			if (putevi.size() == 0) return mst;		//kad je nepovezan, vraca se mst bez tih cvorova u stablu
			g = putevi.top();
			putevi.pop();
		} while (visited[g.id2]);

		visited[g.id2] = true; //visited[g.id1] = true;
		poseceniCvorovi++;
		addEdge(g.id1, g.id2, g.tezina, true, mst);
		mst->at(0).cena += g.tezina;
		
		
		for (Node* j = Graf->at(g.id2).next; j != nullptr; j = j->next)
		{
			if (!visited[j->id]) {
				putevi.push(Grana{ g.id2, j->id, j->cena });
			}
		}
	}
	return mst;
}


Putevi dijkstra(int id,vector<Node>* GrafDrumskog) {

	int br_nepovezanih = 0;
	for (auto i = GrafDrumskog->begin(); i != GrafDrumskog->end(); i++) if (i->next == nullptr) br_nepovezanih++; //broji nepovezane cvorove
	int preostalo = GrafDrumskog->size() - br_nepovezanih;
	
	vector<int> daljina(GrafDrumskog->size(), INT_MAX);
	vector<vector<prethodnik>> trag(GrafDrumskog->size(), vector<prethodnik> (1,prethodnik()));
	priority_queue<CvorDrumskog, vector<CvorDrumskog>, porediDaljineDrumskog> pq;
	vector<bool> visited(GrafDrumskog->size(), false);

	daljina[id] = 0;
	trag[0][0] = { -1 };
	pq.push(CvorDrumskog{ id, 0 });

	while (preostalo)
	{
		CvorDrumskog min;
		do { //uzima iz pq prvi koji je neposecen
			if (pq.size() == 0) return Putevi{ daljina,trag };	//ako je nepovezan, red je prazan, vracaju se ovi putevi
			min = pq.top();
			pq.pop();
		} while (visited[min.id]);
		visited[min.id] = true;
		preostalo--;
		for (Node* i = GrafDrumskog->at(min.id).next; i != nullptr; i = i->next) {//apdejtuje sve susede izvucenog cvora
			if (daljina[min.id] + i->cena < daljina[i->id]) {
				daljina[i->id] = daljina[min.id] + i->cena;
				pq.push({ i->id,daljina[i->id] });	//svaki cvor koji se izmeni se stavlja ponovo sa svojom udaljenoscu u pq
				if (trag[i->id].size() > 1) {
					trag[i->id].clear(); 
					trag[i->id].push_back(prethodnik{ min.id,i->izgradjen }); //ako ima vise prethodnika, a nadjen kraci put,sve ih brise
				}
				else {
					trag[i->id][0].id_preth = min.id;
					trag[i->id][0].izgradjen = i->izgradjen; 
				}
			}
			else if (daljina[min.id] + i->cena == daljina[i->id]) {//ako su jednaki, znaci da postoje dva puta do tog
				trag[i->id].push_back(prethodnik{ min.id,i->izgradjen });//sklopi ovo u jednu granu
			}
		}

	}


	return Putevi{ daljina,trag };
}


void ispisiPutevaDijkstra(int id,vector<Node>* GrafDrumskog)
{
	Putevi p = dijkstra(id, GrafDrumskog);
	for (int i = 0; i < static_cast<int>(GrafDrumskog->size()); i++)
	{
		if (i == id) continue;

		vector<vector<int>> svi_putevi;
		stack<prethodnik> stek;
		svi_putevi.push_back({});
		svi_putevi[0].push_back({ 0 });//br neizgradjenih se cuva na prvom mestu u vektoru puta
		int index_puta = 0;
		int tren = i;
		int br_puteva = 1;
		int gotovi_putevi = 0;

		//dodaje se prvi el
		stek.push({ i });
		do {
			tren = stek.top().id_preth;
			stek.pop();
			svi_putevi[index_puta].push_back(tren);
			do {

				if (p.trag[tren].size() > 1)		//grananje
				{
					for (auto j = ++p.trag[tren].begin(); j != p.trag[tren].end(); j++) {
						stek.push(*j);
						svi_putevi.push_back({});
						svi_putevi[br_puteva] = svi_putevi[index_puta];
						svi_putevi[br_puteva][0] += (j->izgradjen ? 0 : 1);
						br_puteva++;
					}
				}

				if (!p.trag[tren][0].izgradjen) svi_putevi[index_puta][0]++;
				tren = p.trag[tren][0].id_preth;

				svi_putevi[index_puta].push_back(tren);
			} while (tren != id);
			gotovi_putevi++;
			index_puta = br_puteva - gotovi_putevi;
		} while (!stek.empty());

		//sortiranje po broju neizgradjenih puteva
		sort(svi_putevi.begin(), svi_putevi.end(), poredjenjeBrojaNeizgradjenihPuteva);
		//sad zapravo ispis
		cout << "Put od " << id << " do " << i << endl;
		if (p.daljina[i] != INT_MAX) {
			cout << "Duzina puta:" << p.daljina[i] << endl;
			for (auto j = svi_putevi.begin(); j != svi_putevi.end(); j++)
			{
				cout << "Postoji neizgradjenih puteva:" << j->at(0) << endl << "Put:";
				for (auto k = --j->end(); k != j->begin(); k--) cout << "->" << *k;
				cout << endl;
			}
		}
		else { cout << "Ne postoji put"; }
		
		cout << endl;

	}
	
	
}


int main()
{
	vector<Node>* Graf = nullptr;
	vector<Node>* GrafDrumskog = nullptr;
	vector<Node>* mst = nullptr;
	while (1)
	{

		printf("~Meni~\n"
			"1.Kreiraj prazan graf\n"
			"2.Dodaj lokaciju\n"
			"3.Izbrisi lokaciju\n"
			"4.Dodaj put metrou\n"
			"5.Ukloni put metrou\n"
			"6.Dodaj drumski put\n"
			"7.Ukloni drumski put\n"
			"8.Prikazi reprezentaciju grafa(metro)\n"
			"9.Prikazi reprezentaciju grafa(drumski)\n"
			"10.Obrisi graf(metro)\n"
			"11.Obrisi graf(drumski)\n"
			"12.Izracunaj minimalno stablo(Metro sa minimalnom cenom)\n"
			"13.Racunanje najkracih puteva\n"
			"14.Unos grafa u jednom redu\n"
			"0.Prekini program\n"
			"Uneti sifru zeljene operacije: ");
		int i = 0;
		int n;
		cin >> i; cout << endl;
		switch (i)
		{
		default:cout << "Nedozvoljena opcija. Odaberi neku od ponudjenih opcija." << endl; break;
		case 0:exit(0); break;
		case 1:
		{
			printf("Kreiranje praznog grafa. Uneti broj cvorova: ");
			//int n = 0;
			cin >> n; cout << endl;
			createGraph(n, Graf);
			createGraph(n, GrafDrumskog);
			break;
		}
		case 2: addNode(Graf); addNode(GrafDrumskog); break;
		case 3:
		{
			cout << "Uneti ID cvora za brisanje: ";
			int id;
			cin >> id;
			deleteNode(id, Graf);
			deleteNode(id, GrafDrumskog);
			break;
		}
		case 4:
		{
			cout << "Dodavaje puta izmedju lokacije 1 i  lokacije 2.\nLokacija 1: ";
			int idA1, idA2, cena;
			cin >> idA1;
			cout << "Lokacija 2: ";
			cin >> idA2;
			cout << "Cena:";
			cin >> cena;
			cout << endl;
			addEdge(idA1, idA2, cena, true, Graf);
			break;
		}
		case 5:
		{
			cout << "Uklanjanje puta izmedju lokacije 1 i  lokacije 2.\nLokacija 1: ";
			int id1, id2;
			cin >> id1;
			cout << "Lokacija 2: ";
			cin >> id2;
			cout << endl;
			deleteEdge(id1, id2, Graf);
			break;
		}
		case 6:
		{
			cout << "Dodavaje puta izmedju lokacije 1 i  lokacije 2.\nLokacija 1: ";
			int idA1, idA2, cena;
			cin >> idA1;
			cout << "Lokacija 2: ";
			cin >> idA2;
			cout << "Cena:";
			cin >> cena;
			cout << "Izgradjen?(1-da, 0-ne)";
			bool izgradjen;
			cin >> izgradjen;
			cout << endl;
			addEdge(idA1, idA2, cena, izgradjen, GrafDrumskog);
			break;
		}
		case 7:
		{
			cout << "Uklanjanje puta izmedju lokacije 1 i  lokacije 2.\nLokacija 1: ";
			int id1, id2;
			cin >> id1;
			cout << "Lokacija 2: ";
			cin >> id2;
			cout << endl;
			deleteEdge(id1, id2, GrafDrumskog);
			break;
		}
		case 8: printGraph(Graf); break;
		case 9:printGraph(GrafDrumskog); break;
		case 10:deleteGraph(Graf); break;
		case 11:deleteGraph(GrafDrumskog); break;
		case 12:
			mst = minimalTree(Graf);
			cout << "Minimalno stablo\n";
			printGraph(mst);
			cout << "Minimalna cena je: " << mst->at(0).cena << endl;
			break;
		case 13:
			cout << "ID lokacije od koje se racunaju najkraci putevi: ";
			cin >> i;
			cout << "Najkraci putevi" << endl;
			ispisiPutevaDijkstra(i, GrafDrumskog);
			break;
		case 14:
			cout<< "0. Metro\n"
				"1.Drumski\n";
			cin >> i;
			cout << "Uneti broj kljucnih lokacija u gradu: ";
			cin >> n;
			string format;
			if (i == 0) {
				format = "lokacija1 lokacija2 cena";
				createGraph(n, Graf);
			}
			else {
				format = "lokacija1 lokacija2 duzina izgradjen";
				createGraph(n, GrafDrumskog);
			}
			
			cout << "Uneti izmedju kojih lokacija je moguca izgradnja metroa, i koliko bi ona kostala, u sledecem formatu:\n" << format << " \nUnos se prekida kada se na ulazu nadje -1.\n";
			while (1)
			{
				int id1, id2, cena;
				cin >> id1;
				if (id1 == -1) break;
				else {
					cin >> id2 >> cena;
					int izgradjen = true;
					if (i == 0) addEdge(id1, id2, cena, izgradjen, Graf);
					else {
						cin >> izgradjen;
						addEdge(id1, id2, cena, izgradjen, GrafDrumskog);
					}
				}
			}
			break;
		}

	}

	return 0;
}