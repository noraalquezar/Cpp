#include "pch.h"
#include "Graph.h"
#include <limits>


// SalesmanTrackGreedy =========================================================

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits& visits)
{
	CVertex* v = visits.m_Vertices.front();
	CVertex* last = visits.m_Vertices.back();
	//Candidats= tots els vèrtexs de la llista de visites menys el primer i l'últim
	list<CVertex*> Candidats;
	Candidats = visits.m_Vertices;
	Candidats.remove(v);
	Candidats.remove(last);

	double min;
	CVertex* v1 = NULL;
	CVertex* vActual;

	CTrack cami_aux(&graph);
	CTrack track(&graph);

	while (!Candidats.empty()) {
		//Aplicant l’algorisme de Dijkstra calcular les distàncies de tots els vèrtexs del graf a la visita v.
		DijkstraQueue(graph, v);
		//Et quedes amb la visita que te la distancia mes curta
		//Et quedes amb la millor d'aquell moment esperant que l'optim local tambe sigui optim global. Anar sempre a la ciutat més propera
		min = INFINITY;
		for (CVertex* vertex : Candidats) {
			if (vertex->m_DijkstraDistance < min) {
				min = vertex->m_DijkstraDistance;
				v1 = vertex; // Seleccionar v1 com el vèrtex pertanyent a Candidats amb distància mínima.
			}
		}
		//Et guardes la nova visita que te el cami més curt fins el vertex actual 
		vActual = v1;
		//Es crea cami tirant enrrere de la nova visita (vactual) que hem trobat fins la visita anterior(v)
		//Podem encadenar els camins afegint un apuntador a aresta a cada vèrtex.Aquest l’actualitzem quan actualitzem la distància del vèrtex.
		while (vActual != v) {
			cami_aux.AddFirst(vActual->m_pDijkstraPrevious);
			vActual = vActual->m_pDijkstraPrevious->m_pOrigin;
		}
		//Afegir el cami auxiliar al cami final i netejar el cami auxiliar
		track.Append(cami_aux);
		cami_aux.Clear();
		Candidats.remove(v1);
		//Diem que la visita que teniem fins ara passa a ser v per despres quan trobem una altra visita calculem el cami fins a v
		v = v1;
	}

	//Fer el mateix pero per l'ultima visita per separat
	DijkstraQueue(graph, v);
	while (last != v) {
		cami_aux.AddFirst(last->m_pDijkstraPrevious);
		last = last->m_pDijkstraPrevious->m_pOrigin;
	}
	track.Append(cami_aux);
	return track;
}