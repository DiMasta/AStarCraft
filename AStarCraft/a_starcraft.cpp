#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <set>
#include <queue>
#include <algorithm>
#include <ctime>
#include <deque>
#include <math.h>
#include <climits>
#include <cstring>
#include <fstream>
#include <iterator>

using namespace std;

//#define OUTPUT_GAME_DATA
#define REDIRECT_CIN_FROM_FILE
//#define REDIRECT_COUT_TO_FILE
#define DEBUG_ONE_TURN

static const string INPUT_FILE_NAME = "input.txt";
static const string OUTPUT_FILE_NAME = "output.txt";
static const string EMPTY_STRING = "";

static const int INVALID_ID = -1;
static const int INVALID_NODE_DEPTH = -1;
static const int TREE_ROOT_NODE_DEPTH = 1;
static const int ZERO_CHAR = '0';
static const int DIRECTIONS_COUNT = 8;
static const int BYTE_SIZE = 8;

static const int BOARD_WIDTH = 19;
static const int BOARD_HEIGHT = 10;
static const int MAXIMUM_ROBOTS_COUNT = 10;

enum class CellType : char {
	INVALID = '&',
	VOID = '#',
	EMPTY_PLATFORM = '.',
	UP_ARROW = 'U',
	RIGHT_ARROW = 'R',
	DOWN_ARROW = 'D',
	LEFT_ARROW = 'L',
};

enum class RobotDirection : char {
	INVALID = '&',
	UP = 'U',
	RIGHT = 'R',
	DOWN = 'D',
	LEFT = 'L',
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

// First 40 bits are showing each robot movement in that cell
// Next 6 flags are shwoing the type of the cell (void, arrow)
typedef long long Cell;
static const Cell DEFAULT_CELL = 0;

namespace Masks {
	static const Cell VOID	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'1000'0000'0000'0000'0000'0000'0000;
	static const Cell EMPTY	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0100'0000'0000'0000'0000'0000'0000;
	static const Cell UP	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0010'0000'0000'0000'0000'0000'0000;
	static const Cell RIGTH	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0001'0000'0000'0000'0000'0000'0000;
	static const Cell DOWN	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'1000'0000'0000'0000'0000'0000;
	static const Cell LEFT	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0100'0000'0000'0000'0000'0000;
};

class Board {
public:
	Board();
	~Board();

	void init();
	void addRow(int rowIdx, const string& line);

private:
	Cell board[BOARD_HEIGHT][BOARD_WIDTH];
};

//*************************************************************************************************************
//*************************************************************************************************************

Board::Board() {
	init();
}

//*************************************************************************************************************
//*************************************************************************************************************

Board::~Board() {

}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::init() {
	for (int rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		for (int colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
			board[rowIdx][colIdx] = DEFAULT_CELL;
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::addRow(int rowIdx, const string& line) {
	for (int colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
		switch (static_cast<CellType>(line[colIdx])) {
			case (CellType::VOID): {
				board[rowIdx][colIdx] |= Masks::VOID;
				break;
			}
			case (CellType::EMPTY_PLATFORM): {
				board[rowIdx][colIdx] |= Masks::EMPTY;
				break;
			}
			case (CellType::UP_ARROW): {
				board[rowIdx][colIdx] |= Masks::UP;
				break;
			}
			case (CellType::RIGHT_ARROW ): {
				board[rowIdx][colIdx] |= Masks::RIGTH;
				break;
			}
			case (CellType::DOWN_ARROW): {
				board[rowIdx][colIdx] |= Masks::DOWN;
				break;
			}
			case (CellType::LEFT_ARROW): {
				board[rowIdx][colIdx] |= Masks::LEFT;
				break;
			}
			default: {
				break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Robot {
public:
	Robot();
	~Robot();

private:
	RobotDirection direction;
	int8_t xPos;
	int8_t yPos;
};

//*************************************************************************************************************
//*************************************************************************************************************

Robot::Robot() :
	direction(RobotDirection::INVALID),
	xPos(0),
	yPos(0)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Robot::~Robot() {

}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class State {
public:
	State();
	~State();

	void addBoardRow(int rowIdx, const string& line);
private:
	Board board;
	Robot robots[MAXIMUM_ROBOTS_COUNT];
};

//*************************************************************************************************************
//*************************************************************************************************************

State::State() :
	board()
{

}

//*************************************************************************************************************
//*************************************************************************************************************

State::~State() {

}

//*************************************************************************************************************
//*************************************************************************************************************

void State::addBoardRow(int rowIdx, const string& line) {
	board.addRow(rowIdx, line);
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef int NodeId;
const NodeId INVALID_NODE_ID = -1;

class Node {
public:
	Node();
	Node(NodeId id, int nodeDepth, NodeId parentId, bool rootNote, bool explored, bool inFrontier);
	~Node();

	NodeId getId() const {
		return id;
	}

	int getNodeDepth() const {
		return nodeDepth;
	}

	NodeId getParentId() const {
		return parentId;
	}

	bool getRootNode() const {
		return rootNote;
	}

	bool getExplored() const {
		return explored;
	}

	bool getInFrontier() const {
		return inFrontier;
	}

	void setId(NodeId id) { this->id = id; }
	void setNodeDepth(int nodeDepth) { this->nodeDepth = nodeDepth; }
	void setParentId(NodeId parentId) { this->parentId = parentId; }
	void setRootNode(bool rootNote) { this->rootNote = rootNote; }
	void setExplored(bool explored) { this->explored = explored; }
	void setInFrontier(bool inFrontier) { this->inFrontier = inFrontier; }

private:
	NodeId id;
	int nodeDepth;
	NodeId parentId;
	bool rootNote;
	bool explored;
	bool inFrontier;
};

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node() :
	id(INVALID_NODE_ID),
	nodeDepth(INVALID_NODE_DEPTH),
	parentId(INVALID_NODE_ID),
	rootNote(false),
	explored(false),
	inFrontier(false)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node(NodeId id, int nodeDepth, NodeId parentId, bool rootNote, bool explored, bool inFrontier) :
	id(id),
	nodeDepth(nodeDepth),
	parentId(parentId),
	rootNote(rootNote),
	explored(explored),
	inFrontier(inFrontier)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Node::~Node() {

}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef vector<NodeId> ChildrenList;
typedef map<NodeId, ChildrenList> GraphMap;
typedef map<NodeId, Node*> IdNodeMap;
typedef vector<NodeId> NodeStack;
typedef deque<NodeId> NodeQueue;
typedef set<NodeId> NodeSet;

class Graph {
public:
	Graph();
	Graph(int nodesCount, GraphMap graph);
	~Graph();

	int getNodesCount() const {
		return nodesCount;
	}

	GraphMap getGraph() const {
		return graph;
	}

	IdNodeMap getIdNodeMap() const {
		return idNodeMap;
	}

	Node* getNode(NodeId nodeId) const {
		return idNodeMap.at(nodeId);
	}

	void setNodesCount(int nodesCount) { this->nodesCount = nodesCount; }
	void setGraph(GraphMap graph) { this->graph = graph; }
	void setIdNodeMap(IdNodeMap idNodeMap) { this->idNodeMap = idNodeMap; }

	void addEdge(NodeId parentId, NodeId childId);
	void createNode(NodeId nodeId, int nodeDepth, NodeId parentId);
	void clear();
	bool nodeCreated(NodeId nodeId) const;
	void deleteAllNodes();
	vector<NodeId> treeRootsIds() const;
	void dfs(NodeId treeRootNodeId);
	void bfs(NodeId treeRootNodeId);
	int getMaxNodeDepth() const;
	bool edgeExists(NodeId parent, NodeId child) const;
	vector<NodeId> backtrack(NodeId from, NodeId to) const;
	NodeId getFirstNodeId() const;
	int depthOfTree(NodeId nodeId) const;
	int treeDiameter(NodeId nodeId) const;
	void graphResetAlgParams();

private:
	int nodesCount;
	GraphMap graph;

	// Map used to store all nodes, used to check if node is already created
	// and for easy accesss when deleteing memory pointed by each node
	IdNodeMap idNodeMap;
};

//*************************************************************************************************************
//*************************************************************************************************************

Graph::Graph() :
	nodesCount(0),
	graph()
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Graph::Graph(int nodesCount, GraphMap graph) :
	nodesCount(nodesCount),
	graph(graph)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Graph::~Graph() {
	deleteAllNodes();
	graph.clear();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::deleteAllNodes() {
	for (IdNodeMap::iterator it = idNodeMap.begin(); it != idNodeMap.end(); ++it) {
		Node* node = it->second;

		if (node) {
			delete node;
			node = NULL;
		}
	}

	idNodeMap.clear();
	nodesCount = 0;
}

//*************************************************************************************************************
//*************************************************************************************************************

vector<NodeId> Graph::treeRootsIds() const {
	vector<NodeId> res;

	for (IdNodeMap::const_iterator nodeIt = idNodeMap.begin(); nodeIt != idNodeMap.end(); ++nodeIt) {
		NodeId nodeId = nodeIt->first;

		bool isChildOfANode = false;

		for (GraphMap::const_iterator graphIt = graph.begin(); graphIt != graph.end(); ++graphIt) {
			ChildrenList childrenList = graphIt->second;
			if (find(childrenList.begin(), childrenList.end(), nodeId) != childrenList.end()) {
				isChildOfANode = true;
				break;
			}
		}

		if (!isChildOfANode) {
			res.push_back(nodeId);
		}
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::dfs(NodeId treeRootNodeId) {
	getNode(treeRootNodeId)->setNodeDepth(TREE_ROOT_NODE_DEPTH);

	NodeStack frontier;

	frontier.push_back(treeRootNodeId);
	idNodeMap[treeRootNodeId]->setInFrontier(true);

	while (!frontier.empty()) {
		NodeId state = frontier.back();
		frontier.pop_back();
		idNodeMap[treeRootNodeId]->setInFrontier(false);

		idNodeMap[state]->setExplored(true);

		ChildrenList* children = &graph[state];
		for (size_t childIdx = 0; childIdx < children->size(); ++childIdx) {
			NodeId childId = children->at(childIdx);

			bool nodeExplored = idNodeMap[childId]->getExplored();
			bool nodeInFrontier = idNodeMap[childId]->getInFrontier();
			if (!nodeExplored && !nodeInFrontier) {
				frontier.push_back(childId);

				int parentDepth = idNodeMap[state]->getNodeDepth();
				idNodeMap[childId]->setNodeDepth(parentDepth + 1);
				idNodeMap[childId]->setParentId(state);
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::bfs(NodeId treeRootNodeId) {
	getNode(treeRootNodeId)->setNodeDepth(TREE_ROOT_NODE_DEPTH);

	NodeQueue frontier;

	frontier.push_back(treeRootNodeId);
	idNodeMap[treeRootNodeId]->setInFrontier(true);

	while (!frontier.empty()) {
		NodeId state = frontier.front();
		frontier.pop_front();
		idNodeMap[treeRootNodeId]->setInFrontier(false);

		idNodeMap[state]->setExplored(true);

		ChildrenList* children = &graph[state];
		for (size_t childIdx = 0; childIdx < children->size(); ++childIdx) {
			NodeId childId = children->at(childIdx);

			bool nodeExplored = idNodeMap[childId]->getExplored();
			bool nodeInFrontier = idNodeMap[childId]->getInFrontier();
			if (!nodeExplored && !nodeInFrontier) {
				frontier.push_back(childId);

				int parentDepth = idNodeMap[state]->getNodeDepth();
				idNodeMap[childId]->setNodeDepth(parentDepth + 1);
				idNodeMap[childId]->setParentId(state);
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

int Graph::getMaxNodeDepth() const {
	int maxNodeDepth = INVALID_NODE_DEPTH;

	for (IdNodeMap::const_iterator nodeIt = idNodeMap.begin(); nodeIt != idNodeMap.end(); ++nodeIt) {
		int nodeDepth = nodeIt->second->getNodeDepth();
		if (nodeDepth > maxNodeDepth) {
			maxNodeDepth = nodeDepth;
		}
	}

	return maxNodeDepth;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Graph::edgeExists(NodeId parent, NodeId child) const {
	bool res = false;

	if (nodeCreated(parent) && nodeCreated(child)) {
		ChildrenList children = graph.at(parent); // TODO: copying do not copy use * for children
		res = find(children.begin(), children.end(), child) != children.end();
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

vector<NodeId> Graph::backtrack(NodeId from, NodeId to) const {
	vector<NodeId> path;

	while (from != to) {
		path.push_back(from);
		from = idNodeMap.at(from)->getParentId();
	}

	path.push_back(to);

	reverse(path.begin(), path.end());

	return path;
}

//*************************************************************************************************************
//*************************************************************************************************************

NodeId Graph::getFirstNodeId() const {
	return idNodeMap.begin()->first;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Graph::depthOfTree(NodeId nodeId) const {
	if (idNodeMap.at(nodeId)->getRootNode()) {
		return 0;
	}

	int maxdepth = 0;

	for (ChildrenList::const_iterator nodeIt = graph.at(nodeId).begin(); nodeIt != graph.at(nodeId).end(); ++nodeIt) {
		maxdepth = max(maxdepth, depthOfTree(*nodeIt));
	}

	return maxdepth + 1;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Graph::treeDiameter(NodeId nodeId) const {
	if (idNodeMap.at(nodeId)->getRootNode()) {
		return 0;
	}

	int max1 = 0, max2 = 0;
	for (ChildrenList::const_iterator nodeIt = graph.at(nodeId).begin(); nodeIt != graph.at(nodeId).end(); ++nodeIt) {
		int h = depthOfTree(*nodeIt);
		if (h > max1) {
			max2 = max1;
			max1 = h;
		}
		else if (h > max2) {
			max2 = h;
		}
	}

	int maxChildDia = 0;
	for (ChildrenList::const_iterator nodeIt = graph.at(nodeId).begin(); nodeIt != graph.at(nodeId).end(); ++nodeIt) {
		maxChildDia = max(maxChildDia, treeDiameter(*nodeIt));
	}

	return max(maxChildDia, max1 + max2 + 1);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::graphResetAlgParams() {
	for (IdNodeMap::iterator nodeIt = idNodeMap.begin(); nodeIt != idNodeMap.end(); ++nodeIt) {
		nodeIt->second->setExplored(false);
		nodeIt->second->setInFrontier(false);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::addEdge(NodeId parentId, NodeId childId) {
	graph[parentId].push_back(childId);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::createNode(NodeId nodeId, int nodeDepth, NodeId parentId) {
	if (!nodeCreated(nodeId)) {
		Node* node = new Node(nodeId, nodeDepth, parentId, false, false, false);
		idNodeMap[nodeId] = node;
		graph[nodeId];
		++nodesCount;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Graph::clear() {
	deleteAllNodes();
	graph.clear();
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Graph::nodeCreated(NodeId nodeId) const {
	return idNodeMap.end() != idNodeMap.find(nodeId);
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Game {
public:
	Game();
	~Game();

	void initGame();
	void gameBegin();
	void gameEnd();
	void gameLoop();
	void getGameInput();
	void getTurnInput();
	void turnBegin();
	void makeTurn();
	void turnEnd();
	void play();

	void debug() const;

private:
	int turnsCount;

	State gameState;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	gameState()
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Game::~Game() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::initGame() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameBegin() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameEnd() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameLoop() {
	while (true) {
		turnBegin();
		getTurnInput();
		makeTurn();
		turnEnd();

#ifdef DEBUG_ONE_TURN
		break;
#endif // DEBUG_ONE_TURN
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getGameInput() {
	for (int rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		string line;
		cin >> line; cin.ignore();

		gameState.addBoardRow(rowIdx, line);
	}

	int robotCount;
	cin >> robotCount; cin.ignore();
	for (int i = 0; i < robotCount; i++) {
		int x;
		int y;
		string direction;
		cin >> x >> y >> direction; cin.ignore();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getTurnInput() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnBegin() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	//cout << "0 0 U 1 1 R 2 2 D 3 3 L" << endl;
	cout << "3 4 R 4 4 R 5 4 L" << endl;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnEnd() {
	++turnsCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::play() {
	initGame();
	getGameInput();
	gameBegin();
	gameLoop();
	gameEnd();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::debug() const {
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

#ifdef TESTS
#include "debug.h"
#endif // TESTS

int main(int argc, char** argv) {
#ifdef TESTS
	doctest::Context context;
	int res = context.run();
#else

#ifdef REDIRECT_CIN_FROM_FILE
	ifstream in(INPUT_FILE_NAME);
	streambuf *cinbuf = cin.rdbuf();
	cin.rdbuf(in.rdbuf());
#endif // REDIRECT_CIN_FROM_FILE

#ifdef REDIRECT_COUT_TO_FILE
	ofstream out(OUTPUT_FILE_NAME);
	streambuf *coutbuf = cout.rdbuf();
	cout.rdbuf(out.rdbuf());
#endif // REDIRECT_COUT_TO_FILE

	Game game;
	game.play();
#endif // TESTS

	return 0;
}