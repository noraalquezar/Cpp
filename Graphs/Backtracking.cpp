#include "pch.h"
#include "Graph.h"
#include <set>
#include <stack>


// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================
/*
Recorrer en profunditat prioritaria (pas endavant, pas endarrere)
Cada crida a la funció correspon amb un node o fulla de l’arbre
Ramificacions: crides recursives
Com estalviar cerques branques de l’arbre de cerca:
– El camí no pot passar dues vegades pel mateix vèrtexs.
– Quan el camí que estem construint és més llarg que el millor trobat fins al moment no té
sentit continuar.
Es pot passar per un vertex més d'una vegada pero si es per camins de vertex diferents.  
*/

//Declaracions
CTrack Cami(NULL);
stack<CEdge*> PilaCami;
stack<CEdge*> PilaAux;
double LongitudCami;
double LongitudAux;
unsigned visites = 0;

void TrobarCami(stack<CEdge*>* PilaAux, double LongitudAux, CVertex* v, CVisits& visits, CVertex* last) {
	//Si has calulat un cami que es mes curt que el que tenies treure aresta
	//PODA
	if (LongitudAux < LongitudCami) {
		if (v == last && visites == visits.GetNVertices()) {
			//Guardes el cami i longitud total
			LongitudCami = LongitudAux;
			PilaCami = *PilaAux;
		}

		else {
			//Guardes a una nova variable el valor inicial perque es pot modificar si es una visita nova 
			// i si al final no arriba a una solucio optima s'haura de fer pas enrrere i recuperar el valor incial. 
			unsigned visites_anteriors = v->m_VisitatsAnteriorment;

			//Si el vertex es una visita i no s'havia visitat abans
			if (v->m_VertexToVisit == true && visites_anteriors == 0) visites++; 
			//ModifIquem el valor i aixi podem tornar a passar per tots els vèrtexs 
			v->m_VisitatsAnteriorment = visites;

			//Per cada vei
			for (CEdge* e : v->m_Edges) {
				//PROFUNDITAT PRIORITARIA
				//Si el vertex desti de l'aresta no ha passat per aquell tram afegirlo a la pila
				//Es pot passar per un vertex més d'una vegada pero si es per camins de vertex diferents. 
				//PODA
				if (e->m_pDestination->m_VisitatsAnteriorment != visites) {
					PilaAux->push(e);
					//Passar per parametres la longitud que es tenia + la longitud de l'aresta i el vertex desti de l'aresta
					TrobarCami(PilaAux, LongitudAux + e->m_Length, e->m_pDestination, visits, last);
					//Salta quan la longitud era mes gran que la que tenies o quan s'ha trobat un cami
					//Quan s'a trobat el cami optim, surts de totes les funciones recursives eliminant cada element de la pila auxiliar 
					//Perque el cami ja el tens guardat a PilaCami
					PilaAux->pop();
				}
			}
			//Si s'han explorat totes les soluciones d'aquell vertex i cap arriba a una solucio optima (no te la solucio total o el cami es mes llarg que el que tenies
			//tirar enrrere
			v->m_VisitatsAnteriorment = visites_anteriors;
			//si asobre estaves a una visita, tirar enrrere les visites. 
			if (v->m_VertexToVisit ==true && visites_anteriors == 0) visites--;
		}
	}
}
/*
* 
* El backtracking implica explorar sistemáticamente todas las posibles 
combinaciones de soluciones, y cuando se llega a un punto en el que se 
verifica que un camino explorado no conduce a la solución deseada, se realiza un 
"backtrack" (retroceso) eliminando elementos de la pila y volviendo a un estado 
anterior para explorar otras opciones.

*/

CTrack SalesmanTrackBacktracking(CGraph& graph, CVisits& visits)
{
	CVertex* last = visits.m_Vertices.back();
	CVertex* v = visits.m_Vertices.front();

	//Elimina ultim vertex de visites
	visits.m_Vertices.pop_back();
	//Marcar que tots els vertex no s'han visitat 
	for (CVertex& v : graph.m_Vertices) {
		v.m_VisitatsAnteriorment = 0;
		v.m_VertexToVisit = false;
	}

	LongitudCami = numeric_limits<double>::max();
	LongitudAux = 0.0;
	//Marcar les visites com a true, ja que s'han de visitar
	for (CVertex* v : visits.m_Vertices) v->m_VertexToVisit = true;
	//Marcar la primera com que no s'ha de visitar i ja s'ha visitat un cop
	v->m_VertexToVisit = false;
	v->m_VisitatsAnteriorment = 1;
	visites = 1;

	//Condicio inicial
	TrobarCami(&PilaAux, 0, v, visits, last);

	//Quan ja es te el cami optim afegeixes a cami la primera aresta de la pila i  l'elimines perque es puguin agafar les altres
	while (!PilaCami.empty()) {
		Cami.m_Edges.push_front(PilaCami.top());
		PilaCami.pop();
	}
	//Afegir a l'ultim de la pila la ultima visita
	visits.m_Vertices.push_back(last);
	return Cami;

}


// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================

//S'utilitza Dijkstra per trobar els camins entre visites i backtracking
//per decidir l’ordre de les visites.
//Convertir els vèrtexs a visitar en índexs d’aquest array

struct structTrack {
	CTrack track;
	double length;
};
vector<vector<structTrack>> MatriuCamins;
vector<int> ordreVisits;
vector<bool> shaVisitat;

void recursivaGreedy(int index, CVisits& visits)
{
	//Marcar que el vertex s'ha visitat. 
	ordreVisits.push_back(index);
	shaVisitat[index] = true;
	visites++;

	int iter;
	//Si s'ha arribat al total de visites
	if (visites == visits.m_Vertices.size() - 1) {
		//Si amb aquest increment es mes curt el cami, afegirlo al cami
		LongitudAux += MatriuCamins[index][visits.m_Vertices.size() - 1].length;
		if (LongitudAux < LongitudCami) {
			ordreVisits.push_back(visits.m_Vertices.size() - 1);
			//Crear el camí
			Cami.Clear();
			//Convertir els vertex a visitar a index de l'array
			for (iter = 0; iter < visits.m_Vertices.size() - 1; iter++) {
				Cami.Append(MatriuCamins[ordreVisits[iter]][ordreVisits[iter + 1]].track);
			}
			LongitudCami = LongitudAux;
			ordreVisits.pop_back();
		}
		//Tallar la cerca quan el camí que estem generant és més llarg que el camí més curt que
		//hem trobat fins al moment.
		LongitudAux -= MatriuCamins[index][visits.m_Vertices.size() - 1].length;
	}
	//PODA
	//Si la longnitud actual es millor que la que teniem:
	else if (LongitudAux < LongitudCami) {
		for (iter = 1; iter < visits.m_Vertices.size() - 1; iter++) {
			//Si la visita encara no ha sigut visitada
			if (shaVisitat[iter]) goto next_G;
			//Mires si el cami que poses es mes curt que el que tenies, si es aixi et 
			// quedes amb el cami amb la suma. (cami de la visita que ha passat per paramentre amb qualevol de les altres)
			LongitudAux += MatriuCamins[index][iter].length;
			recursivaGreedy(iter, visits);
			//Si no es aixi no el poses
			LongitudAux -= MatriuCamins[index][iter].length;
		next_G:;
		}
	}

	//Pas enrrere
	ordreVisits.pop_back();
	shaVisitat[index] = false;
	visites--;

}

CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{
	vector<CVertex*> vectorVisits; //Crees un vector on afegeixes les visites.
	for (CVertex* v : visits.m_Vertices) vectorVisits.push_back(v);
	structTrack Aux{ NULL };
	CTrack AuxTrack(&graph);
	CVertex* v1;
	vector<structTrack> camins;
	//Guardar en un array de dues dimensions tots els camins òptims entre visita i visita.
	for (int i = 0; i < visits.m_Vertices.size(); i++) {
		DijkstraQueue(graph, vectorVisits[i]);
		for (int j = 0; j < visits.m_Vertices.size(); j++) {
			v1 = vectorVisits[j];
			//Mentres que la visita del bucle interior sigui diferent a la visita del bucle exterior
			while (v1 != vectorVisits[i]) {
				//Afegeixes al cami tirant enrrere
				AuxTrack.AddFirst(v1->m_pDijkstraPrevious);
				v1 = v1->m_pDijkstraPrevious->m_pOrigin;
			}
			//Afegeixes el cami i l'elimines 
			Aux.track = AuxTrack;
			AuxTrack.Clear();
			//L'allargada del cami es 
			Aux.length = vectorVisits[j]->m_DijkstraDistance;
			camins.push_back(Aux);
		}
		//Et crea tots els camins optims entre dos vertex, pero hem de decidir en quin ordre visitarlos per aconseguir la combinacio que ens doni la combinacio optima 
		//Guardar en un array de dues dimensions tots els camins òptims de les visites i eliminar camins
		MatriuCamins.push_back(camins);
		camins.clear();
	}
	Cami.Clear();
	Cami.SetGraph(&graph);
	LongitudCami = numeric_limits<double>::max();
	LongitudAux = 0.0;
	for (int i = 0; i < visits.m_Vertices.size(); i++) shaVisitat.push_back(false);
	visites = 0;
	//Cridem la funció recursiva per primera vegada
	recursivaGreedy(0, visits);
	return Cami;
}