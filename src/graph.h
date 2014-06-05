/*
 * graph.h
 *
 *  Created on: 2014. 4. 24.
 *      Author: sunghee
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <string>
#include <vector>
#include <stack>

using namespace std;

//class node * node_pointer;
class edge;

// declaration of vertex
class vertex {
public:
	string label;
	double x; // coordinate of x;
	double y; // coordinate of y;
	double z; // corrdinate of z;
	vector<edge> links; //edges connected to this vertex
	vector<vertex> neighbors; // vertices connected to this vertex
};
// declaration of edge
class edge{
public:
	string label;
	double length;
	double diameter;
	edge* prev;
	edge* next;

	vertex * start_node, *end_node;
} ;

// ADT for graph
template <class T, class S>
class graph{
public:
	T * head_vertex;
	S * head_edge;
	vector<T> nodes;
	vector<S> links;

public:
	graph(void)
	{
		head_vertex=NULL;
		head_edge=NULL;

	};
	~graph(void)
	{
		nodes.clear();
		links.clear();
	}
	void addVertex(T aNode)
	{
		nodes.push_back(aNode);
		return;
	}
	void addEdge(S aEdge)
	{
		links.push_back(aEdge);

		return;
	}
	stack<string> path(string s_string, string e_string)
	{
	//	ostringstream s_string, e_string;   // stream used for the conversion
	//	s_string << start;
	//	e_string << end;
		stack<string> path;
		vector<vertex> neighbors;

		T* currVertexPtr=getVertexByLabel(e_string);
//		cout << "*1-1:currVertex:*" << currVertexPtr->label <<"\n";

//		cout << "*1*" << endl;

		vector<vertex>::iterator iter_vertex;
		vector<edge>::iterator iter_edge;

//		cout << "*2*" << endl;

		T currVertex;
		while ( currVertexPtr->label != s_string )
		{
//			cout << "*2-1:currVertex:*"<<(currVertexPtr->label)<<"\n";
			path.push(currVertexPtr->label);
//			cout << "*2-2*" << endl;
//			cout << "neighbor size: " << getNeighbors(currVertexPtr->label).size() << "\n";

			neighbors = (getNeighbors(currVertexPtr->label));
			currVertex = (neighbors.at(0));
//			cout << "*2-3*:currVertex:"<< currVertex.label;
			currVertexPtr = &currVertex;
		}
//		cout << "*3*" << endl;

		path.push(getVertexByLabel(s_string)->label);
//		cout << "*4*" << endl;

		return path;
	}

	void shortestPath(int start)
	{
		ostringstream s_string, e_string;   // stream used for the conversion
		s_string << start;
	//	e_string << end;
	//	queue<int> line;
		cout << "*1*" << endl;

		vertex* sVertex = getVertexByLabel(s_string.str());
		cout << "*2*" << endl;


		vector<vertex>::iterator iter_vertex;
		vector<edge>::iterator iter_edge;

		cout << "*3*" << endl;
		//
		for (iter_vertex = sVertex->neighbors.begin(); iter_vertex !=sVertex->neighbors.end(); iter_vertex++)
		{  // if there is a
			if (iter_edge->start_node->label == s_string.str())
			{

	//			return &(*iter_edge);
				if (iter_edge->end_node->label == s_string.str())
					;//return &(*iter_edge);
			}
		}
		cout << "*4*" << endl;

		return ;
	}
	T* getVertexByLabel(string aLabel)
	{
		vector<vertex>::iterator iter;
		for (iter = nodes.begin(); iter !=nodes.end(); iter++)
		{
			if (iter->label == aLabel)
				return &(*iter);
		}
		return NULL;

	}
	S* getEdgeByLabel(string aLabel)
	{
		vector<edge>::iterator iter;
		for (iter = links.begin(); iter !=links.end(); iter++)
		{
			if (iter->label == aLabel)
				return &(*iter);
		}
		return NULL;
	}
	vector<T> getNeighbors(string aVertex)
	{
	//	ostringstream v_string;   // stream used for the conversion
	//	v_string << aVertex;
//		cout << "*2-2-1*" << endl;

		T* theVertex;
//		cout << "*2-2-2*" << endl;
		theVertex =  getVertexByLabel(aVertex);
//		cout << "*theVertex label:"<< aVertex <<"***"<< endl;
		return (theVertex->neighbors);
	}


};

class vessel
{

	public:
		string labels[];

		graph<vertex,edge> v_graph; // vascular graph

	public:
		vessel();
		~vessel();

};

template<class LabelType>

class GraphInterface{

public:

//	~graph();

	virtual int getNumVertices() const = 0;



	virtual int getNumEdges() const = 0;



	virtual bool add(LabelType start, LabelType end, int edgeWedight) = 0;

	virtual bool remove(LabelType start, LabelType end)= 0;


	virtual int getEdgeWeight(LabelType start, LabelType end) const = 0;


	virtual void depthFirstTraversal(LabelType start, void visit(LabelType&)) = 0;

	virtual void breadthFirstTraversal(LabelType start, void visit(LabelType&)) = 0;

};



#endif /* GRAPH_H_ */
