// **************************************************************************
// File       [ graph.cpp ]
// Author     [ Eddy Hsu ]
// Synopsis   [ This is a finite state machine. ]
// Date       [ 2017/06/09 created ]
// **************************************************************************
#include "graph.h"


Edge::Edge(Node *a, Node *b, string& iw, string& ow)
{
	node[0] = a; 
	node[1] = b;
	input_weight = iw;	
	output_weight = ow;	
}

bool Edge::operator < (const Edge& rhs) const{

	int id1a = node[0]->id;
	int id1b = node[1]->id;

	int id2a = rhs.node[0]->id;
	int id2b = rhs.node[1]->id;


	if ( id1a == id2a ) return id1b < id2b;
	if ( id1a == id2b ) return id1b < id2a;
	if ( id1b == id2a ) return id1a < id2b;
	if ( id1b == id2b ) return id1a < id2a;

	return output_weight < rhs.output_weight;
}

Node * Edge::getNeighbor(Node *n)
{
	if ( node[0] == n ) return node[1];
	if ( node[1] == n ) return node[0];

	return 0;	
}

Node::Node(const int& i)
{
	id = i;
	traveled = false;
	color = -1;
	d = DIS_INF;
	prev = 0;
}

void Node::addEdgeIn(Edge *e)
{
	edge_in.push_back(e);
}

void Node::addEdgeOut(Edge *e)
{
	edge_out.push_back(e);
}

bool edgeComp( const Edge* A, const Edge* B ){
	if ( (*A) < (*B) ) return true;
	return false;
}

void Node::sortEdgeIn()
{
    sort(edge_in.begin(), edge_in.end(), edgeComp);
}

bool edgeCompByOW( const Edge* A, const Edge* B ){
	if ( A->output_weight < B->output_weight ) return true;
	return false;
}

void Node::sortEdgeInByOutputWeight()
{
    sort(edge_in.begin(), edge_in.end(), edgeCompByOW);
}


Graph::Graph() {}

Graph::~Graph()
{
	vector<Edge *>::iterator itE;
	for ( itE = edges.begin() ; itE != edges.end() ; itE++ )
	{
		delete (*itE);
		(*itE) = 0;
	}
	
	map<int, Node *>::iterator itN;
	for ( itN = nodesMap.begin() ; itN != nodesMap.end() ; itN++ )
	{
		delete (*itN).second;
		(*itN).second = 0;
	}

    /*vector<Node *>::iterator itN2;
	for ( itN2 = nodes.begin() ; itN2 != nodes.end() ; itN2++ )
	{
		(*itN2) = 0;
	}*/
}
		
void Graph::addEdge(const int& v1, const int& v2, string& iw, string& ow)
{
	Node *a, *b;
	map<int, Node *>::iterator it = nodesMap.find(v1);
	if ( it != nodesMap.end() )
		a = (*it).second;
	else
	{
		a = new Node(v1);
		nodesMap[v1] = a;
        nodes.push_back(a);
	}

	it = nodesMap.find(v2);
	if ( it != nodesMap.end() )
		b = (*it).second;
	else
	{
		b = new Node(v2);
		nodesMap[v2] = b;
        nodes.push_back(b);
	}

	Edge *e = new Edge(a, b, iw, ow);
	edges.push_back(e);

	a->edge_out.push_back(e);
	b->edge_in.push_back(e);

}
	
void Graph::sortEdgesInOfNode()
{
	map<int, Node *>::iterator it;
	for ( it = nodesMap.begin() ; it != nodesMap.end() ; it++ )
	{
		Node *node = (*it).second;
		node->sortEdgeIn();
	}

}

bool edgeCompByIW( const Edge* A, const Edge* B )
{
	if ( A->input_weight < B->input_weight ) return true;
	return false;
}

void Graph::sortEdgesByInputWeight()
{
    sort(edges.begin(), edges.end(), edgeCompByIW);
}


bool NodeCompByD( const Node* A, const Node* B )
{
	if ( A->edge_in.size() + A->edge_out.size() > B->edge_in.size() + B->edge_out.size() ) 
        return true;
    else if (A->edge_in.size() + A->edge_out.size() == B->edge_in.size() + B->edge_out.size()) {
        if (A->id < B->id) 
            return true;
        else 
            return false;
    }
    else
        return false;
}

void Graph::sortNodesByDegree()
{
    sort(nodes.begin(), nodes.end(), NodeCompByD);
}


bool NodeCompByID( const Node* A, const Node* B ){
	if ( A->id < B->id ) return true;
    else return false;
}

void Graph::sortNodesByID()
{
    sort(nodes.begin(), nodes.end(), NodeCompByID);
}

void Graph::init()
{
	map<int, Node *>::iterator itN;
	for ( itN = nodesMap.begin() ; itN != nodesMap.end() ; itN++ )
	{
		Node *node = (*itN).second;
		node->traveled = false;
		node->d = DIS_INF;
		node->prev = 0;
    	node->color = -1;
	}
	
}

void Graph::displayNodesAndEdges()
{
	cout << "# of state = " << nodes.size() << endl;
    for(int i=0; i<nodes.size();i++){
        cout << "s" << nodes[i]->id << ": " << endl;
        for(int j=0; j<nodes[i]->edge_out.size();j++){
        	cout << nodes[i]->edge_out[j]->input_weight;
        	cout << " s" << nodes[i]->edge_out[j]->node[1]->id << " ";
        	cout << nodes[i]->edge_out[j]->output_weight << endl;
        }
        cout << endl;
    }
}

void Graph::convertFSMtoDot(char* const dotfile) 
{
	ofstream file;
	file.open("test.dot");    
    cout << endl << "digraph fsm_graph {" << endl;
    file << endl << "digraph fsm_graph {" << endl;
    for(int i=0; i<nodes.size();i++){
        for(int j=0; j<nodes[i]->edge_out.size();j++){
            cout << "	s" << nodes[i]->id << " ";
        	cout << "-> s" << nodes[i]->edge_out[j]->node[1]->id << " ";
        	cout << " [label = \"" << nodes[i]->edge_out[j]->input_weight << "/" << nodes[i]->edge_out[j]->output_weight << "\"];" << endl;
            file << "	s" << nodes[i]->id << " ";
        	file << "-> s" << nodes[i]->edge_out[j]->node[1]->id << " ";         	
        	file <<  " [label = \"" << nodes[i]->edge_out[j]->input_weight << "/" << nodes[i]->edge_out[j]->output_weight << "\"];" << endl;
        	//cout << nodes[i]->edge_out[j]->input_weight;
        	//cout << nodes[i]->edge_out[j]->output_weight << endl;
        }
    }
    cout << "}" << endl;
    file << "}" <<endl;
    file.close();
}

set<int> Graph::countCycleNode()
{
	for(int i=0;i<edges.size();i++){
		if(edges[i]->node[0]==edges[i]->node[1]){
			cout << "Cycle Node = s" << edges[i]->node[0]->id << endl;
			cycle_node.insert(edges[i]->node[0]->id);
		}
	}
	return cycle_node;
}

Node * Graph::getNodeById(const int& id)
{
	return nodesMap[id];
}

void Graph::initializeWithVector(vector < vector <int> >& adj_list, vector < vector <string> >& input_list, vector < vector <string> >& output_list) 
{
	for(int i=0; i<output_list.size();i++){
        for(int j=0; j<output_list[i].size();j++){
        	addEdge(i, adj_list[i][j], input_list[i][j], output_list[i][j]);
        }
    }
}
