#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <random>
#include <chrono>
#include "matrix.h"

// SalesmanTrackProbabilistic ==================================================
// Extras
#include <vector>
#include <numeric>

vector<int> MinPathProbabilistic(int n, vector<double> costs, double* cost_gradient_total)
{
	//Aquesta funció genera un vector aleatori i hi realitza el descens del gradient
	double cost_grad; int indx, i, j;
	vector <int> vector_o(n - 2), vector_d(n - 1), vector_gradient(n);
	*cost_gradient_total = 0;
	bool sortir;

	//Omplim el vector_o i la 1a posicio del vector_d
	iota(vector_o.begin(), vector_o.end(), 1);
	vector_d.push_back(0);

	//Creem un vector randomitzat (vector_d)
	for (i = 0; i < n - 2; i++){
		//tria un numero random en el rang que no es te en compte inici i final i tots aquells que ja s'han triat abans
		indx = rand() % (n - i - 2);
		vector_d[i + 1] = vector_o[indx];
		vector_o[indx] = vector_o[n - i - 3];
		*cost_gradient_total += costs[n * vector_d[i] + vector_d[i + 1]];
	}
	vector_d[n - 1] = n - 1;
	*cost_gradient_total += costs[n * vector_d[n - 1] + vector_d[n - 2]];
	i = 0, j = 0;
	while (i < n - 2 && j < n - 1)
	{
		sortir = false;
		//No aplicar a la primera ni la ultima visita
		for (i = 1; i < n - 2; ++i)
		{
			//A partir de la visita del bucle anterior
			for (j = i + 1; j < n - 1; ++j)
			{
				//Generem una nova configuració girant les visites i <-> j
				//Realitza iterativament intercanvis entre els parells de visites per millorar cost.
				vector_gradient = vector_d;
				indx = vector_gradient[i];
				vector_gradient[i] = vector_gradient[j];
				vector_gradient[j] = indx;
				//Recalculem els costs segons el nou ordre de visites de vector_gradient
				cost_grad = 0;
				for (int k = 1; k < n; k++)
					cost_grad += costs[n * vector_gradient[k] + vector_gradient[k - 1]];

				if (cost_grad < *cost_gradient_total)
				{
					*cost_gradient_total = cost_grad;
					vector_d = vector_gradient;
					sortir = true;
					break;
				}
			}
			if (sortir) break;
		}
	}
	return vector_d;
}


CTrack SalesmanTrackProbabilistic(CGraph& graph, CVisits& visits)
{
	// inicialització de variables
	int n_visites = visits.GetNVertices(), n_intents = 5000 * n_visites;
	if (n_visites > 23) n_intents = 19000 * n_visites;
	double cost_gradient = 0, min_cost_gradient = 0;
	vector <double> costs;
	vector <CVertex*> llista_visites;
	vector<int> vector_gradient, min_cost_vector;


	// Creem la matriu de costs original
	for (CVertex* vo : visits.m_Vertices)
	{
		llista_visites.push_back(vo);
		Dijkstra(graph, vo);
		for (CVertex* vd : visits.m_Vertices)
		{
			if (vo != vd) costs.push_back(vd->m_DijkstraDistance);
			else costs.push_back(0); // cas en el qual no cal visitar
		}
	}
	// generem un primer vector fent un primer descens del gradient
	min_cost_vector = MinPathProbabilistic(n_visites, costs, &min_cost_gradient);

	// anem realitzant el descens del gradient des de diversos punts de la funció
	// i ens quedem amb el vector que genera el camí més curt
	//El proces es repeteix fins que no es pugui trobar més intercanvis que millorin el cost 
	for (int i = 0; i < n_intents; i++)
	{
		vector_gradient = MinPathProbabilistic(n_visites, costs, &cost_gradient);
		if (cost_gradient < min_cost_gradient) {
			min_cost_gradient = cost_gradient;
			min_cost_vector = vector_gradient;
		}
	}
	//Reconstruim el recorregut a partir de les visites
	CVisits visites_aux = NULL; 
	CTrack cami_complert(&graph);

	visites_aux.m_Vertices.push_back(llista_visites[min_cost_vector[0]]);

	for (int i = 1; i < n_visites; i++)
	{
		//Un cop tens l'ordre de les visites crides a greedy perque et crei el camí
		visites_aux.m_Vertices.push_back(llista_visites[min_cost_vector[i]]);
		cami_complert.Append(SalesmanTrackGreedy(graph, visites_aux));
		visites_aux.m_Vertices.pop_front();
	}
	return cami_complert;
}
