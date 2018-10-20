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

// First 40 bits are showing each robot movement in that cell
// Next 6 flags are shwoing the type of the cell (void, arrow)
typedef unsigned long long Cell;

namespace Masks {
	static const Cell FLAG	= 0b1000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000;
	static const Cell VOID	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'1000'0000'0000'0000'0000'0000;
	static const Cell EMPTY	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0100'0000'0000'0000'0000'0000;
	static const Cell UP	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0010'0000'0000'0000'0000'0000;
	static const Cell RIGTH	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0001'0000'0000'0000'0000'0000;
	static const Cell DOWN	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'1000'0000'0000'0000'0000;
	static const Cell LEFT	= 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0100'0000'0000'0000'0000;
	static const Cell CLEAR = 0b0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0010'0000'0000'0000'0000;
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
	Coords operator*(Coord numerical);
	Coords& operator*=(Coord numerical);

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
	void setRobotTrace(
		const Coords& position,
		Cell robotDirectionFlag
	);


	bool isMarkPossible(const Coords& clearCellCoords, Cell mark) const;

	/// Returns true if the robot already was there in the same postion
	bool checkRobotDirection(
		const Coords& position,
		Cell robotDirectionFlag
	) const;

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

void Board::setRobotTrace(
	const Coords& position,
	Cell robotDirectionFlag
){
	gameBoard[position.getYCoord()][position.getXCoord()] |= robotDirectionFlag;
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

bool Board::checkRobotDirection(
	const Coords& position,
	Cell robotDirectionFlag
) const {
	return gameBoard[position.getYCoord()][position.getXCoord()] & robotDirectionFlag;
}

//*************************************************************************************************************
//*************************************************************************************************************

Cell Board::getCell(const Coords& coords) const {
	return gameBoard[coords.getYCoord()][coords.getXCoord()];
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

	void setPosition(const Coords& position) { this->position = position; }
	void setDirection(RobotDirection direction) { this->direction = direction; }

	void move();
	void rotate(Cell cell);
	void destroyRobot();

	bool isDead() const;

private:
	Coords position;
	RobotDirection direction;
};

//*************************************************************************************************************
//*************************************************************************************************************

Robot::Robot() :
	position(),
	direction(RobotDirection::INVALID)
{

}
//*************************************************************************************************************
//*************************************************************************************************************

Robot::Robot(const Robot& robot) :
	position(robot.position),
	direction(robot.direction)
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

class State {
public:
	State();
	State(const State& state);
	~State();

	int8_t getRobotsCount() const { return robotsCount; }
	int8_t getFunctioningRobotsCount() const { return functioningRobotsCount; }
	int getScore() const { return score; }
	const string& getString() const { return move; }

	void setRobotsCount(int8_t robotsCount) { this->robotsCount = robotsCount; }
	void setFunctioningRobotsCount(int8_t functioningRobotsCount) { this->functioningRobotsCount = functioningRobotsCount; }
	void setScore(int score) { this->score = score; }
	void setMove(const string& move) { this->move = move; }

	State& operator=(const State& state);

	void addBoardRow(int rowIdx, const string& line);

	void addRobot(
		int8_t xPos,
		int8_t yPos,
		RobotDirection direction
	);

	void findFirstClearCell(Coords& clearCellCoords) const;
	void simulate(const Coords& clearCellCoords, Cell mark);
	void simulateRobots();

	bool isMarkPossible(const Coords& clearCellCoords, Cell mark) const;

	Cell makeRobotDirectionFlag(RobotDirection robotDirection, int8_t robotIdx) const;

	void convertToMove(const Coords& clearCellCoords, Cell mark, string& res) const;

private:
	Board board;
	Robot robots[MAX_ROBOTS_COUNT];
	int8_t robotsCount;
	int8_t functioningRobotsCount;
	int score;
	string move;

	void copyRobots(const State& state);
};

//*************************************************************************************************************
//*************************************************************************************************************

State::State() :
	board(),
	robotsCount(0),
	functioningRobotsCount(0),
	score(0),
	move(EMPTY_STRING)
{
	// Robots will be automatically created with Robot()
}

//*************************************************************************************************************
//*************************************************************************************************************

State::State(const State& state) :
	board(state.board),
	robotsCount(state.robotsCount),
	functioningRobotsCount(state.functioningRobotsCount),
	score(state.score),
	move(state.move)
{
	copyRobots(state);
}

//*************************************************************************************************************
//*************************************************************************************************************

State::~State() {

}

//*************************************************************************************************************
//*************************************************************************************************************

State& State::operator=(const State& state) {
	if (this != &state) {
		board = state.board;
		robotsCount = state.robotsCount;
		functioningRobotsCount = state.functioningRobotsCount;
		score = state.score;
		move = state.move;

		copyRobots(state);
	}
	
	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::addBoardRow(int rowIdx, const string& line) {
	board.addRow(rowIdx, line);
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::addRobot(
	int8_t xPos,
	int8_t yPos,
	RobotDirection direction
) {
	robots[robotsCount].setPosition(Coords(xPos, yPos));
	robots[robotsCount].setDirection(direction);

	++robotsCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::findFirstClearCell(Coords& clearCellCoords) const {
	board.findFirstClearCell(clearCellCoords);
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::simulate(const Coords& clearCellCoords, Cell mark) {
	board.markCell(clearCellCoords, mark);

	simulateRobots();

	convertToMove(clearCellCoords, mark, move);
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::simulateRobots() {
	for (int8_t robotIdx = 0; robotIdx < robotsCount; ++robotIdx) {
		Robot& robot = robots[robotIdx];

		if (!robot.isDead()) {
			++score;

			// mark the current cell of the robot
			board.setRobotTrace(robot.getPosiiton(), makeRobotDirectionFlag(robot.getDirection(), robotIdx));

			robot.move();
			Cell newRobotCell = board.getCell(robot.getPosiiton());

			if (newRobotCell & Masks::VOID) {
				robot.destroyRobot();
				--functioningRobotsCount;
			}
			else {
				robot.rotate(newRobotCell);

				if (board.checkRobotDirection(robot.getPosiiton(), makeRobotDirectionFlag(robot.getDirection(), robotIdx))) {
					robot.destroyRobot();
					--functioningRobotsCount;
				}
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

bool State::isMarkPossible(const Coords& clearCellCoords, Cell mark) const {
	return board.isMarkPossible(clearCellCoords, mark);
}

//*************************************************************************************************************
//*************************************************************************************************************

Cell State::makeRobotDirectionFlag(RobotDirection robotDirection, int8_t robotIdx) const {
	Cell flag = Masks::FLAG;
	flag >>= robotIdx * ROBOT_DIRECTION;

	switch (robotDirection) {
		case RobotDirection::UP: {
			flag >>= 0;
			break;
		}
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

 void State::convertToMove(const Coords & clearCellCoords, Cell mark, string& res) const {
	 res = EMPTY_STRING;

	 if (mark & Masks::EMPTY) {
		return;
	}

	res += to_string(clearCellCoords.getXCoord());
	res += SPACE;
	res += to_string(clearCellCoords.getYCoord());
	res += SPACE;

	if (mark & Masks::UP) {
		res += static_cast<char>(CellType::UP_ARROW);
	}

	if (mark & Masks::RIGTH) {
		res += static_cast<char>(CellType::RIGHT_ARROW);
	}

	if (mark & Masks::DOWN) {
		res += static_cast<char>(CellType::DOWN_ARROW);
	}

	if (mark & Masks::LEFT) {
		res += static_cast<char>(CellType::LEFT_ARROW);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void State::copyRobots(const State& state) {
	for (int8_t robotIdx = 0; robotIdx < state.getRobotsCount(); ++robotIdx) {
		robots[robotIdx] = state.robots[robotIdx];
	}
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
	Node(NodeId id, int nodeDepth, NodeId parentId, const State& state, bool rootNote, bool explored, bool inFrontier);
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

	State* getState() {
		return &state;
	}

	void setId(NodeId id) { this->id = id; }
	void setNodeDepth(int nodeDepth) { this->nodeDepth = nodeDepth; }
	void setParentId(NodeId parentId) { this->parentId = parentId; }
	void setRootNode(bool rootNote) { this->rootNote = rootNote; }
	void setExplored(bool explored) { this->explored = explored; }
	void setInFrontier(bool inFrontier) { this->inFrontier = inFrontier; }
	void setState(const State& state) { this->state = state; }

private:
	NodeId id;
	int nodeDepth;
	NodeId parentId;
	bool rootNote;
	bool explored;
	bool inFrontier;

	State state;
};

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node() :
	id(INVALID_NODE_ID),
	nodeDepth(INVALID_NODE_DEPTH),
	parentId(INVALID_NODE_ID),
	state(),
	rootNote(false),
	explored(false),
	inFrontier(false)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Node::Node(NodeId id, int nodeDepth, NodeId parentId, const State& state, bool rootNote, bool explored, bool inFrontier) :
	id(id),
	nodeDepth(nodeDepth),
	parentId(parentId),
	state(state),
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
		const State& gameState
	);

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

NodeId Graph::createNode(
	NodeId parentId,
	const State& gameState
) {
	NodeId nodeId = nodesCount;

	if (!nodeCreated(nodeId)) {
		int nodeDepth = 0;
		if (parentId != INVALID_NODE_ID) {
			nodeDepth = 1 + idNodeMap[parentId]->getNodeDepth();
		}

		Node* node = new Node(nodeId, nodeDepth, parentId, gameState, false, false, false);
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

	State getTurnState() const { return turnState; }

	void setTurnState(const State& turnState) { this->turnState = turnState; }

	void build();
	void createChildren(NodeId parentId, ChildrenList& children);

private:
	State turnState;

	Graph gameTree;
};

//*************************************************************************************************************
//*************************************************************************************************************

GameTree::GameTree() :
	turnState(),
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
	const NodeId rootId = gameTree.createNode(INVALID_NODE_ID, turnState);

	// Sorted list for the nodes to explore
	// ENDING with the highest score state
	NodesList nodesForntier;
	nodesForntier.push_back(rootId);

	while (!nodesForntier.empty()) {
		const NodeId parentId = nodesForntier.back();
		nodesForntier.pop_back();

		ChildrenList children;
		createChildren(parentId, children);
		// eventually sort children
	 	for (size_t childIdx = 0; childIdx < children.size(); ++childIdx) {
			nodesForntier.push_back(children[childIdx]);
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GameTree::createChildren(NodeId parentId, ChildrenList& children) {
	Node* parent = gameTree.getNode(parentId);

	State* parentState = parent->getState();
	// Find first clear cell
	Coords clearCellCoords;
	parentState->findFirstClearCell(clearCellCoords);

	if (parentState->getFunctioningRobotsCount() > 0) {
		if (clearCellCoords.isValid()) {
			// Generate all possible states based on the parent state
			for (Cell mark : POSSIBLE_CELL_MARKS) {
				if (parentState->isMarkPossible(clearCellCoords, mark)) {
					// First create the node
					NodeId childNodeId = gameTree.createNode(parent->getId(), *parentState);

					// Then simulate from the tree directly, to make one less copy
					gameTree.getNode(childNodeId)->getState()->simulate(clearCellCoords, mark);
					children.push_back(childNodeId);
				}
			}
		}
		else {
			NodeId childNodeId = gameTree.createNode(parent->getId(), *parentState);
			// Then simulate from the tree directly, to make one less copy
			gameTree.getNode(childNodeId)->getState()->simulateRobots();
			children.push_back(childNodeId);
		}
	}
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
	GameTree gameTree;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	gameState(),
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

		gameState.addBoardRow(rowIdx, line);
	}

	int robotCount;
	cin >> robotCount; cin.ignore();
	gameState.setFunctioningRobotsCount(robotCount);

	for (int i = 0; i < robotCount; i++) {
		int x;
		int y;
		string direction;
		cin >> x >> y >> direction; cin.ignore();

		gameState.addRobot(
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
	gameTree.setTurnState(gameState);
	gameTree.build();

	//cout << "0 0 U 1 1 R 2 2 D 3 3 L" << endl;
	//cout << "3 4 R 4 4 R 5 4 L" << endl;
	cout << "0 0 U" << endl;
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