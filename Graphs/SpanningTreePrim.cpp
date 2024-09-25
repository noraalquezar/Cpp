#include "pch.h"
#include "Graph.h"
#include <vector>
#include <queue>
using namespace std;

// =============================================================================
// SpanningTreePrim ============================================================
// =============================================================================



CSpanningTree SpanningTreePrim(CGraph& graph) {
    // Comparador per ordenar les arestes retorna true si pE1 > pE2
    
    struct PrimComparator {
        bool operator()(CEdge* pE1, CEdge* pE2) {
            return pE1->m_Length > pE2->m_Length;
        }
    };

    //Comprova que hi hagi arestes al graph

    if (graph.m_Edges.empty()) {
        return 0;
    }

    // Crea la cua de prioritat 
    priority_queue<CEdge*, vector<CEdge*>, PrimComparator> queue;

    // Crea l'arbre 
    CSpanningTree tree(&graph);

    // Marquem tots els vèrtex com si no perteneixessin al graph
    for (CVertex& v : graph.m_Vertices) {
        v.m_PrimInTree = false;
    }

    // Seleccionem un vertex d'inici (el primer vertex del grafo)
    CVertex* startVertex = &graph.m_Vertices.front();

    //Diem que si que pertany al graph
    startVertex->m_PrimInTree = true;

    //Afegim a la cua les arestes que estan connectades amb el vertex d'inici
    for (CEdge* pE : startVertex->m_Edges) {
        queue.push(pE);
    }

   
    while (!queue.empty()) {
       
        CEdge* pE = queue.top(); //Obte l'element de la part superior 
        queue.pop(); //Ho elimina de la cua

        // Si el vertex ja es troba al arbre, obviem la aresta
        if (pE->m_pDestination->m_PrimInTree) {
            continue;
        }

        //Afegim la aresta al arbre d'expancio minima 
        tree.m_Edges.push_back(pE);

        //Marquem el vertex de desti com si pertany al arbre 
        pE->m_pDestination->m_PrimInTree = true;

        //Afegim les arestes connectades al vertex de desti a la cua de prioritat
        for (CEdge* pAdjE : pE->m_pDestination->m_Edges) {
            if (!pAdjE->m_pDestination->m_PrimInTree) {
                queue.push(pAdjE);
            }
        }
    }
    //Retornem l'arbre d'expansió mínima
    return tree;
    
}

/*
* CConvexHull ConvexHull(CVertex* A, CVertex* B, list<CVertex*> Punts) {
    CConvexHull CHLeft = NULL;
    CConvexHull CHRight = NULL;

    list<CVertex*> Left;
    list<CVertex*> Right;

    CConvexHull Hull = NULL;
    Hull.m_Vertices.push_back(A);
    Hull.m_Vertices.push_back(B);

    // Si Punts está vacío, retornar la lista [A,B]
    if (Punts.empty()) {
        return Hull;
    }

    // Trobar el punt C de Punts que maximice AreaTriangle(A,C,B)
    double maxArea = 0;
    CVertex* C = nullptr;

    for (CVertex* v : Punts) {
        double currentArea = AreaTriangle(A->m_Point, B->m_Point, v->m_Point);
        if (currentArea > maxArea) {
            maxArea = currentArea;
            C = v;
        }
    }

    // Separar punts en Left y Right
    for (CVertex* v : Punts) {
        if (PosicioRespeteRecta(A->m_Point, C->m_Point, v->m_Point) < 0) {
            Left.push_back(v);
        }
        else if (PosicioRespeteRecta(B->m_Point, C->m_Point, v->m_Point) > 0) {
            Right.push_back(v);
        }
    }

    // Calcular las partes izquierda y derecha de la envoltura convexa
    CHLeft = ConvexHull(A, C, Left);
    CHRight = ConvexHull(C, B, Right);

    // Añadir los vértices de CHRight a CHLeft
    for (auto vertex : CHRight.m_Vertices) {
        CHLeft.m_Vertices.push_back(vertex);
    }

    // Eliminar C de la lista CHLeft
    CHLeft.m_Vertices.remove(C);

    return CHLeft;
}
* 
* 
* 



*/


/*

CConvexHull QuickHull(CGraph& graph) {
    list<CVertex*> Up;
    list<CVertex*> Down;
    CConvexHull Hull = NULL;
    CVertex* A = nullptr;
    CVertex* B = nullptr;

    if (graph.m_Vertices.empty()) return Hull;

    // Buscar dos puntos extremos A y B
    int x_min = INT_MAX;
    int x_max = INT_MIN;

    for (auto& v : graph.m_Vertices) {
        if (v.m_Point.m_X < x_min) {
            x_min = v.m_Point.m_X;
            A = &v;
        }
        if (v.m_Point.m_X > x_max) {
            x_max = v.m_Point.m_X;
            B = &v;
        }
    }

    if (A->m_Point.m_Y == B->m_Point.m_Y && A->m_Point.m_X == B->m_Point.m_X) {
        Hull.m_Vertices.push_back(A);
        return Hull;
    }
    
    int dist = 0;

    for (CEdge& e : graph.m_Edges) {
        if (e.m_Length > dist) {
            dist = e.m_Length;
            A = e.m_pOrigin;
            B = e.m_pDestination;
        }
    }
    

    //Separar el conjunt punts en els quals estan a sobre i els que 
    // estan a sota de la recta AB

for (CVertex& v : graph.m_Vertices) {
    if (PosicioRespeteRecta(A->m_Point, B->m_Point, v.m_Point) < 0) {
        Up.push_back(&v);
    }
    else if (PosicioRespeteRecta(A->m_Point, B->m_Point, v.m_Point) > 0) {
        Down.push_back(&v);
    }
}


//Calculem les parts superior i inferior del envoltant conexa. 
CConvexHull CHUp = ConvexHull(A, B, Up);
CConvexHull CHDown = ConvexHull(B, A, Down);

//Eliminem els punts A i B de la llista Punts per indicar que han estat visitats 
// i ajuntem CHLeft i CHRight concatenant les llistes de punts.
CHUp.m_Vertices.remove(A);
CHUp.m_Vertices.remove(B);
CHUp.m_Vertices.splice(CHUp.m_Vertices.end(), CHDown.m_Vertices);

return CHUp;
}



*/