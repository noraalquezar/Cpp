#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 

//CBBNode

class CBBNode {
//Per cada node necessitem desar: node inicial, 
// llista de decisions que s’han pres fins arribar 
// a ell i node al qual s’ha arribat.
public:
	list<CVertex*> m_Index; //llista de decisions que s’han pres fins arribar a ell
	double m_Length; //funcio estimacio
	int m_Visited;

public:
	//Copia d'un node pots crear noves branques sense modificar el node original.
	//Es esencial per explorar diferents camins sense afectar el cami actual.
	CBBNode(CBBNode& Node)
	{
		m_Index = Node.m_Index;
		m_Length = Node.m_Length;
		m_Visited = Node.m_Visited;
	}

	//Estás construyendo una representación de la solución parcial asociada con ese nodo
	//Construccion incremental de soluciones
	CBBNode(CVertex* Visit, double length)
	{
		//Conjunto de decisiones tomadas hasta el momento en la construcción de la solución
		m_Index.push_back(Visit);
		m_Length = length;
		m_Visited = 1;
	}
};

//Compara si l'allargada del node del primer es mes gran que la del segon. 
//AMPLADA PRIORITARIA
struct CBBNodeComparator {
	bool operator()(const CBBNode* n1, const CBBNode* n2) {	
		return n1->m_Length > n2->m_Length;
	}
};

// SalesmanTrackBranchAndBound1 ===================================================
CTrack SalesmanTrackBranchAndBound1(CGraph& graph, CVisits& visits)
{
	CVisits visites1 = visits;
	visites1.m_Vertices.pop_back();

	CVertex* actual;
	list<CEdge*> aux;
	pair<list<CEdge*>, double> Matriu[10][10];

	int posicioI = 0;
	int posicioJ = 0;

	//Creem la matriu
	//Fila, hi haura una menys que columna ja que no començaras amb la visita de desti
	for (CVertex* v : visites1.m_Vertices) {	
		v->m_Index = posicioI;
		posicioJ = 0;
		//Trobar la distancia mes curta de la visita amb totes els altres
		DijkstraQueue(graph, v);
		//Columna
		for (CVertex* v2 : visits.m_Vertices) {
			aux.clear();
			actual = v2;
			////anem tirant enrere de vèrtex en vèrtex fins arribar al vèrtex amb el que estem treballant 
			while (actual != v) {	
				//d'entre totes les arestes del graf, busquem aquella anterior al vèrtex i l'afegim a aux
				for (CEdge& e : graph.m_Edges) {
					if (e.m_pOrigin == actual->m_pDijkstraPrevious->m_pOrigin && e.m_pDestination == actual) {
						aux.push_front(&e);
						actual = actual->m_pDijkstraPrevious->m_pOrigin;
						break;
					}
				}
			}
			//1ra posicio (aux, llista d'arestes del cami de dijsktra parcial, 2na posicio distancia dijkstra
			Matriu[posicioI][posicioJ] = make_pair(aux, v2->m_DijkstraDistance);
			if (((v2->m_DijkstraDistance > 0) && (v2->m_DijkstraDistance < Matriu[visits.GetNVertices() - 1][posicioJ].second)) || (Matriu[visits.GetNVertices() - 1][posicioJ].second == NULL)) {
				Matriu[visits.GetNVertices() - 1][posicioJ].second = v2->m_DijkstraDistance;
			}
			posicioJ++;
		}
		posicioI++;
	}

	CTrack track(&graph);
	//Agafes la primera visita
	//SOLUCIO ARREL camí que només conté el vèrtex origen
	visites1.m_Vertices.pop_front();
	if (visites1.m_Vertices.empty()) {
		track.m_Edges = Matriu[0][1].first;
		return track;
	}

	//Crees la cua amb prioritat i afegeixes a la cua la primera visita de la llista
	priority_queue<CBBNode*, std::vector<CBBNode*>, CBBNodeComparator> CBBPriorityQueue;
	
	//Constructor d'inicialització de la cua de prioritat
	CBBPriorityQueue.push(new CBBNode(visits.m_Vertices.front(), 0));


	list<CVertex*> path;
	CVertex* LastVisited;

	//Constructor de copia
	CBBNode* CBBNODEActual;
	CBBNode* newNode;

	double length;
	double bDist = DBL_MAX;

	//Mentres existeixin nodes vius 
	//cada iteración del bucle representa un nivell diferent de l'arbre
	//SELECCIÓ
	while (!CBBPriorityQueue.empty()) {
		//El node seleccionat es treu de la cua i es ramifica.
		CBBNODEActual = CBBPriorityQueue.top();
		CBBPriorityQueue.pop();

		//Guardar l'ultim vertex que hem guardat
		//Afegir al camí de la solució parcial un dels veïns del vèrtex final del camí.
		LastVisited = CBBNODEActual->m_Index.back();
		//SI ES UNA SOLUCIÓ COMPLETA
		//Si el numero de la visita del node actual es la mateixa que el numero total de visites:
		if (CBBNODEActual->m_Visited == visites1.GetNVertices() + 1) {
			//La distancia sera la distancia del que ja es tenia mes la del vertex final
			length = CBBNODEActual->m_Length + Matriu[LastVisited->m_Index][visits.GetNVertices() - 1].second;
			//Heurística simple
			//Seleccionar sempre la solució parcial més curta
			if (length < bDist) {
				bDist = length;
				//Afegir la solucio optima al cami 
				path = CBBNODEActual->m_Index;
			}
		}
		//SOLUCIO PARCIAL Si encara no s'ha arribat al total de visites:
		else {
			//RAMIFICACIÓ  genera els seus nodes fills
			//Es verifica que cada node fill compleix les condicions per ser 
			//una solució parcial completable.
			for (CVertex* v2 : visites1.m_Vertices) {
				//COTA
				//Si la visita encara no esta en el cami
				if (find(CBBNODEActual->m_Index.begin(), CBBNODEActual->m_Index.end(), v2) == CBBNODEActual->m_Index.end()) {
					// Per cada node viu calculem una funció d’estimació (lo prometedor que
					//és construir la solució a partir d’aquest node.)

					//Funcio solucio= part construida + estimacio(heuristica)
					length = CBBNODEActual->m_Length + Matriu[LastVisited->m_Index][v2->m_Index].second;
					
					//Poda per no complir les condicions de solució parcial completable
					//HEURISTICA SIMPLE: Seleccionar sempre la solució parcial més curta

					//COTA
					if (length < bDist) {
						//Es fa una copia del node actual
						newNode = new CBBNode(*CBBNODEActual);
						newNode->m_Visited++;
						newNode->m_Length = length;
						newNode->m_Index.push_back(v2);

					//S'inserta en una cua ordenada pel valor de l’estimació.
					//Poda dels nodes amb els valors d’estimació més dolents
					//Ja que no arribaran al inici de la cua quan ja s’hagi trobat la solució
						CBBPriorityQueue.push(newNode);
					}
				}
			}
		}
		delete CBBNODEActual;
	}
	//Crear el track (el path conte l'ordre de les visites, pero falta ajuntarles amb els altres vertex)
	CVertex* anterior = visits.m_Vertices.front();
	path.pop_front();
	for (CVertex* v : path) {
		for (CEdge* e : Matriu[anterior->m_Index][v->m_Index].first) {
			track.m_Edges.push_back(e);
		}
		anterior = v;
	}
	//Falta afegir l'ultim element, el desti. 
	for (CEdge* e : Matriu[path.back()->m_Index][visits.GetNVertices() - 1].first) {
		track.m_Edges.push_back(e);
	}
	return track;
	
}


// SalesmanTrackBranchAndBound2 ===================================================

class CBBNode_Weight {
public:
	list<CVertex*> m_Index;
	double m_WeigthMin;
	int m_Visited;
public:
	CBBNode_Weight(CBBNode_Weight& Node)
	{
		m_Index = Node.m_Index;
		m_WeigthMin = Node.m_WeigthMin;
		m_Visited = Node.m_Visited;
	}
	CBBNode_Weight(CVertex* Visit, double WeigthMin)
	{
		m_Index.push_back(Visit);
		m_WeigthMin = WeigthMin;
		m_Visited = 1;
	}
};


// comparator ==================================================================

struct CBBNodeComparator_Weight {
	bool operator()(const CBBNode_Weight* n1, const CBBNode_Weight* n2) {
		return n1->m_WeigthMin > n2->m_WeigthMin;
	}
};

// SalesmanTrackBranchAndBound2 ===================================================

CTrack SalesmanTrackBranchAndBound2(CGraph& graph, CVisits& visits)
{
	CVisits visites2 = visits;
	visites2.m_Vertices.pop_back();

	int nVisits = visits.GetNVertices();

	CVertex* actual;
	list<CEdge*> aux;
	pair<list<CEdge*>, double> Matriu[18][18];

	int posicioI = 0;
	int posicioJ = 0;

	for (CVertex* v : visites2.m_Vertices) {
		v->m_Index = posicioI;
		posicioJ = 0;
		DijkstraQueue(graph, v);
		for (CVertex* v2 : visits.m_Vertices) {
			aux.clear();
			actual = v2;
			while (actual != v) {
				for (CEdge& e : graph.m_Edges) {
					if (e.m_pOrigin == actual->m_pDijkstraPrevious->m_pOrigin && e.m_pDestination == actual) {
						aux.push_front(&e);
						actual = actual->m_pDijkstraPrevious->m_pOrigin;
						break;
					}
				}
			}
			Matriu[posicioI][posicioJ] = make_pair(aux, v2->m_DijkstraDistance);
			if (((v2->m_DijkstraDistance > 0) && (v2->m_DijkstraDistance < Matriu[nVisits - 1][posicioJ].second)) || (Matriu[nVisits - 1][posicioJ].second == NULL)) {
				Matriu[nVisits - 1][posicioJ].second = v2->m_DijkstraDistance;
			}
			posicioJ++;
		}
		posicioI++;
	}

	CTrack track(&graph);

	visites2.m_Vertices.pop_front();
	if (visites2.m_Vertices.empty()) {
		track.m_Edges = Matriu[0][1].first;
		return track;
	}
	priority_queue<CBBNode_Weight*, std::vector<CBBNode_Weight*>, CBBNodeComparator_Weight> CBBPriorityQueue;
	double WeigthMin = 0;

	//Suma de totes les visites si s'utilitza el minim de les distancies.
	for (int j = 1; j < nVisits; j++) {
		WeigthMin += Matriu[nVisits - 1][j].second;
	}
	CBBPriorityQueue.push(new CBBNode_Weight(visits.m_Vertices.front(), WeigthMin));

	list<CVertex*> path;
	CBBNode_Weight* CBBNODEActual;
	CVertex* LastVisited;
	CBBNode_Weight* newNode;
	//double WeigthMin;
	double bDist = DBL_MAX;

	while (!CBBPriorityQueue.empty()) {
		CBBNODEActual = CBBPriorityQueue.top();
		CBBPriorityQueue.pop();
		LastVisited = CBBNODEActual->m_Index.back();
		if (CBBNODEActual->m_Visited == visites2.GetNVertices() + 1) {
			// Restar les cotes d’arribar al vèrtex Vj 
			// Sumar la longitud del camí que va de Vi a Vj
			//longitud del camí més una estimació del que queda per recórrer menys 
				WeigthMin = CBBNODEActual->m_WeigthMin + Matriu[LastVisited->m_Index][nVisits - 1].second - Matriu[nVisits - 1][nVisits - 1].second;
				if (WeigthMin < bDist) {
					bDist = WeigthMin;
					path = CBBNODEActual->m_Index;
				}
			
		}

		else {
			for (CVertex* v2 : visites2.m_Vertices) {
				if (find(CBBNODEActual->m_Index.begin(), CBBNODEActual->m_Index.end(), v2) == CBBNODEActual->m_Index.end()) {
					//Afegeixo al camí [V1→...→Vi] el vèrtex a visitar Vj: Les seves cotes seran:
					//Cota superior[V1→...→Vi] - max(CVj) + longitud de Vi→Vj
					//Cota inferior[V1→...→Vi] - min(CVj) + longitud de Vi→Vj
					//el càlcul de les cotes no fa les sumes en el mateix ordre que el càlcul de 
					// la longitud dels camins
					
					WeigthMin = CBBNODEActual->m_WeigthMin + Matriu[LastVisited->m_Index][v2->m_Index].second - Matriu[nVisits - 1][v2->m_Index].second;
					if (WeigthMin < bDist) {
						newNode = new CBBNode_Weight(*CBBNODEActual);
						newNode->m_Visited++;
						newNode->m_WeigthMin = WeigthMin;
						newNode->m_Index.push_back(v2);
						CBBPriorityQueue.push(newNode);
					}
				}
			}
		}
		delete CBBNODEActual;
	}

	CVertex* anterior = visits.m_Vertices.front();
	path.pop_front();
	for (CVertex* v : path) {
		for (CEdge* e : Matriu[anterior->m_Index][v->m_Index].first) {
			track.m_Edges.push_back(e);
		}
		anterior = v;
	}
	for (CEdge* e : Matriu[path.back()->m_Index][nVisits - 1].first) {
		track.m_Edges.push_back(e);
	}
	
	return track;

	
}



// SalesmanTrackBranchAndBound3 ===================================================


class CBBNode1 {
public:
	list<CVertex*> m_Index;
	double m_WeigthMin1;
	int m_Visited;
public:
	CBBNode1(CBBNode1& Node)
	{
		m_Index = Node.m_Index;
		m_WeigthMin1 = Node.m_WeigthMin1;
		m_Visited = Node.m_Visited;
	}
	CBBNode1(CVertex* Visit, double WeigthMin1)
	{
		m_Index.push_back(Visit);
		m_WeigthMin1 = WeigthMin1;
		m_Visited = 1;
	}
};


// comparator ==================================================================

struct CBBNodeComparator_Weight1 {
	bool operator()(const CBBNode1* n1, const CBBNode1* n2) {
		return n1->m_WeigthMin1 > n2->m_WeigthMin1;
	}
};


CTrack SalesmanTrackBranchAndBound3(CGraph& graph, CVisits& visits)
{
	CVisits visites3 = visits;
	visites3.m_Vertices.pop_back();

	int nVisits = visits.GetNVertices();

	CVertex* actual;
	list<CEdge*> aux;
	pair<list<CEdge*>, double> Matriu[18][18];

	int posicioI = 0;
	int posicioJ = 0;

	//Creem la matriu
	for (CVertex* v : visites3.m_Vertices) {
		v->m_Index = posicioI;
		posicioJ = 0;
		DijkstraQueue(graph, v);
		for (CVertex* v2 : visits.m_Vertices) {
			aux.clear();
			actual = v2;
			while (actual != v) {
				for (CEdge& e : (&graph)->m_Edges) {
					if (e.m_pOrigin == actual->m_pDijkstraPrevious->m_pOrigin && e.m_pDestination == actual) {
						aux.push_front(&e);
						actual = actual->m_pDijkstraPrevious->m_pOrigin;
						break;
					}
				}
			}
			Matriu[posicioI][posicioJ] = make_pair(aux, v2->m_DijkstraDistance);
			if (((v2->m_DijkstraDistance > 0) && (v2->m_DijkstraDistance < Matriu[nVisits - 1][posicioJ].second)) || (Matriu[nVisits - 1][posicioJ].second == NULL)) {
				Matriu[nVisits - 1][posicioJ].second = v2->m_DijkstraDistance;
			}
			posicioJ++;
		}
		posicioI++;
	}

	CTrack track(&graph);

	visites3.m_Vertices.pop_front();
	if (visites3.m_Vertices.empty()) {
		track.m_Edges = Matriu[0][1].first;
		return track;
	}
	priority_queue<CBBNode1*, std::vector<CBBNode1*>, CBBNodeComparator_Weight1> CBBPriorityQueue;
	
	double WeigthMin1 = 0;
	//double WeigthMax = 0;
	for (int j = 1; j < nVisits; j++) {
		WeigthMin1 += Matriu[nVisits - 1][j].second;
	}
	CBBPriorityQueue.push(new CBBNode1(visits.m_Vertices.front(), WeigthMin1));

	list<CVertex*> path;
	CBBNode1* CBBNODEActual;
	CVertex* LastVisited;
	CBBNode1* newNode;
	double Dist_Total;
	double bDist = DBL_MAX;

	while (!CBBPriorityQueue.empty()) {
		CBBNODEActual = CBBPriorityQueue.top();
		CBBPriorityQueue.pop();
		LastVisited = CBBNODEActual->m_Index.back();

		if (CBBNODEActual->m_Visited == visites3.GetNVertices() + 1) {
			Dist_Total = CBBNODEActual->m_WeigthMin1 + Matriu[LastVisited->m_Index][nVisits - 1].second - Matriu[nVisits - 1][nVisits - 1].second;
			if (Dist_Total < bDist) {
				bDist = Dist_Total;
				path = CBBNODEActual->m_Index;
			}
		}

		else {
			for (CVertex* v2 : visites3.m_Vertices) {
				if (find(CBBNODEActual->m_Index.begin(), CBBNODEActual->m_Index.end(), v2) == CBBNODEActual->m_Index.end()) {
					Dist_Total = CBBNODEActual->m_WeigthMin1 + Matriu[LastVisited->m_Index][v2->m_Index].second - Matriu[nVisits - 1][v2->m_Index].second;
					if (Dist_Total <= bDist) {
						newNode = new CBBNode1(*CBBNODEActual);
						newNode->m_Visited++;
						newNode->m_WeigthMin1 = Dist_Total;
						newNode->m_Index.push_back(v2);
						CBBPriorityQueue.push(newNode);
					}
				}
			}
		}
		delete CBBNODEActual;
	}

	CVertex* anterior = visits.m_Vertices.front();
	path.pop_front();
	for (CVertex* v : path) {
		for (CEdge* e : Matriu[anterior->m_Index][v->m_Index].first) {
			track.m_Edges.push_back(e);
		}
		anterior = v;
	}
	for (CEdge* e : Matriu[path.back()->m_Index][nVisits - 1].first) {
		track.m_Edges.push_back(e);
	}
	return track;
}



//-----------------------------------------------------------------------------------------------------------------



struct Etiqueta {
	double amplada;
	double alcada;
};

struct Fulla {
	double amplada;
	double alcada;
	std::vector<Etiqueta> etiquetes;
	std::vector<Etiqueta> millorSolucio;
	double millorAmpladaUtilitzada;
};

struct NodeBnB {
	double ampladaActual;
	std::vector<Etiqueta> solucioParcial;
	double cotaInferior;  // Calcula aquí la teva cota inferior (heurística)
};

bool comparadorEtiqueta(const Etiqueta& a, const Etiqueta& b) {
	return a.amplada > b.amplada;  // Ordenar etiquetes per amplada de manera descendent
}

struct ComparadorNodeBnB {
	bool operator()(const NodeBnB& a, const NodeBnB& b) const {
		return a.cotaInferior > b.cotaInferior;
	}
};

void branchAndBound(const Fulla& fulla) {
	std::priority_queue<NodeBnB, std::vector<NodeBnB>, ComparadorNodeBnB> cuaPrioritat;

	NodeBnB nodeInicial = {0.0, {}, 0.0};  // Amplada inicial és 0
	nodeInicial.cotaInferior = calculaCotaInferior(nodeInicial, fulla);

	cuaPrioritat.push(nodeInicial);

	while (!cuaPrioritat.empty()) {
		NodeBnB nodeActual = cuaPrioritat.top();
		cuaPrioritat.pop();

		if (nodeActual.cotaInferior >= fulla.millorAmpladaUtilitzada) {
			// No continuar si ja hem superat la millor amplada utilitzada fins ara
			continue;
		}

		if (nodeActual.solucioParcial.size() == fulla.etiquetes.size()) {
			// Hem trobat una solució completa
			fulla.millorSolucio = nodeActual.solucioParcial;
			fulla.millorAmpladaUtilitzada = nodeActual.ampladaActual;
			continue;
		}

		// Iterar sobre les etiquetes disponibles i intentar-les col·locar
		for (const Etiqueta& etiqueta : fulla.etiquetes) {
			if (std::find(nodeActual.solucioParcial.begin(), nodeActual.solucioParcial.end(), etiqueta) ==
				nodeActual.solucioParcial.end()) {
				
				NodeBnB nouNode = nodeActual;
				nouNode.solucioParcial.push_back(etiqueta);

				// Ordenar etiquetes en funció de l'amplada
				std::sort(nouNode.solucioParcial.begin(), nouNode.solucioParcial.end(), comparadorEtiqueta);

				nouNode.ampladaActual = 0.0;
				double novaAlcada = 0.0;

				// Calcula l'amplada i alcada utilitzades amb les etiquetes actuals
				for (const Etiqueta& etiqueta : nouNode.solucioParcial) {
					nouNode.ampladaActual += etiqueta.amplada;
					novaAlcada = std::max(novaAlcada, etiqueta.alcada);
				}

				// Calcula la cota inferior del nou node
				nouNode.cotaInferior = calculaCotaInferior(nouNode, fulla);

				// Afegeix el nou node a la cua de prioritat
				cuaPrioritat.push(nouNode);
			}
		}
	}
}




//-------------------------------------------------------------------------------------------------------------------------------


// Estructura que representa un estat del taulell
struct Estat {
	std::vector<std::vector<int>> taulell;
	int cost;

	// Comparador per la cua de prioritat
	bool operator>(const Estat& other) const {
		return cost > other.cost;
	}
};

// Funció que calcula el cost d'un estat
int calcularCost(const std::vector<std::vector<int>>& taulell) {
	int cost = 0;
	int n = taulell.size();

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (taulell[i][j] != 0) {
				int valorCorrecte = i * n + j + 1;
				int filaCorrecta = (taulell[i][j] - 1) / n;
				int columnaCorrecta = (taulell[i][j] - 1) % n;

				cost += std::abs(i - filaCorrecta) + std::abs(j - columnaCorrecta);
			}
		}
	}

	return cost;
}

// Funció per moure les peces
std::vector<std::vector<int>> mourePeca(const std::vector<std::vector<int>>& taulell, int fila, int columna, int novaFila, int novaColumna) {
	std::vector<std::vector<int>> nouTaulell = taulell;
	std::swap(nouTaulell[fila][columna], nouTaulell[novaFila][novaColumna]);
	return nouTaulell;
}

// Funció que resol el Puzle utilitzant Branch and Bound
void resoldrePuzle(const std::vector<std::vector<int>>& estatInicial) {
	int n = estatInicial.size();

	// Inicialització de l'estat inicial
	Estat estat;
	estat.taulell = estatInicial;
	estat.cost = calcularCost(estat.taulell);

	// Cua de prioritat per explorar els nodes segons el seu cost
	std::priority_queue<Estat, std::vector<Estat>, std::greater<Estat>> cuaPrioritat;
	cuaPrioritat.push(estat);

	while (!cuaPrioritat.empty()) {
		// Treiem el node amb el menor cost de la cua
		Estat estatActual = cuaPrioritat.top();
		cuaPrioritat.pop();

		// Comprovem si hem arribat a la solució
		if (estatActual.cost == 0) {
			std::cout << "Solució trobada!" << std::endl;
			// Mostrar el taulell de la solució
			for (const auto& fila : estatActual.taulell) {
				for (int valor : fila) {
					std::cout << valor << " ";
				}
				std::cout << std::endl;
			}
			return;
		}

		// Trobar la posició del forat (0)
		int foratFila, foratColumna;
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				if (estatActual.taulell[i][j] == 0) {
					foratFila = i;
					foratColumna = j;
					break;
				}
			}
		}

		// Possibles moviments: amunt, avall, esquerra, dreta
		const int moviments[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

		// Explorar els possibles moviments
		for (const auto& moviment : moviments) {
			int novaFila = foratFila + moviment[0];
			int novaColumna = foratColumna + moviment[1];

			if (novaFila >= 0 && novaFila < n && novaColumna >= 0 && novaColumna < n) {
				Estat nouEstat;
				nouEstat.taulell = mourePeca(estatActual.taulell, foratFila, foratColumna, novaFila, novaColumna);
				nouEstat.cost = calcularCost(nouEstat.taulell);

				// Afegir el nou estat a la cua de prioritat
				cuaPrioritat.push(nouEstat);
			}
		}
	}

	std::cout << "No s'ha trobat cap solució." << std::endl;
}
