#include "pch.h"
#include "Graph.h"
#include <vector>
#include <queue>
#include <limits> 

// =============================================================================
// Dijkstra ====================================================================
// =============================================================================


/*
Complexitat: O(V^2+A)=O(V^2)
*/
void Dijkstra(CGraph& graph, CVertex* pStart)
{
	//Inicialitzem les dist�ncies de tots els v�rtexs a infinit
	//Marquem tots els v�rtexs com no visitats
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = std::numeric_limits<double>::max();
		v.m_DijkstraVisit = false;
	}
	//Inicialitzem pActual, que inicialment �s pStart, i indiquem que la seva dist�ncia �s 0
	CVertex* pActual = pStart;
	pActual->m_DijkstraDistance = 0;
	double min;
	//Mentre hi havi vertex no visitats
	while (pActual != NULL) {
		//Recorre tots els ve�ns v de pActual i fer el seg�ent
		for (CEdge* e : pActual->m_Edges) {
			//Si la distancia que te el vertex desti que conecta amb el vertex actual es mes gran que la suma 
			//De la distancia del vertex actual mes la aresta canviar la distancia del vertex desti
			if (e->m_pDestination->m_DijkstraDistance > (pActual->m_DijkstraDistance + e->m_Length)) {
				e->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + e->m_Length;
			}
		}
		//Marcar com visitat 
		pActual->m_DijkstraVisit = true;
		pActual = NULL;
		min = INFINITE;
		//Si un vertex que encara no ha estat visitat te una distancia mes petita que la que tenies,
		//Et quedes amb la distancia i dius que el vertex actual es el de la distancia mes petita. 
		for (CVertex& v : graph.m_Vertices) {
			if (v.m_DijkstraVisit == false && v.m_DijkstraDistance < min) {
				min = v.m_DijkstraDistance;
				pActual = &v;
			}
		}
	}
}



// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================

/*
* No necessitem marcar com a visitat perqu� aix� ja ho fa la cua, quan treus 
es que ja ha etsta visitat, i quan poses es perqu� s'ha de visitar. 
*/

void DijkstraQueue(CGraph& graph, CVertex* pStart)
{
	// Comparator per a la cua de prioritat
	struct DijkstraComparator {
		bool operator()(CVertex* pV1, CVertex* pV2) {
			return pV1->m_DijkstraDistance > pV2->m_DijkstraDistance;
		}
	};
	// Inicialitzar les dist�ncies dels v�rtexs
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = std::numeric_limits<double>::max();
	}
	pStart->m_DijkstraDistance = 0;
	// Cua de prioritat per gestionar els v�rtexs
	std::priority_queue<CVertex*, std::vector<CVertex*>, DijkstraComparator> queue;

	CVertex* va = pStart;
	// Posar a la cua el v�rtex start
	queue.push(va);

	while (!queue.empty()) {
		// Treure el primer element de la cua
		va = queue.top();
		// Rec�rrer tots els ve�ns del v actual
		for (CEdge* pAdjE : va->m_Edges) {
			CVertex* pAdjV = pAdjE->m_pDestination;
			// Si la dist�ncia de v �s m�s gran que la dist�ncia del v�rtex actual m�s la longitud de l�aresta que els uneix,
			// actualitzar la dist�ncia de v i afegir-lo a la cua.
			if (pAdjV->m_DijkstraDistance > va->m_DijkstraDistance + pAdjE->m_Length) {
				pAdjV->m_DijkstraDistance = va->m_DijkstraDistance + pAdjE->m_Length;
				queue.push(pAdjV);
				pAdjE->m_pDestination->m_pDijkstraPrevious = pAdjE;
			}
		}
		queue.pop();// Marcar va com visitat
	}
}
