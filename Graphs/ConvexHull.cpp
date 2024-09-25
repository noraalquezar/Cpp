#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <set>
#include <vector>


// =============================================================================
// CONVEX HULL =================================================================
// =============================================================================

double PosicioRespeteRecta(CGPoint& a, CGPoint& b, CGPoint& c) {
	return (a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y);
}

double AreaTriangle(CGPoint& a, CGPoint& b, CGPoint c) {
	return abs((a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y)) / 2.0;
}
CConvexHull ConvexHull(CVertex* A, CVertex* B, list<CVertex*> Punts) {
	//Declarem un ConvexHull i li afegim els vèrtexs A i B. Si Punts és buit, retornem la llista [A,B].
	CConvexHull CH = NULL;
	CH.m_Vertices.push_back(A);
	CH.m_Vertices.push_back(B);
	if (Punts.empty()) return CH;

	//Trobem el punt C de Punts que formi el triangle d'àrea màxima amn els vèrtexs A i B. Utilitzem la funció AreaTriangle(A,C,B) per a calcular l'àrea.
	CVertex* C = NULL; //Treballem amb una llista d'apuntadors à vèrtexs
	double area = 0;
	for (CVertex* vertex : Punts) {
		if (AreaTriangle(A->m_Point, B->m_Point, vertex->m_Point) > area) {
			area = AreaTriangle(A->m_Point, B->m_Point, vertex->m_Point);
			C = vertex;
		}
	}

	//Separem els punts en:
	//LEFT: ESQUERRA AC
	//RIGHT: DRETA BC --> canviat
	list<CVertex*> Left, Right;
	for (CVertex* vertex : Punts) {
		if (PosicioRespeteRecta(A->m_Point, C->m_Point, vertex->m_Point) < 0) Left.push_back(vertex);
		else if (PosicioRespeteRecta(B->m_Point, C->m_Point, vertex->m_Point) > 0) Right.push_back(vertex);
	}

	//Calculem les parts esquerra i dreta de la envoltant conexa, que seran dos ConvexHulls. Per fer-ho, fem que la funció es cridi a si mateixa de forma recursiva.
	CConvexHull CHLeft = ConvexHull(A, C, Left);
	CConvexHull CHRight = ConvexHull(C, B, Right);

	//Eliminem el punt C de la llist Punts per indicar que ha estat visitat i ajuntem CHLeft i CHRight concatenant les llistes de punts. (Evitem repetir el punt C!!)
	CHLeft.m_Vertices.remove(C);
	CHLeft.m_Vertices.splice(CHLeft.m_Vertices.end(), CHRight.m_Vertices);

	return CHLeft;
}

// QuickHull ===================================================================

CConvexHull QuickHull(CGraph& graph)
{
	//Inicialitzem un ConvexHull i els vèrtexs A i B, que seran els vèrtexs extrems: aquell situat més a l'esquerra i aquell situat més a la dreta.
	CConvexHull CH = NULL;
	CVertex* A = NULL;
	CVertex* B = NULL;

	//Per començar, si el graf és buit, retornem un ConvexHull buit.
	if (graph.m_Vertices.empty())return CH;

	//Trobem A i B buscant el punt amb el menor m_X i el punt amb el major m_X.
	double min = INFINITY;
	double max = -INFINITY;


	for (CVertex& vertex : graph.m_Vertices) {
		if (vertex.m_Point.m_X < min) {
			min = vertex.m_Point.m_X;
			A = &vertex;
		}
		if (vertex.m_Point.m_X >= max) {
			max = vertex.m_Point.m_X;
			B = &vertex;
		}
	}
	

	//Tenim en compte que, si A i B coincideixen, vol dir que només hi ha un únic punt. L'afegim a la llista de vèrtexs del CH i retornem CH.
	if (A->m_Point.m_Y == B->m_Point.m_Y) {
		if (A->m_Point.m_X == B->m_Point.m_X) {
			CH.m_Vertices.push_back(A);
			return CH;
		}
	}

   //Separem els punts en:
   //UP: SOBRE AB
   //DOWN: SOTA AB
	list<CVertex*> Up, Down;
	for (CVertex& v : graph.m_Vertices) {
		if (PosicioRespeteRecta(A->m_Point, B->m_Point, v.m_Point) < 0) Up.push_back(&v);
		else if (PosicioRespeteRecta(A->m_Point, B->m_Point, v.m_Point) > 0) Down.push_back(&v);
	}

	//Calculem les parts superior i inferior de la envoltant convexa, que seran dos ConvexHulls. Per fer-ho, utilitzem la funció ConvexHull.
	CConvexHull CHUp = ConvexHull(A, B, Up);
	CConvexHull CHDown = ConvexHull(B, A, Down);

	//Eliminem els punts A i B de la llista Punts per indicar que han estat visitats i ajuntem CHLeft i CHRight concatenant les llistes de punts. (Evitem repetir els punts A i B!!)
	CHUp.m_Vertices.remove(A);
	CHUp.m_Vertices.remove(B);
	CHUp.m_Vertices.splice(CHUp.m_Vertices.end(), CHDown.m_Vertices);

	return CHUp;
}