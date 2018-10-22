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
static const string SPACE = " ";

static const int INVALID_ID = -1;
static const int INVALID_NODE_DEPTH = -1;
static const int TREE_ROOT_NODE_DEPTH = 1;
static const int ZERO_CHAR = '0';
static const int DIRECTIONS_COUNT = 9;
static const int BYTE_SIZE = 8;

static const int8_t ROBOT_DIRECTION = 4;
static const int8_t BOARD_WIDTH = 19;
static const int8_t BOARD_HEIGHT = 10;
static const int8_t MAX_ROBOTS_COUNT = 10;
static const int8_t MAX_CELL_MARKS = 5; // UDLR and Empty

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

typedef uint8_t Cell;

namespace Masks {
	static const Cell FLAG	= 0b1000'0000;
	static const Cell VOID	= 0b0100'0000;
	static const Cell CLEAR = 0b0010'0000; // 0 may be used instead
	static const Cell EMPTY	= 0b0001'0000;
	static const Cell UP	= 0b0000'1000;
	static const Cell RIGTH	= 0b0000'0100;
	static const Cell DOWN	= 0b0000'0010;
	static const Cell LEFT	= 0b0000'0001;
};

static const Cell DEFAULT_CELL = Masks::CLEAR;

static const Cell POSSIBLE_CELL_MARKS[MAX_CELL_MARKS] = {
	Masks::EMPTY,
	Masks::UP,
	Masks::RIGTH,
	Masks::DOWN,
	Masks::LEFT
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef int8_t Coord;
const Coord INVALID_COORD = -1;

class Coords {
public:
	Coords();
	Coords(Coord xCoord, Coord yCoord);

	Coord getXCoord() const {
		return xCoord;
	}

	Coord getYCoord() const {
		return yCoord;
	}

	void setXCoord(Coord xCoord) { this->xCoord = xCoord; }
	void setYCoord(Coord yCoord) { this->yCoord = yCoord; }

	Coords& operator=(const Coords& other);
	bool operator==(const Coords& other);
	Coords operator+(const Coords& other);
	Coords& operator+=(const Coords& other);
	Coords operator-(const Coords& other);
	Coords& operator-=(const Coords& other);
	Coords operator*(const Coords& other);
	Coords& operator*=(const Coords& other);

	bool isValid() const;
	void debug() const;
	void print() const;

	Coord distance(const Coords& coords) const;
	void roundCoords();
	void clamp();

	friend Coord distance(const Coords& point0, const Coords& point1);
private:
	Coord xCoord;
	Coord yCoord;
};

//*************************************************************************************************************
//*************************************************************************************************************

Coords::Coords() :
	xCoord(INVALID_COORD),
	yCoord(INVALID_COORD)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords::Coords(
	Coord xCoord,
	Coord yCoord
) :
	xCoord(xCoord),
	yCoord(yCoord)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords& Coords::operator=(const Coords& other) {
	if (this != &other) {
		xCoord = other.xCoord;
		yCoord = other.yCoord;
	}

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Coords::operator==(const Coords& other) {
	return (xCoord == other.xCoord) && (yCoord == other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords Coords::operator+(const Coords& other) {
	return Coords(xCoord + other.xCoord, yCoord + other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords& Coords::operator+=(const Coords& other) {
	xCoord += other.xCoord;
	yCoord += other.yCoord;

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords Coords::operator-(const Coords& other) {
	return Coords(xCoord - other.xCoord, yCoord - other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords& Coords::operator-=(const Coords& other) {
	xCoord -= other.xCoord;
	yCoord -= other.yCoord;

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords Coords::operator*(const Coords& other) {
	return Coords(xCoord * other.xCoord, yCoord * other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords& Coords::operator*=(const Coords& other) {
	xCoord *= other.xCoord;
	yCoord *= other.yCoord;

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Coords::isValid() const {
	return INVALID_COORD != xCoord && INVALID_COORD != yCoord;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Coords::debug() const {
	cerr << "Position: X=" << xCoord << ", Y=" << yCoord << endl;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Coords::print() const {
	cout << xCoord << " " << yCoord;
}

//*************************************************************************************************************
//*************************************************************************************************************

Coord Coords::distance(const Coords& coords) const {
	Coord kat0 = coords.xCoord - xCoord;
	Coord kat1 = coords.yCoord - yCoord;

	Coord hip = (Coord)sqrt((kat0 * kat0) + (kat1 * kat1));
	return hip;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Coords::roundCoords() {
	xCoord = static_cast<Coord>(round(xCoord));
	yCoord = static_cast<Coord>(round(yCoord));
}

//*************************************************************************************************************
//*************************************************************************************************************

void Coords::clamp() {
	if (xCoord < 0) {
		xCoord = BOARD_WIDTH - 1;
	}

	if (xCoord > BOARD_WIDTH - 1) {
		xCoord = 0;
	}

	if (yCoord < 0) {
		yCoord = BOARD_HEIGHT - 1;
	}

	if (yCoord > BOARD_HEIGHT- 1) {
		yCoord = 0;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

Coord distance(const Coords& point0, const Coords& point1) {
	Coord lineXLenght = static_cast<Coord>(abs(point0.xCoord - point1.xCoord));
	Coord lineYLenght = static_cast<Coord>(abs(point0.yCoord - point1.yCoord));

	Coord distance = static_cast<Coord>(sqrt((lineXLenght * lineXLenght) + (lineYLenght * lineYLenght)));

	return distance;
}

//*************************************************************************************************************
//*************************************************************************************************************

enum Direction {
	DIR_INVALID = -1,
	DIR_N = 0,
	DIR_NE,
	DIR_E,
	DIR_SE,
	DIR_S,
	DIR_SW,
	DIR_W,
	DIR_NW,
	DIR_STAY,
};

//*************************************************************************************************************
//*************************************************************************************************************

Coords DIRECTIONS[DIRECTIONS_COUNT] = {
	Coords(0, -1), // N
	Coords(1, -1), // NE
	Coords(1,  0), // E
	Coords(1,  1), // SE
	Coords(0,  1), // S
	Coords(-1,  1), // SW
	Coords(-1,  0), // W
	Coords(-1, -1),  // NW
	Coords(0, 0) // STAY
};


//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Board {
public:
	Board();
	Board(const Board& board);
	~Board();

	Board& operator=(const Board& board);

	void init();
	void addRow(int rowIdx, const string& line);
	void findFirstClearCell(Coords& firstClearCell) const;
	void markCell(const Coords& clearCellCoords, Cell mark);
	void getMoves(string& moves) const;
	
	bool isMarkPossible(const Coords& clearCellCoords, Cell mark) const;

	Cell getCell(const Coords& coords) const;

private:
	Cell gameBoard[BOARD_HEIGHT][BOARD_WIDTH];

	void copyGameBoard(const Board& board);
};

//*************************************************************************************************************
//*************************************************************************************************************

Board::Board() {
	init();
}

//*************************************************************************************************************
//*************************************************************************************************************

Board::Board(const Board& board) {
	copyGameBoard(board);
}

//*************************************************************************************************************
//*************************************************************************************************************

Board::~Board() {

}

//*************************************************************************************************************
//*************************************************************************************************************

Board& Board::operator=(const Board& board) {
	if (this != &board) {
		copyGameBoard(board);
	}

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::init() {
	for (int8_t rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		for (int8_t colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
			gameBoard[rowIdx][colIdx] = DEFAULT_CELL;
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::addRow(int rowIdx, const string& line) {
	for (int8_t colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
		Cell& cell = gameBoard[rowIdx][colIdx];
		CellType cellType = static_cast<CellType>(line[colIdx]);

		switch (cellType) {
			case (CellType::VOID): {
				cell |= Masks::VOID;
				break;
			}
			case (CellType::EMPTY_PLATFORM): {
				cell |= Masks::EMPTY;
				break;
			}
			case (CellType::UP_ARROW): {
				cell |= Masks::UP;
				break;
			}
			case (CellType::RIGHT_ARROW ): {
				cell |= Masks::RIGTH;
				break;
			}
			case (CellType::DOWN_ARROW): {
				cell |= Masks::DOWN;
				break;
			}
			case (CellType::LEFT_ARROW): {
				cell |= Masks::LEFT;
				break;
			}
			default: {
				break;
			}
		}

		// At the beginning the empty cells are not considered clear
		// They are set to empty and not clear when expanding the tree as one of the options
		if (CellType::EMPTY_PLATFORM != cellType) {
			cell &= ~Masks::CLEAR; // Unset CLEAR mark
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::copyGameBoard(const Board& board) {
	for (int8_t rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		for (int8_t colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
			gameBoard[rowIdx][colIdx] = board.gameBoard[rowIdx][colIdx];
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::findFirstClearCell(Coords& firstClearCell) const {
	for (int8_t rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		for (int8_t colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
			if (gameBoard[rowIdx][colIdx] & Masks::CLEAR) {
				firstClearCell.setXCoord(colIdx);
				firstClearCell.setYCoord(rowIdx);

				return; // Fastest way to exit the two loops, without if check for the outer
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::markCell(const Coords& clearCellCoords, Cell mark) {
	Cell& cell = gameBoard[clearCellCoords.getYCoord()][clearCellCoords.getXCoord()];
	cell |= mark;
	cell &= ~Masks::CLEAR;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Board::isMarkPossible(const Coords& clearCellCoords, Cell mark) const {
	// If staying empty, won't go to VOID
	if (Masks::EMPTY == mark) {
		return true;
	}

	Direction dir = DIR_STAY;
	
	switch (mark) {
		case (Masks::UP): {
			dir = DIR_N;
			break;
		}
		case (Masks::LEFT): {
			dir = DIR_W;
			break;
		}
		case (Masks::DOWN): {
			dir = DIR_S;
			break;
		}
		case (Masks::RIGTH): {
			dir = DIR_E;
			break;
		}
		default: {
			break;
		}
	}

	Coords newCoords =  DIRECTIONS[dir] + clearCellCoords;
	newCoords.clamp();

	return !(gameBoard[newCoords.getYCoord()][newCoords.getXCoord()] & Masks::VOID);
}

//*************************************************************************************************************
//*************************************************************************************************************

Cell Board::getCell(const Coords& coords) const {
	return gameBoard[coords.getYCoord()][coords.getXCoord()];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Board::getMoves(string& moves) const {
	moves = EMPTY_STRING;

	for (int8_t rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		for (int8_t colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
			Cell cell = gameBoard[rowIdx][colIdx];

			if (!(cell & Masks::EMPTY)) {
				if (cell & Masks::UP) {
					moves += static_cast<char>(RobotDirection::UP);
				}

				if (cell & Masks::LEFT) {
					moves += static_cast<char>(RobotDirection::LEFT);
				}

				if (cell & Masks::DOWN) {
					moves += static_cast<char>(RobotDirection::DOWN);
				}

				if (cell & Masks::RIGTH) {
					moves += static_cast<char>(RobotDirection::RIGHT);
				}

				moves += SPACE;
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
	Robot(const Robot& robot);
	~Robot();

	Robot& operator=(const Robot& robot);

	const Coords& getPosiiton() const { return position; }
	RobotDirection getDirection() const { return direction; }
	bool getFirstTurn() const { return firstTurn; }

	void setPosition(const Coords& position) { this->position = position; }
	void setDirection(RobotDirection direction) { this->direction = direction; }
	void setFirstTurn(bool firstTurn) { this->firstTurn = firstTurn; }

	void move();
	void rotate(Cell cell);
	void destroyRobot();

	bool isDead() const;

private:
	Coords position;
	RobotDirection direction;
	bool firstTurn; // Not sure if to use this or to rotate twice
};

//*************************************************************************************************************
//*************************************************************************************************************

Robot::Robot() :
	position(),
	direction(RobotDirection::INVALID),
	firstTurn(true)
{

}
//*************************************************************************************************************
//*************************************************************************************************************

Robot::Robot(const Robot& robot) :
	position(robot.position),
	direction(robot.direction),
	firstTurn(robot.firstTurn)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Robot::~Robot() {

}

//*************************************************************************************************************
//*************************************************************************************************************

Robot& Robot::operator=(const Robot& robot) {
	if (this != &robot) {
		position = robot.position;
		direction = robot.direction;
		firstTurn = robot.firstTurn;
	}

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

// Can be optimized
void Robot::move() {
	Direction dir;

	switch (direction) {
		case RobotDirection::UP: {
			dir = DIR_N;
			break;
		}
		case RobotDirection::RIGHT: {
			dir = DIR_E;
			break;
		}
		case RobotDirection::DOWN: {
			dir = DIR_S;
			break;
		}
		case RobotDirection::LEFT: {
			dir = DIR_W;
			break;
		}
		default: {
			break;
		}
	}

	position += DIRECTIONS[dir];
	position.clamp();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Robot::rotate(Cell cell) {
	if (cell & Masks::UP) {
		direction = RobotDirection::UP;
	}

	if (cell & Masks::RIGTH) {
		direction = RobotDirection::RIGHT;
	}

	if (cell & Masks::DOWN) {
		direction = RobotDirection::DOWN;
	}

	if (cell & Masks::LEFT) {
		direction = RobotDirection::LEFT;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Robot::destroyRobot() {
	setPosition(Coords(INVALID_COORD, INVALID_COORD));
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Robot::isDead() const {
	return !position.isValid();
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef unsigned long long VisitedFlags;
static const VisitedFlags DEFAULT_FLAG = 0b1000'0000;

class State {
public:
	State();

	State(const State& state);

	State& operator=(const State& state);

	void reset();
	void copySate(const State& state);

	void setRobotTrace(
		const Coords& position,
		VisitedFlags robotDirectionFlag
	);

	void addRobotsCount(int8_t robotsCount);

	void addRobot(
		int8_t idx,
		int8_t x,
		int8_t y,
		RobotDirection direction
	);

	bool checkRobotDirection(
		const Coords& position,
		VisitedFlags robotDirectionFlag
	);

	int simulate(const Board& boardState);

	VisitedFlags makeRobotVisitedFlag(int8_t robotIdx, RobotDirection robotDirection);
	VisitedFlags& getVisitedFlags(const Coords& coords);
	 
private:
	VisitedFlags visitedFlags[BOARD_HEIGHT][BOARD_WIDTH];
	Robot initialRobots[MAX_ROBOTS_COUNT];
	Robot robots[MAX_ROBOTS_COUNT];
	int8_t robotsCount;
	int8_t functioningRobotsCount;

};

//*************************************************************************************************************
//*************************************************************************************************************

State::State() :
	robotsCount(0),
	functioningRobotsCount(0)
{
	reset();
}

//*************************************************************************************************************
//*************************************************************************************************************

State::State(const State& state) {
	copySate(state);
}

//*************************************************************************************************************
//*************************************************************************************************************

State& State::operator=(const State& state) {
	if (this != &state) {
		copySate(state);
	}

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::reset() {
	for (int8_t rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		for (int8_t colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
			visitedFlags[rowIdx][colIdx] = 0;
		}
	}

	for (int8_t robotIdx = 0; robotIdx < robotsCount; ++robotIdx) {
		robots[robotIdx] = initialRobots[robotIdx];
	}

	functioningRobotsCount = robotsCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::copySate(const State& state) {
	for (int8_t rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		for (int8_t colIdx = 0; colIdx < BOARD_WIDTH; ++colIdx) {
			visitedFlags[rowIdx][colIdx] = state.visitedFlags[rowIdx][colIdx];
		}
	}
	
	for (int8_t robotIdx = 0; robotIdx < robotsCount; ++robotIdx) {
		robots[robotIdx] = state.robots[robotIdx];
		initialRobots[robotIdx] = state.initialRobots[robotIdx];
	}

	robotsCount = state.robotsCount;
	functioningRobotsCount = state.functioningRobotsCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::setRobotTrace(
	const Coords& position,
	VisitedFlags robotDirectionFlag
) {
	getVisitedFlags(position) |= robotDirectionFlag;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::addRobotsCount(int8_t robotsCount) {
	this->robotsCount = robotsCount;
	functioningRobotsCount = robotsCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::addRobot(int8_t idx, int8_t x, int8_t y, RobotDirection direction) {
	initialRobots[idx].setPosition(Coords(x, y));
	initialRobots[idx].setDirection(direction);

	robots[idx] = initialRobots[idx];
}

//*************************************************************************************************************
//*************************************************************************************************************

bool State::checkRobotDirection(const Coords& position, VisitedFlags robotDirectionFlag) {
	return getVisitedFlags(position) & robotDirectionFlag;
}

//*************************************************************************************************************
//*************************************************************************************************************

int State::simulate(const Board& boardState) {
	int score = 0;

	while (functioningRobotsCount > 0) {
		for (int robotIdx = 0; robotIdx < robotsCount; ++robotIdx) {
			Robot& robot = robots[robotIdx];

			if (!robot.isDead()) {
				++score;

				if (robot.getFirstTurn()) {
					robot.rotate(boardState.getCell(robot.getPosiiton()));
					robot.setFirstTurn(false);
				}

				setRobotTrace(robot.getPosiiton(), makeRobotVisitedFlag(robotIdx, robot.getDirection()));
				robot.move();

				if (boardState.getCell(robot.getPosiiton()) & Masks::VOID) {
					robot.destroyRobot();
					--functioningRobotsCount;
				}
				else {
					robot.rotate(boardState.getCell(robot.getPosiiton()));

					// Robot was here in this direction, destroy it
					if (checkRobotDirection(robot.getPosiiton(), makeRobotVisitedFlag(robotIdx, robot.getDirection()))) {
						robot.destroyRobot();
						--functioningRobotsCount;
					}
				}
			}
		}
	}

	return score;
}

//*************************************************************************************************************
//*************************************************************************************************************

VisitedFlags State::makeRobotVisitedFlag(int8_t robotIdx, RobotDirection robotDirection) {
	VisitedFlags flag = DEFAULT_FLAG;
	flag >>= robotIdx * ROBOT_DIRECTION;

	switch (robotDirection) {
		//case RobotDirection::UP: {
		//	flag >>= 0;
		//	break;
		//}
		case RobotDirection::RIGHT: {
			flag >>= 1;
			break;
		}
		case RobotDirection::DOWN: {
			flag >>= 2;
			break;
		}
		case RobotDirection::LEFT: {
			flag >>= 3;
			break;
		}
		default: {
			break;
		}
	}
	return flag;
}

//*************************************************************************************************************
//*************************************************************************************************************

VisitedFlags& State::getVisitedFlags(const Coords& coords) {
	return visitedFlags[coords.getYCoord()][coords.getXCoord()];
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
	Node(NodeId id, NodeId parentId, const Board& state);
	~Node();

	NodeId getId() const {
		return id;
	}

	NodeId getParentId() const {
		return parentId;
	}

	Board& getBoardState() {
		return boardState;
	}

	void setId(NodeId id) { this->id = id; }
	void setParentId(NodeId parentId) { this->parentId = parentId; }
	void setBoardState(const Board& boardState) { this->boardState = boardState; }

private:
	NodeId id;
	NodeId parentId; // Could be removed after debugging is good

	Board boardState;
};

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node() :
	id(INVALID_NODE_ID),
	parentId(INVALID_NODE_ID),
	boardState()
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node(NodeId id, NodeId parentId, const Board& boardState) :
	id(id),
	parentId(parentId),
	boardState(boardState)
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

typedef vector<NodeId> NodesList;
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

	NodeId createNode(
		NodeId parentId,
		const Board& boardState
	);

	void clear();
	bool nodeCreated(NodeId nodeId) const;
	void deleteAllNodes();
	vector<NodeId> treeRootsIds() const;
	bool edgeExists(NodeId parent, NodeId child) const;
	vector<NodeId> backtrack(NodeId from, NodeId to) const;
	NodeId getFirstNodeId() const;

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

void Graph::addEdge(NodeId parentId, NodeId childId) {
	graph[parentId].push_back(childId);
}

//*************************************************************************************************************
//*************************************************************************************************************

NodeId Graph::createNode(
	NodeId parentId,
	const Board& boardState
) {
	NodeId nodeId = nodesCount;

	if (!nodeCreated(nodeId)) {
		int nodeDepth = 0;
		//if (parentId != INVALID_NODE_ID) {
		//	nodeDepth = 1 + idNodeMap[parentId]->getNodeDepth();
		//}

		Node* node = new Node(nodeId, parentId, boardState);
		idNodeMap[nodeId] = node;
		graph[nodeId];

		if (INVALID_NODE_ID != parentId) {
			graph[parentId].push_back(nodeId);
		}
		++nodesCount;
	}

	return nodeId;
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

class GameTree {
public:
	GameTree();

	~GameTree();

	const Board& getTurnBoard() const { return turnBoard; }
	const string& getBestMoves() const { return bestMoves; }

	void setTurnBoard(const Board& turnBoard) { this->turnBoard = turnBoard; }
	void setState(const State& state) { this->state = state; }

	void build();
	void createChildren(NodeId parentId, ChildrenList& children);
	void updateBestChild(NodeId childNodeId, int score);
	void gatherMosves();

private:
	Board turnBoard;
	NodeId bestChild;
	int bestScore;
	string bestMoves;
	State state;

	Graph gameTree;
};

//*************************************************************************************************************
//*************************************************************************************************************

GameTree::GameTree() :
	turnBoard(),
	bestChild(INVALID_NODE_ID),
	bestScore(0),
	bestMoves(EMPTY_STRING),
	state(),
	gameTree()
{

}

//*************************************************************************************************************
//*************************************************************************************************************

GameTree::~GameTree() {

}

//*************************************************************************************************************
//*************************************************************************************************************

void GameTree::build() {
	const NodeId rootId = gameTree.createNode(INVALID_NODE_ID, turnBoard);

	NodeQueue nodesForntier;
	nodesForntier.push_back(rootId);

	while (!nodesForntier.empty()) {
		const NodeId parentId = nodesForntier.front();
		nodesForntier.pop_front();

		ChildrenList children;
		createChildren(parentId, children);

		// This for loop could be jumped over if no reordering of children will happen
	 	for (size_t childIdx = 0; childIdx < children.size(); ++childIdx) {
			nodesForntier.push_back(children[childIdx]);
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GameTree::createChildren(NodeId parentId, ChildrenList& children) {
	Node* parent = gameTree.getNode(parentId);

	const Board& parentBoardState = parent->getBoardState();
	// Find first clear cell
	Coords clearCellCoords;
	parentBoardState.findFirstClearCell(clearCellCoords);

	if (clearCellCoords.isValid()) {
		for (Cell mark : POSSIBLE_CELL_MARKS) {
			if (parentBoardState.isMarkPossible(clearCellCoords, mark)) {
				NodeId childNodeId = gameTree.createNode(parent->getId(), parentBoardState);
				gameTree.getNode(childNodeId)->getBoardState().markCell(clearCellCoords, mark);
				children.push_back(childNodeId);
			}
		}
	}
	else {
		int score = state.simulate(parentBoardState);
		if (score > bestScore) {
			bestScore = score;
			bestChild = parentId;
		}

		state.reset();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GameTree::updateBestChild(NodeId childNodeId, int score) {
	if (score > bestScore) {
		bestScore = score;
		bestChild = childNodeId;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GameTree::gatherMosves() {
	Node* bestNode = gameTree.getNode(bestChild);
	bestNode->getBoardState().getMoves(bestMoves);
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

	Board gameBoard;
	State state;
	GameTree gameTree;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	gameBoard(),
	state(),
	gameTree()
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
	for (int8_t rowIdx = 0; rowIdx < BOARD_HEIGHT; ++rowIdx) {
		string line;
		cin >> line; cin.ignore();

		gameBoard.addRow(rowIdx, line);
	}

	int robotCount;
	cin >> robotCount; cin.ignore();
	state.addRobotsCount(static_cast<int8_t>(robotCount));

	for (int i = 0; i < robotCount; i++) {
		int x;
		int y;
		string direction;
		cin >> x >> y >> direction; cin.ignore();

		state.addRobot(
			static_cast<int8_t>(i),
			static_cast<int8_t>(x),
			static_cast<int8_t>(y),
			static_cast<RobotDirection>(direction[0])
		);
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
	gameTree.setTurnBoard(gameBoard);
	gameTree.setState(state);
	gameTree.build();
	gameTree.gatherMosves();
	cout << gameTree.getBestMoves() << endl;

	//cout << "0 0 U 1 1 R 2 2 D 3 3 L" << endl;
	//cout << "14 2 L 3 7 R 4 7 L 5 7 L 6 7 L 7 7 L 8 7 L 9 7 L 10 7 L 11 7 L 12 7 L" << endl;
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