#include "../include/GeneralProblems.h"
#include "../include/SP.h"
#include "../include/lista1.h"
#include <vector>
#include <deque>
#include <set>
#include <stack>
#include <algorithm>

using namespace std;

//PRIVATE METHDOS
static int findTriangle(vec2 v, const vec2* points, const vector<int> & triangles){
	int i;
	for (i = 0; i < triangles.size() / 3; i++){
		vector<int>::const_iterator start = triangles.begin() + i * 3;
		TriangleLocalization loc = findInTriangle(v, points[start[0]], points[start[1]], points[start[2]]);
		if (loc == INSIDE)
			return i;
	}
	return -1;
}

static void pushPath(int src, int dest, const int *parent, vector<int> & out){
	if (src != dest)
		pushPath(src, parent[dest], parent, out);
	out.push_back(dest);
}

static Diagonal getSharedVertices(const int* triA, const int* triB){
	int resp[2];
	int index = 0;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (triA[i] == triB[j]){
				resp[index++] = triA[i];
			}
		}
	}
	return Diagonal(resp[0], resp[1]);
}

static int getDifferentVertex(Diagonal d, const int* triangle){
	for (int i = 0; i < 3; i++)
	if (triangle[i] != d.a && triangle[i] != d.b)
		return triangle[i];
	return -1;
}

static bool isPredecessor(vec2 candidate, vec2 next, vec2 v, int expectedSign){
	vec2 delta = v - next, baseSegment = candidate - next;
	int cross = baseSegment.crossSign(delta);
	return cross == expectedSign || cross == 0;
}

static bool triangleContainsVertex(const int *triangle, int vertex){
	return std::find(triangle, triangle + 3, vertex) != triangle + 3;
}

static deque<int> makeFirstFunnel(vec2 src, const vec2* points, int indexA, int indexB){
	vec2 diffA = points[indexA] - src;
	vec2 diffB = points[indexB] - src;
	deque<int> resp;
	if (diffA.crossSign(diffB) >= 0){
		resp.push_back(indexB);
		resp.push_back(-1);
		resp.push_back(indexA);
	}
	else {
		resp.push_back(indexA);
		resp.push_back(-1);
		resp.push_back(indexB);
	}
	return resp;
}

static vec2 getFunnelPoint(vec2 src, const vec2* points, const deque<int> & funnel, int index){
	if (funnel[index] == -1)
		return src;
	return points[funnel[index]];
}

static bool predecessorIsApex(const vec2* points, const deque<int> & funnel, int apex, vec2 src, vec2 v){
	if (funnel.size() < 3)
		return true;
	deque<int>::const_iterator apexIt = find(funnel.begin(), funnel.end(), apex);
	vec2 vApex = apex == -1 ? src : points[*apexIt];
	vec2 vNext = getFunnelPoint(src, points, funnel, (apexIt - funnel.begin()) + 1);
	vec2 vPrev = getFunnelPoint(src, points, funnel, (apexIt - funnel.begin()) - 1);


	vec2 v1 = vNext - vApex,
		v2 = vPrev - vApex,
		delta = v - vApex;
	bool resp = v1.crossSign(delta) == 1 && v2.crossSign(delta) == -1;
	return resp;
}

static int searchLeftForHead(vec2 src, vec2 target, int apexIndex, const vec2* points, const deque<int> & funnel){
	for (int i = 0; i < apexIndex; i++){
		vec2 a = getFunnelPoint(src, points, funnel, i);
		vec2 b = getFunnelPoint(src, points, funnel, i + 1);
		if (isPredecessor(a, b, target, 1))
			return i;
	}
	return -1;
}

static int searchRightForHead(vec2 src, vec2 target, int apexIndex, const vec2* points, const deque<int> & funnel){
	for (int i = funnel.size() - 1; i > apexIndex; i--){
		vec2 a = getFunnelPoint(src, points, funnel, i);
		vec2 b = getFunnelPoint(src, points, funnel, i - 1);
		if (isPredecessor(a, b, target, -1))
			return i;
	}
	return -1;
}

static int findPredecessor(deque<int> & funnel, int apex, const vec2* points, vec2 src, vec2 dest, int targetIndex, bool beginsOnLeft){

	vec2 target = targetIndex == -2 ? dest : points[targetIndex];
	int apexIndex = find(funnel.begin(), funnel.end(), apex) - funnel.begin();
	if (predecessorIsApex(points, funnel, apex, src, target))
		return apexIndex;

	int pos;
	if (beginsOnLeft){
		pos = searchLeftForHead(src, target, apexIndex, points, funnel);
		if (pos == -1)
			pos = searchRightForHead(src, target, apexIndex, points, funnel);
	}
	else {
		pos = searchRightForHead(src, target, apexIndex, points, funnel);
		if (pos == -1)
			pos = searchLeftForHead(src, target, apexIndex, points, funnel);
	}

	if (pos == -1)
		fprintf(stderr, "No predecessor for %d found!\n", targetIndex);
	return pos;
}

//PUBLIC METHODS
Diagonal::Diagonal(int first, int second) : a(first), b(second){}

vector<int> DepthFirstSearch(vec2 src, vec2 target, const vec2* points, const Graph & g, const vector<int> & triangles){
	//find first triangle
	vector<int> resp;
	int srcTriangle = findTriangle(src, points, triangles);
	int destTriangle = findTriangle(target, points, triangles);

	if (srcTriangle != -1 && destTriangle != -1){
		if (destTriangle == srcTriangle)
			resp.push_back(srcTriangle);
		else{
			set<int> visited;
			stack<int> toVisit;
			toVisit.push(srcTriangle);

			size_t triCount = triangles.size() / 3;
			int *parent = new int[triCount];
			std::fill_n(parent, triCount, -1);
			int t;

			while (toVisit.size() > 0 && (t = toVisit.top()) != destTriangle){
				toVisit.pop();

				for (int dest : g[t])
				if (visited.find(dest) == visited.end()){
					parent[dest] = t;
					toVisit.push(dest);
				}

				visited.insert(t);
			}

			pushPath(srcTriangle, destTriangle, parent, resp);
			delete parent;
		}
	}

	return resp;
}

Graph getDualGraph(const vector<int> & triangulation){
	Graph resp(triangulation.size() / 3);
	if (triangulation.size() > 0){
		int out[3];
		for (int i = 0; i < triangulation.size() - 3; i += 3){
			int a[3] = { triangulation[i], triangulation[i + 1], triangulation[i + 2] };
			sort(a, a + 3);
			for (int j = i + 3; j < triangulation.size(); j += 3){
				int b[3] = { triangulation[j], triangulation[j + 1], triangulation[j + 2] };
				sort(b, b + 3);
				if (set_intersection(a, a + 3, b, b + 3, out) - out >= 2){
					resp[i / 3].insert(j / 3);
					resp[j / 3].insert(i / 3);
				}
			}
		}
	}

	return resp;
}

vector<int> SP(vec2 src, vec2 dest, const vec2* points, int count, deque<deque<int>> & funnels){
	vector<int> triangles = earClippingTriangulate(points, count);
	Graph g = getDualGraph(triangles);
	vector<int> tree = DepthFirstSearch(src, dest, points, g, triangles);

	vector<int> resp;

	if (tree.size() > 1){

		int* preds = new int[count];
		std::fill_n(preds, count, -1);
		int nextVertex = -1;

		Diagonal d = getSharedVertices(triangles.data() + tree[0] * 3, triangles.data() + tree[1] * 3);
		preds[d.a] = preds[d.b] = -1;
		deque<int> funnel = makeFirstFunnel(src, points, d.a, d.b);
		int apex = -1;
		funnels.push_back(funnel);
		for (int i = 1; i < tree.size(); i++){
			bool lastTriangle = i == tree.size() - 1;
			if (lastTriangle)
				nextVertex = -2;
			else
				nextVertex = getDifferentVertex(d, triangles.data() + tree[i] * 3);

			bool leftFirst = !lastTriangle && !triangleContainsVertex(triangles.data() + tree[i + 1] * 3, funnel[0]);

			vec2 target = nextVertex == -2 ? dest : points[nextVertex];

			int headIndex = findPredecessor(funnel, apex, points, src, dest, nextVertex, leftFirst);
			int apexIndex = find(funnel.begin(), funnel.end(), apex) - funnel.begin();
			bool popedApex = (leftFirst && headIndex > apexIndex) || (!leftFirst && headIndex < apexIndex);
			int head = funnel[headIndex];
			if (leftFirst)
				funnel.erase(funnel.begin(), funnel.begin() + headIndex);
			else
				funnel.erase(funnel.begin() + headIndex + 1, funnel.end());


			if (!lastTriangle)
			{
				if (leftFirst)
					funnel.push_front(nextVertex);
				else
					funnel.push_back(nextVertex);
				funnels.push_back(funnel);
				if (popedApex)
					apex = head;

				preds[nextVertex] = head;
			}
			else
				nextVertex = head;

			if (!lastTriangle && triangleContainsVertex(triangles.data() + tree[i + 1] * 3, d.a))
				d.b = nextVertex;
			else
				d.a = nextVertex;
		}

		while (nextVertex != -1){
			resp.insert(resp.begin(), nextVertex);
			nextVertex = preds[nextVertex];
		}
		delete preds;
	}


	return resp;
}