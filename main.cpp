
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <deque>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <climits>
#include <iostream>
using namespace std;

const int TOTAL_ROWS = 20;
const int TOTAL_COLUMNS = 30;
const int CELL_SIZE_PIXELS = 30;
const int HEADS_UP_DISPLAY_HEIGHT = 130;
const int WINDOW_WIDTH_PIXELS = TOTAL_COLUMNS * CELL_SIZE_PIXELS;
const int WINDOW_HEIGHT_PIXELS = (TOTAL_ROWS * CELL_SIZE_PIXELS) + HEADS_UP_DISPLAY_HEIGHT;

const float AUTOMATIC_MOVE_INTERVAL = 0.09f;
const float MANUAL_MOVE_INTERVAL = 0.12f;
const int MAXIMUM_TRAIL_LENGTH = 60;

const sf::Color backgroundColor(18, 22, 30);
const sf::Color emptyCellColor(30, 38, 52);
const sf::Color obstacleColor(160, 45, 35);
const sf::Color obstacleShineColor(210, 80, 60);
const sf::Color targetColor(40, 130, 220);
const sf::Color targetGlowColor(80, 170, 255);
const sf::Color trailColor(80, 200, 230, 120);
const sf::Color hudBackgroundColor(12, 16, 24);
const sf::Color hudBorderColor(55, 75, 110);
const sf::Color mainTextColor(200, 220, 255);
const sf::Color dimTextColor(80, 110, 160);
const sf::Color safeColor(60, 200, 100);
const sf::Color warningColor(240, 180, 40);
const sf::Color dangerColor(230, 60, 60);
const sf::Color accentColor(100, 180, 255);

template <typename T>
class MyCoordinate 
{
private:
    T rowValue;
    T columnValue;

public:
    MyCoordinate()
    {
        this->rowValue = 0;
        this->columnValue = 0;
    }
    MyCoordinate(T r, T c) 
    {
        this->rowValue = r;
        this->columnValue = c;
    }
    T getRow() const 
    {
        return this->rowValue;
    }
    T getColumn() const
    {
        return this->columnValue;
    }
    void setRow(T r) {
        this->rowValue = r;
    }
    void setColumn(T c) 
    {
        this->columnValue = c;
    }
    bool operator==(const MyCoordinate<T>& other) const 
    {
        if (this->rowValue == other.rowValue && this->columnValue == other.columnValue) 
        {
            return true;
        } 
        else 
        {
            return false;
        }
    }
};

typedef MyCoordinate<int> Point2D;

template <typename T>
T findMaximum(T firstValue, T secondValue) 
{
    if (firstValue > secondValue) 
    {
        return firstValue;
    } 
    else
    {
        return secondValue;
    }
}

template <typename T>
T findMinimum(T firstValue, T secondValue)
 {
    if (firstValue < secondValue) 
    {
        return firstValue;
    } 
    else 
    {
        return secondValue;
    }
}
sf::Color interpolateColor(sf::Color firstColor, sf::Color secondColor, float interpolationFactor) 
{
    interpolationFactor = findMaximum<float>(0.0f, findMinimum<float>(1.0f, interpolationFactor));
    uint8_t newRed = (uint8_t)(firstColor.r + (secondColor.r - firstColor.r) * interpolationFactor);
    uint8_t newGreen = (uint8_t)(firstColor.g + (secondColor.g - firstColor.g) * interpolationFactor);
    uint8_t newBlue = (uint8_t)(firstColor.b + (secondColor.b - firstColor.b) * interpolationFactor);
    uint8_t newAlpha = (uint8_t)(firstColor.a + (secondColor.a - firstColor.a) * interpolationFactor);
    return sf::Color(newRed, newGreen, newBlue, newAlpha);
}
string addPaddingLeft(const string& originalString, int requiredWidth, char paddingChar = '0') 
{
    int currentLength = (int)originalString.size();
    if (currentLength >= requiredWidth)
    {
        return originalString;
    }
    int amountToPad = requiredWidth - currentLength;
    string paddedString = "";
    for (int i = 0; i < amountToPad; i++) 
    {
        paddedString = paddedString + paddingChar;
    }
    paddedString = paddedString + originalString;
    return paddedString;
}

class SimulationMap 
{
private:
    vector<vector<int>> mapGrid;
    int randomSeed;

public:
    SimulationMap() 
    {
        cout << "SimulationMap Object Created!" << endl;
        mapGrid.resize(TOTAL_ROWS);
        for (int i = 0; i < TOTAL_ROWS; i++) 
        {
            mapGrid[i].resize(TOTAL_COLUMNS);
            for (int j = 0; j < TOTAL_COLUMNS; j++) 
            {
                mapGrid[i][j] = 0;
            }
        }
        randomSeed = (int)time(nullptr);
        generateObstacles();
    }
    ~SimulationMap()
    {
        cout << "SimulationMap Object Destroyed!" << endl;
    }
    void generateObstacles() 
    {
        cout << "Generating new map with obstacles..." << endl;
        srand(randomSeed);
        for (int i = 0; i < TOTAL_ROWS; i++) 
        {
            for (int j = 0; j < TOTAL_COLUMNS; j++)
             {
                int randomNumber = rand() % 6;
                if (randomNumber == 0) {
                    mapGrid[i][j] = 1;
                } 
                else 
                {
                    mapGrid[i][j] = 0; 
                }
            }
        }
        mapGrid[0][0] = 0; 
        mapGrid[0][1] = 0; 
        mapGrid[1][0] = 0;
        mapGrid[TOTAL_ROWS - 1][TOTAL_COLUMNS - 1] = 0;
        mapGrid[TOTAL_ROWS - 1][TOTAL_COLUMNS - 2] = 0;
        mapGrid[TOTAL_ROWS - 2][TOTAL_COLUMNS - 1] = 0;
    }
    void resetMap()
     {
        randomSeed = rand(); 
        generateObstacles();
    }
    bool isValidCell(int testRow, int testColumn) const 
    {
        bool isRowValid = (testRow >= 0) && (testRow < TOTAL_ROWS);
        bool isColumnValid = (testColumn >= 0) && (testColumn < TOTAL_COLUMNS);
        if (isRowValid && isColumnValid) 
        {
            if (mapGrid[testRow][testColumn] == 0) 
            {
                return true;
            } 
        else 
        {
                return false;
        }
        } 
        else 
        {
            return false;
        }
    }
    vector<vector<int>> getGrid() const 
    {
        return mapGrid;
    }
};
class AStarNode 
{
private:
    int rowPosition;
    int columnPosition;
    int costFromStart; 
    int totalEstimatedCost;
    AStarNode* parentNodePointer;
public:
    AStarNode(int r, int c, int gCost, int fCost, AStarNode* parent) 
    {
        this->rowPosition = r;
        this->columnPosition = c;
        this->costFromStart = gCost;
        this->totalEstimatedCost = fCost;
        this->parentNodePointer = parent;
    }
    int getRowPosition() const { return this->rowPosition; }
    int getColumnPosition() const { return this->columnPosition; }
    int getCostFromStart() const { return this->costFromStart; }
    int getTotalEstimatedCost() const { return this->totalEstimatedCost; }
    AStarNode* getParentNodePointer() const { return this->parentNodePointer; }
};
class NodeComparator 
{
public:
    bool operator()(const AStarNode* firstNode, const AStarNode* secondNode) const 
    {
        return firstNode->getTotalEstimatedCost() > secondNode->getTotalEstimatedCost();
    }
};
class PathFindingSystem 
{
public:
    //to calculate Manhattan Distance Heuristic
    static int calculateHeuristic(int currentRow, int currentColumn, int targetRow, int targetColumn) 
    {
        int verticalDifference = abs(currentRow - targetRow);
        int horizontalDifference = abs(currentColumn - targetColumn);
        return verticalDifference + horizontalDifference;
    }
    static vector<Point2D> calculateAStarPath(const SimulationMap& currentMap, Point2D startPoint, Point2D targetPoint) 
    {
        cout << "Calculating A* Path..." << endl;
        vector<vector<int>> distanceToCell;
        distanceToCell.resize(TOTAL_ROWS);
        for (int i = 0; i < TOTAL_ROWS; i++) 
        {
            distanceToCell[i].resize(TOTAL_COLUMNS);
            for (int j = 0; j < TOTAL_COLUMNS; j++)
             {
                distanceToCell[i][j] = INT_MAX;
            }
        }
        priority_queue<AStarNode*, vector<AStarNode*>, NodeComparator> explorationQueue;
        int initialHeuristic = calculateHeuristic(startPoint.getRow(), startPoint.getColumn(), targetPoint.getRow(), targetPoint.getColumn());
        AStarNode* startNode = new AStarNode(startPoint.getRow(), startPoint.getColumn(), 0, initialHeuristic, nullptr);
        explorationQueue.push(startNode);
        distanceToCell[startPoint.getRow()][startPoint.getColumn()] = 0;
        int deltaRow[4] = { 1, -1, 0, 0 };
        int deltaColumn[4] = { 0, 0, 1, -1 };
        AStarNode* destinationNode = nullptr;
        while (explorationQueue.empty() == false)
         {
            AStarNode* currentNode = explorationQueue.top();
            explorationQueue.pop();
            int currentRow = currentNode->getRowPosition();
            int currentColumn = currentNode->getColumnPosition();
            if (currentRow == targetPoint.getRow() && currentColumn == targetPoint.getColumn()) 
            {
                destinationNode = currentNode;
                cout << "Path found to target!" << endl;
                break;
            }
            if (currentNode->getCostFromStart() > distanceToCell[currentRow][currentColumn])
            {
                continue;
            }
            for (int i = 0; i < 4; i++)
             {
                int neighborRow = currentRow + deltaRow[i];
                int neighborColumn = currentColumn + deltaColumn[i];
                if (currentMap.isValidCell(neighborRow, neighborColumn) == true) 
                {
                    int newCostFromStart = currentNode->getCostFromStart() + 1;
                    if (newCostFromStart < distanceToCell[neighborRow][neighborColumn]) 
                    {
                        distanceToCell[neighborRow][neighborColumn] = newCostFromStart;
                        int neighborHeuristic = calculateHeuristic(neighborRow, neighborColumn, targetPoint.getRow(), targetPoint.getColumn());
                        int neighborTotalCost = newCostFromStart + neighborHeuristic;
                        AStarNode* newNeighborNode = new AStarNode(neighborRow, neighborColumn, newCostFromStart, neighborTotalCost, currentNode);
                        explorationQueue.push(newNeighborNode);
                    }
                }
            }
        }
        if (destinationNode == nullptr)
        {
            cout << "Error: No valid path found to target." << endl;
            vector<Point2D> emptyPath;
            return emptyPath;
        }
        vector<Point2D> finalPath;
        AStarNode* backtrackingNode = destinationNode;
        while (backtrackingNode != nullptr) 
        {
            Point2D currentPoint(backtrackingNode->getRowPosition(), backtrackingNode->getColumnPosition());
            finalPath.push_back(currentPoint);
            backtrackingNode = backtrackingNode->getParentNodePointer();
        }
        reverse(finalPath.begin(), finalPath.end());
        return finalPath;
    }
    static int findDistanceToNearestObstacle(const SimulationMap& currentMap, int roverRow, int roverColumn) 
    {
        int minimumDistanceFound = INT_MAX;
        vector<vector<int>> gridData = currentMap.getGrid();
        for (int i = 0; i < TOTAL_ROWS; i++) {
            for (int j = 0; j < TOTAL_COLUMNS; j++) 
            {
                if (gridData[i][j] == 1) 
                {
                    int currentDistance = abs(roverRow - i) + abs(roverColumn - j);
                    if (currentDistance < minimumDistanceFound)
                     {
                        minimumDistanceFound = currentDistance;
                    }
                }
            }
        }
        if (minimumDistanceFound == INT_MAX) 
        {
            return 99;
        } 
        else 
        {
            return minimumDistanceFound;
        }
    }
};
enum class RoverState
{ 
    WAITING_IDLE, 
    MANUALLY_EXPLORING, 
    AUTOMATIC_NAVIGATING, 
    SUCCESSFULLY_ARRIVED, 
    PATH_IS_STUCK 
};
class MarsRoverVehicle 
{
private:
    int currentRowPosition;
    int currentColumnPosition;
    float drawingRowPosition;
    float drawingColumnPosition;
    vector<Point2D> calculatedPath;
    int currentPathStepIndex;
    deque<Point2D> movementTrail;
    int distanceToNearestObstacle;
    RoverState currentStatus;
    bool hasReachedTargetGoal;
    int totalNumberOfMovesMade;

public:
    MarsRoverVehicle() 
    {
        cout << "MarsRoverVehicle Initialized!" << endl;
        this->currentRowPosition = 0;
        this->currentColumnPosition = 0;
        this->drawingRowPosition = 0.0f;
        this->drawingColumnPosition = 0.0f;
        this->currentPathStepIndex = 0;
        this->distanceToNearestObstacle = 99;
        this->currentStatus = RoverState::WAITING_IDLE;
        this->hasReachedTargetGoal = false;
        this->totalNumberOfMovesMade = 0;
    }
    void assignNewPath(vector<Point2D> newPath) 
    {
        this->calculatedPath = newPath;
        this->currentPathStepIndex = 0;
        if (newPath.empty() == true) 
        {
            this->currentStatus = RoverState::PATH_IS_STUCK;
        } 
        else
        {
            this->currentStatus = RoverState::AUTOMATIC_NAVIGATING;
        }
        
        this->hasReachedTargetGoal = false;
    }
    void resetRoverToStart() 
    {
        this->currentRowPosition = 0;
        this->currentColumnPosition = 0;
        this->drawingRowPosition = 0.0f;
        this->drawingColumnPosition = 0.0f;
        this->calculatedPath.clear();
        this->movementTrail.clear();
        this->currentPathStepIndex = 0;
        this->currentStatus = RoverState::WAITING_IDLE;
        this->hasReachedTargetGoal = false;
        this->totalNumberOfMovesMade = 0;
    }
    void moveManually(int deltaRow, int deltaColumn, const SimulationMap& currentMap) 
    {
        int predictedRow = this->currentRowPosition + deltaRow;
        int predictedColumn = this->currentColumnPosition + deltaColumn;
        if (currentMap.isValidCell(predictedRow, predictedColumn) == true) 
        {
            addCurrentPositionToTrail();
            this->currentRowPosition = predictedRow;
            this->currentColumnPosition = predictedColumn;
            this->totalNumberOfMovesMade++;
            this->currentStatus = RoverState::MANUALLY_EXPLORING;
        }
    }
    bool executeNextAutomaticStep() 
    {
        int totalPathLength = (int)this->calculatedPath.size();
        if (this->currentPathStepIndex < totalPathLength)
         {
            addCurrentPositionToTrail();
            Point2D nextPoint = this->calculatedPath[this->currentPathStepIndex];
            this->currentRowPosition = nextPoint.getRow();
            this->currentColumnPosition = nextPoint.getColumn();
            this->currentPathStepIndex++;
            this->totalNumberOfMovesMade++;
            if (this->currentRowPosition == TOTAL_ROWS - 1 && this->currentColumnPosition == TOTAL_COLUMNS - 1) 
            {
                this->currentStatus = RoverState::SUCCESSFULLY_ARRIVED;
                this->hasReachedTargetGoal = true;
            }
            return true;
        }
        
        if (this->currentStatus != RoverState::SUCCESSFULLY_ARRIVED) 
        {
            this->currentStatus = RoverState::SUCCESSFULLY_ARRIVED;
        }
        
        return false;
    }

    // Update drawing coordinates for smooth animation
    void updateAnimationCoordinates(float deltaTime) {
        float animationSpeed = 10.0f; // Multiplier for how fast it animates
        
        // Linear interpolation formula: current = current + (target - current) * speed * time
        this->drawingRowPosition = this->drawingRowPosition + ((float)this->currentRowPosition - this->drawingRowPosition) * animationSpeed * deltaTime;
        
        this->drawingColumnPosition = this->drawingColumnPosition + ((float)this->currentColumnPosition - this->drawingColumnPosition) * animationSpeed * deltaTime;
    }

    // Get color of rover based on obstacle proximity
    sf::Color getRoverDisplayColor() const {
        if (this->distanceToNearestObstacle <= 1) {
            return dangerColor; // Very close to obstacle
        } else if (this->distanceToNearestObstacle <= 3) {
            return warningColor; // Getting close to obstacle
        } else {
            return safeColor; // Far from obstacles
        }
    }

    // Convert status to string for display on HUD
    string getStatusAsString() const {
        if (this->currentStatus == RoverState::WAITING_IDLE) {
            return "IDLE MODE";
        } else if (this->currentStatus == RoverState::MANUALLY_EXPLORING) {
            return "MANUAL EXPLORATION";
        } else if (this->currentStatus == RoverState::AUTOMATIC_NAVIGATING) {
            return "AUTO NAVIGATING";
        } else if (this->currentStatus == RoverState::SUCCESSFULLY_ARRIVED) {
            return "TARGET REACHED";
        } else if (this->currentStatus == RoverState::PATH_IS_STUCK) {
            return "PATH IS BLOCKED";
        } else {
            return "UNKNOWN STATUS";
        }
    }

    // Private helper method to record trail
private:
    void addCurrentPositionToTrail() {
        Point2D currentPos(this->currentRowPosition, this->currentColumnPosition);
        
        // Push to back of queue
        this->movementTrail.push_back(currentPos);
        
        // If queue is too long, remove the oldest one from the front
        if ((int)this->movementTrail.size() > MAXIMUM_TRAIL_LENGTH) {
            this->movementTrail.pop_front();
        }
    }

    // Adding public getters for variables needed by main
public:
    int getCurrentRow() const { return currentRowPosition; }
    int getCurrentColumn() const { return currentColumnPosition; }
    float getDrawingRow() const { return drawingRowPosition; }
    float getDrawingColumn() const { return drawingColumnPosition; }
    vector<Point2D> getCalculatedPath() const { return calculatedPath; }
    int getCurrentPathStep() const { return currentPathStepIndex; }
    deque<Point2D> getMovementTrail() const { return movementTrail; }
    
    int getObstacleDistance() const { return distanceToNearestObstacle; }
    void setObstacleDistance(int dist) { distanceToNearestObstacle = dist; }
    
    RoverState getCurrentStatus() const { return currentStatus; }
    void setCurrentStatus(RoverState state) { currentStatus = state; }
    
    bool getHasReachedGoal() const { return hasReachedTargetGoal; }
    void setHasReachedGoal(bool reached) { hasReachedTargetGoal = reached; }
    
    int getTotalMoves() const { return totalNumberOfMovesMade; }
};
sf::Text createTextObject(const sf::Font& fontRef, const string& textContent,unsigned int characterSize, sf::Color fontColor,sf::Vector2f textPosition,sf::Text::Style textStyle = sf::Text::Regular) 
{
    sf::Text newText(fontRef, textContent, characterSize);
    newText.setFillColor(fontColor);
    newText.setPosition(textPosition);
    newText.setStyle(textStyle);
    return newText;
}
class UserInterfaceHUD 
{
private:
    const sf::Font& referenceToFont;
    sf::Color determineStatusColor(RoverState state) const 
    {
        if (state == RoverState::WAITING_IDLE) 
        {
            return dimTextColor;
        } 
        else if (state == RoverState::MANUALLY_EXPLORING) 
        {
            return accentColor;
        } 
        else if (state == RoverState::AUTOMATIC_NAVIGATING) 
        {
            return safeColor;
        } 
        else if (state == RoverState::SUCCESSFULLY_ARRIVED) 
        {
            return targetGlowColor;
        } 
        else if (state == RoverState::PATH_IS_STUCK) 
        {
            return dangerColor;
        } 
        else 
        {
            return mainTextColor;
        }
    }
    void drawInformationLabel(sf::RenderWindow& targetWindow,const string& titleKey, const string& titleValue,float xPos, float yPos,sf::Color valueColor = mainTextColor) const 
    {
        sf::Text titleText = createTextObject(referenceToFont, titleKey, 10, dimTextColor, {xPos, yPos});
        targetWindow.draw(titleText);
        sf::Text valueText = createTextObject(referenceToFont, titleValue, 15, valueColor, {xPos, yPos + 13}, sf::Text::Bold);
        targetWindow.draw(valueText);
    }

public:
    explicit UserInterfaceHUD(const sf::Font& fontObject) : referenceToFont(fontObject) 
    {}
    void renderHUD(sf::RenderWindow& gameWindow, const MarsRoverVehicle& currentRover,bool isAutoModeEnabled, bool isSimulationPaused, float totalGameTime) const 
    {

        float yPositionStart = (float)(TOTAL_ROWS * CELL_SIZE_PIXELS);
        sf::RectangleShape backgroundRect;
        backgroundRect.setSize(sf::Vector2f((float)WINDOW_WIDTH_PIXELS, (float)HEADS_UP_DISPLAY_HEIGHT));
        backgroundRect.setPosition(sf::Vector2f(0.0f, yPositionStart));
        backgroundRect.setFillColor(hudBackgroundColor);
        gameWindow.draw(backgroundRect);
        sf::RectangleShape borderLine;
        borderLine.setSize(sf::Vector2f((float)WINDOW_WIDTH_PIXELS, 2.0f));
        borderLine.setPosition(sf::Vector2f(0.0f, yPositionStart));
        borderLine.setFillColor(hudBorderColor);
        gameWindow.draw(borderLine);
        float yBasePosition = yPositionStart + 10.0f;
        float column1X = 14.0f;
        float column2X = 220.0f;
        float column3X = 440.0f;
        float column4X = 660.0f;

        sf::Text titleText = createTextObject(referenceToFont, "MARS ROVER CONTROL SYSTEM", 13, accentColor, {column1X, yBasePosition}, sf::Text::Bold);
        gameWindow.draw(titleText);

        if (isSimulationPaused == true) 
        {
            sf::Text pausedText = createTextObject(referenceToFont, "[ PAUSED ]", 13, warningColor, {(float)WINDOW_WIDTH_PIXELS - 115.0f, yBasePosition}, sf::Text::Bold);
            gameWindow.draw(pausedText);
        }

        float row1Y = yBasePosition + 24.0f;
        
        string modeString;
        sf::Color modeColor;
        if (isAutoModeEnabled == true) 
        {
            modeString = "AUTO / A* ALGORITHM";
            modeColor = accentColor;
        } 
        else 
        {
            modeString = "MANUAL CONTROL";
            modeColor = mainTextColor;
        }
        
        drawInformationLabel(gameWindow, "CURRENT MODE", modeString, column1X, row1Y, modeColor);
        drawInformationLabel(gameWindow, "ROVER STATUS", currentRover.getStatusAsString(), column2X, row1Y, determineStatusColor(currentRover.getCurrentStatus()));
        string coordString = "(";
        coordString += to_string(currentRover.getCurrentRow());
        coordString += ", ";
        coordString += to_string(currentRover.getCurrentColumn());
        coordString += ")";
        drawInformationLabel(gameWindow, "GPS POSITION", coordString, column3X, row1Y);
        int currentDistance = currentRover.getObstacleDistance();
        string distanceString;
        sf::Color distanceColor;
        
        if (currentDistance >= 99) 
        {
            distanceString = "ALL CLEAR";
            distanceColor = safeColor;
        } 
        else
         {
            distanceString = to_string(currentDistance) + " cells";
            if (currentDistance <= 1) 
            {
                distanceColor = dangerColor;
            } 
            else if (currentDistance <= 3) 
            {
                distanceColor = warningColor;
            } 
            else
             {
                distanceColor = safeColor;
            }
        }
        drawInformationLabel(gameWindow, "OBSTACLE PROXIMITY", distanceString, column4X, row1Y, distanceColor);

        float row2Y = yBasePosition + 52.0f;
        
        string pathLengthString;
        if (currentRover.getCalculatedPath().empty() == true) 
        {
            pathLengthString = "-";
        } 
        else 
        {
            pathLengthString = to_string((int)currentRover.getCalculatedPath().size());
        }
        drawInformationLabel(gameWindow, "TOTAL PATH LENGTH", pathLengthString, column1X, row2Y);
        
        string currentStepString;
        if (currentRover.getCalculatedPath().empty() == true) 
        {
            currentStepString = "-";
        } 
        else 
        {
            currentStepString = to_string(currentRover.getCurrentPathStep()) + " / " + to_string((int)currentRover.getCalculatedPath().size());
        }
        drawInformationLabel(gameWindow, "CURRENT STEP", currentStepString, column2X, row2Y);
        
        drawInformationLabel(gameWindow, "TOTAL MOVES TAKEN", to_string(currentRover.getTotalMoves()), column3X, row2Y);
        
        int totalSeconds = (int)totalGameTime;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        string timeFormatted = addPaddingLeft(to_string(minutes), 2) + ":" + addPaddingLeft(to_string(seconds), 2);
        drawInformationLabel(gameWindow, "MISSION TIME", timeFormatted, column4X, row2Y);

        float barXPosition = column1X;
        float barYPosition = row2Y + 30.0f;
        float maximumBarWidth = WINDOW_WIDTH_PIXELS - 28.0f;
        float barHeight = 8.0f;

        sf::RectangleShape backgroundBar;
        backgroundBar.setSize(sf::Vector2f(maximumBarWidth, barHeight));
        backgroundBar.setPosition(sf::Vector2f(barXPosition, barYPosition));
        backgroundBar.setFillColor(sf::Color(40, 50, 70));
        gameWindow.draw(backgroundBar);

        int dist = currentRover.getObstacleDistance();
        float fractionFilled;
        
        float distanceRatio = (float)dist / 10.0f;
        if (distanceRatio > 1.0f) 
        {
            fractionFilled = 1.0f - 1.0f;
        } 
        else 
        {
            fractionFilled = 1.0f - distanceRatio;
        }
        sf::Color barColor;
        if (dist <= 1) 
        {
            barColor = dangerColor;
        } 
        else if (dist <= 3) 
        {
            barColor = warningColor;
        } 
        else 
        {
            barColor = safeColor;
        }
        
        sf::RectangleShape filledBar;
        filledBar.setSize(sf::Vector2f(maximumBarWidth * fractionFilled, barHeight));
        filledBar.setPosition(sf::Vector2f(barXPosition, barYPosition));
        filledBar.setFillColor(barColor);
        gameWindow.draw(filledBar);
        sf::Text sensorText = createTextObject(referenceToFont, "PROXIMITY SENSOR VISUALIZATION", 10, dimTextColor, {barXPosition, barYPosition - 13.0f});
        gameWindow.draw(sensorText);
        float controlsY = yBasePosition + 95.0f;
        string controlsTextStr = "[ARROW KEYS] Move Manually   [M] Toggle Auto/Manual Mode   [P] Pause Simulation   [R] Reset Environment";
        sf::Text controlsText = createTextObject(referenceToFont, controlsTextStr, 11, dimTextColor, {column1X, controlsY});
        gameWindow.draw(controlsText);
        sf::Text legendTitle = createTextObject(referenceToFont, "Rover Health Color:", 11, dimTextColor, {(float)WINDOW_WIDTH_PIXELS - 290.0f, controlsY});
        gameWindow.draw(legendTitle);
        float currentXForDots = WINDOW_WIDTH_PIXELS - 225.0f;
        
        sf::CircleShape safeDot(5.0f);
        safeDot.setFillColor(safeColor);
        safeDot.setPosition(sf::Vector2f(currentXForDots, controlsY - 5.0f));
        gameWindow.draw(safeDot);
        sf::Text safeText = createTextObject(referenceToFont, "Safe", 11, dimTextColor, {currentXForDots + 14.0f, controlsY - 8.0f});
        gameWindow.draw(safeText);
        
        currentXForDots += 47.0f;
        sf::CircleShape warnDot(5.0f);
        warnDot.setFillColor(warningColor);
        warnDot.setPosition(sf::Vector2f(currentXForDots, controlsY - 5.0f));
        gameWindow.draw(warnDot);
        sf::Text warnText = createTextObject(referenceToFont, "Warning", 11, dimTextColor, {currentXForDots + 14.0f, controlsY - 8.0f});
        gameWindow.draw(warnText);
        
        currentXForDots += 48.0f;
        sf::CircleShape dangerDot(5.0f);
        dangerDot.setFillColor(dangerColor);
        dangerDot.setPosition(sf::Vector2f(currentXForDots, controlsY - 5.0f));
        gameWindow.draw(dangerDot);
        sf::Text dangerText = createTextObject(referenceToFont, "Danger", 11, dimTextColor, {currentXForDots + 14.0f, controlsY - 8.0f});
        gameWindow.draw(dangerText);
    }
};
int main() 
{
    cout << "WELCOME TO MARS ROVER SIMULATION" << endl;
    cout << "Loading system components..." << endl;
    srand((unsigned int)time(nullptr));
    sf::VideoMode screenMode({(unsigned int)WINDOW_WIDTH_PIXELS, (unsigned int)WINDOW_HEIGHT_PIXELS});
    sf::RenderWindow applicationWindow(screenMode, "Mars Rover Navigation Simulation System");
    applicationWindow.setFramerateLimit(60);

    sf::Font standardFont;
    bool isFontLoadedSuccessfully = false;
    if (standardFont.openFromFile("arial.ttf")) 
    {
        isFontLoadedSuccessfully = true;
    } 
    else if (standardFont.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) 
    {
        isFontLoadedSuccessfully = true;
    } 
    else if (standardFont.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf")) 
    {
        isFontLoadedSuccessfully = true;
    } 
    else if (standardFont.openFromFile("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf")) 
    {
        isFontLoadedSuccessfully = true;
    } 
    else if (standardFont.openFromFile("C:/Windows/Fonts/arial.ttf")) 
    {
        isFontLoadedSuccessfully = true;
    }
    if (isFontLoadedSuccessfully == false) 
    {
        cout << "WARNING: Could not load any font. Text will not be visible on screen!" << endl;
    } 
    else
    {
        cout << "Font loaded successfully!" << endl;
    }
    SimulationMap theSimulationMap;
    MarsRoverVehicle theRover;
    UserInterfaceHUD theHUD(standardFont);
    bool isAutomaticModeActive = false;
    bool isSimulationPaused = false;
    sf::Clock mainGameClock;
    sf::Clock movementTimerClock;
    float totalElapsedGameTime = 0.0f;
    bool triggerArrivalFlash = false;
    float timeSinceFlashStarted = 0.0f;
    const float MAXIMUM_FLASH_DURATION_SECONDS = 1.8f;
    auto recalculatePathUsingAStar = [&]() {
        cout << "Recalculating path..." << endl;
        Point2D startPosition(theRover.getCurrentRow(), theRover.getCurrentColumn());
        Point2D targetPosition(TOTAL_ROWS - 1, TOTAL_COLUMNS - 1);
        
        vector<Point2D> newCalculatedPath = PathFindingSystem::calculateAStarPath(theSimulationMap, startPosition, targetPosition);
        theRover.assignNewPath(newCalculatedPath);
    };
    cout << "System initialized. Starting main loop." << endl;
    while (applicationWindow.isOpen() == true)
     {
        float deltaTimeSeconds = mainGameClock.restart().asSeconds();
        if (isSimulationPaused == false) {
            totalElapsedGameTime = totalElapsedGameTime + deltaTimeSeconds;
        }
        while (auto eventOptional = applicationWindow.pollEvent()) 
        {
            auto& currentEvent = *eventOptional;

            // Handle window close event
            if (currentEvent.is<sf::Event::Closed>()) {
                cout << "Close signal received. Shutting down." << endl;
                applicationWindow.close();
            }
            if (auto* keyPressEvent = currentEvent.getIf<sf::Event::KeyPressed>())
             {

                if (keyPressEvent->code == sf::Keyboard::Key::M)
                 {
                    if (isAutomaticModeActive == true) 
                    {
                        isAutomaticModeActive = false;
                        theRover.setCurrentStatus(RoverState::MANUALLY_EXPLORING);
                        cout << "Switched to MANUAL mode." << endl;
                    } 
                    else 
                    {
                        isAutomaticModeActive = true;
                        recalculatePathUsingAStar();
                        cout << "Switched to AUTOMATIC mode." << endl;
                    }
                }
                else if (keyPressEvent->code == sf::Keyboard::Key::R)
                 {
                    cout << "Resetting simulation map and rover." << endl;
                    theSimulationMap.resetMap();
                    theRover.resetRoverToStart();
                    isAutomaticModeActive = false;
                    isSimulationPaused = false;
                    triggerArrivalFlash = false;
                    totalElapsedGameTime = 0.0f;
                }
  
                else if (keyPressEvent->code == sf::Keyboard::Key::P) 
                {
                    if (isSimulationPaused == true)
                     {
                        isSimulationPaused = false;
                        cout << "Simulation UNPAUSED." << endl;
                    } 
                    else
                     {
                        isSimulationPaused = true;
                        cout << "Simulation PAUSED." << endl;
                    }
                }
            }
        }
        if (isSimulationPaused == false) 
        {
            float currentMoveInterval;
            if (isAutomaticModeActive == true) 
            {
                currentMoveInterval = AUTOMATIC_MOVE_INTERVAL;
            } 
            else 
            {
                currentMoveInterval = MANUAL_MOVE_INTERVAL;
            }
            if (movementTimerClock.getElapsedTime().asSeconds() >= currentMoveInterval) 
            {
                movementTimerClock.restart();

                if (isAutomaticModeActive == true) 
                {
                    bool isStillMoving = theRover.executeNextAutomaticStep();
                    if (isStillMoving == false && theRover.getHasReachedGoal() == true) 
                    {
                        triggerArrivalFlash = true;
                        timeSinceFlashStarted = 0.0f;
                    }
                } 
                else
                 {
                    using Key = sf::Keyboard::Key;
                    
                    if (sf::Keyboard::isKeyPressed(Key::Up)) 
                    {
                        theRover.moveManually(-1, 0, theSimulationMap);
                    }
                    if (sf::Keyboard::isKeyPressed(Key::Down)) 
                    {
                        theRover.moveManually(1, 0, theSimulationMap);
                    }
                    if (sf::Keyboard::isKeyPressed(Key::Left)) 
                    {
                        theRover.moveManually(0, -1, theSimulationMap);
                    }
                    if (sf::Keyboard::isKeyPressed(Key::Right)) 
                    {
                        theRover.moveManually(0, 1, theSimulationMap);
                    }
                    bool isAtTargetRow = (theRover.getCurrentRow() == TOTAL_ROWS - 1);
                    bool isAtTargetColumn = (theRover.getCurrentColumn() == TOTAL_COLUMNS - 1);
                    if (isAtTargetRow && isAtTargetColumn && theRover.getHasReachedGoal() == false) 
                    {
                        cout << "MANUAL SUCCESS: Reached target!" << endl;
                        theRover.setCurrentStatus(RoverState::SUCCESSFULLY_ARRIVED);
                        theRover.setHasReachedGoal(true);
                        triggerArrivalFlash = true;
                        timeSinceFlashStarted = 0.0f;
                    }
                }
            }
            int distToObs = PathFindingSystem::findDistanceToNearestObstacle(theSimulationMap, theRover.getCurrentRow(), theRover.getCurrentColumn());
            theRover.setObstacleDistance(distToObs);
            theRover.updateAnimationCoordinates(deltaTimeSeconds);
            if (triggerArrivalFlash == true) {
                timeSinceFlashStarted = timeSinceFlashStarted + deltaTimeSeconds;
                if (timeSinceFlashStarted > MAXIMUM_FLASH_DURATION_SECONDS) {
                    triggerArrivalFlash = false;
                }
            }
        }
        applicationWindow.clear(backgroundColor);
        {
            int animatedScanLineIndex = (int)(totalElapsedGameTime * 30.0f) % TOTAL_ROWS;
            sf::RectangleShape scannerLine;
            scannerLine.setSize(sf::Vector2f((float)WINDOW_WIDTH_PIXELS, 1.0f));
            scannerLine.setPosition(sf::Vector2f(0.0f, (float)(animatedScanLineIndex * CELL_SIZE_PIXELS)));
            scannerLine.setFillColor(sf::Color(100, 160, 255, 18));
            applicationWindow.draw(scannerLine);
        }

        vector<vector<int>> mapData = theSimulationMap.getGrid();
        for (int i = 0; i < TOTAL_ROWS; i++) 
        {
            for (int j = 0; j < TOTAL_COLUMNS; j++)
             {
                float pixelX = (float)(j * CELL_SIZE_PIXELS);
                float pixelY = (float)(i * CELL_SIZE_PIXELS);
                if (mapData[i][j] == 1) 
                { 
                    sf::RectangleShape obstacleCell;
                    obstacleCell.setSize(sf::Vector2f((float)(CELL_SIZE_PIXELS - 1), (float)(CELL_SIZE_PIXELS - 1)));
                    obstacleCell.setPosition(sf::Vector2f(pixelX, pixelY));
                    obstacleCell.setFillColor(obstacleColor);
                    applicationWindow.draw(obstacleCell);

                    sf::RectangleShape shineEffect;
                    shineEffect.setSize(sf::Vector2f((float)(CELL_SIZE_PIXELS - 1), 3.0f));
                    shineEffect.setPosition(sf::Vector2f(pixelX, pixelY));
                    shineEffect.setFillColor(obstacleShineColor);
                    applicationWindow.draw(shineEffect);
                }
                 else
                 { 
                    sf::RectangleShape emptyCell;
                    emptyCell.setSize(sf::Vector2f((float)(CELL_SIZE_PIXELS - 1), (float)(CELL_SIZE_PIXELS - 1)));
                    emptyCell.setPosition(sf::Vector2f(pixelX, pixelY));
                    emptyCell.setFillColor(emptyCellColor);
                    applicationWindow.draw(emptyCell);
                }
            }
        }

        vector<Point2D> currentPath = theRover.getCalculatedPath();
        int pathSize = (int)currentPath.size();
        for (int i = 0; i < pathSize; i++) 
        {
            Point2D currentPathPoint = currentPath[i];
            bool hasWalkedOverThisPoint = (i < theRover.getCurrentPathStep());
            sf::RectangleShape pathSquare;
            pathSquare.setSize(sf::Vector2f((float)(CELL_SIZE_PIXELS - 10), (float)(CELL_SIZE_PIXELS - 10)));
            float squareX = (float)(currentPathPoint.getColumn() * CELL_SIZE_PIXELS + 5);
            float squareY = (float)(currentPathPoint.getRow() * CELL_SIZE_PIXELS + 5);
            pathSquare.setPosition(sf::Vector2f(squareX, squareY));
            
            if (hasWalkedOverThisPoint == true) 
            {
                pathSquare.setFillColor(sf::Color(255, 210, 50, 25));
            } 
            else 
            {
                pathSquare.setFillColor(sf::Color(255, 210, 50, 140));
            }
            applicationWindow.draw(pathSquare);
            if (hasWalkedOverThisPoint == false) 
            {
                sf::CircleShape pathDot(3.0f);
                pathDot.setFillColor(sf::Color(255, 235, 120, 200));
                float dotX = (float)(currentPathPoint.getColumn() * CELL_SIZE_PIXELS + CELL_SIZE_PIXELS/2 - 3);
                float dotY = (float)(currentPathPoint.getRow() * CELL_SIZE_PIXELS + CELL_SIZE_PIXELS/2 - 3);
                pathDot.setPosition(sf::Vector2f(dotX, dotY));
                applicationWindow.draw(pathDot);
            }
        }
        deque<Point2D> trailPoints = theRover.getMovementTrail();
        int trailSize = (int)trailPoints.size();
        
        for (int i = 0; i < trailSize; i++) 
        {
            Point2D trailPoint = trailPoints[i];
            float calculatedAlpha = ((float)(i + 1) / trailSize) * 180.0f;
            float calculatedRadius = 4.0f + 3.0f * ((float)(i + 1) / trailSize);
            sf::CircleShape trailCircle(calculatedRadius);
            trailCircle.setFillColor(sf::Color(80, 200, 230, (uint8_t)calculatedAlpha));
            float circleX = (float)(trailPoint.getColumn() * CELL_SIZE_PIXELS + CELL_SIZE_PIXELS/2) - calculatedRadius;
            float circleY = (float)(trailPoint.getRow() * CELL_SIZE_PIXELS + CELL_SIZE_PIXELS/2) - calculatedRadius;
            trailCircle.setPosition(sf::Vector2f(circleX, circleY));
            applicationWindow.draw(trailCircle);
        }
        {
            float pulsingFactor = 0.5f + 0.5f * sin(totalElapsedGameTime * 3.0f);
            float goalX = (float)((TOTAL_COLUMNS - 1) * CELL_SIZE_PIXELS);
            float goalY = (float)((TOTAL_ROWS - 1) * CELL_SIZE_PIXELS);

            for (int ringIndex = 3; ringIndex >= 1; ringIndex--) {
                float ringSize = (float)(CELL_SIZE_PIXELS - 2 + ringIndex * 4);
                sf::RectangleShape glowRing(sf::Vector2f(ringSize, ringSize));
                
                glowRing.setPosition(sf::Vector2f(goalX - ringIndex * 2.0f, goalY - ringIndex * 2.0f));
                glowRing.setFillColor(sf::Color(40, 130, 220, (uint8_t)(30 * pulsingFactor * (4 - ringIndex))));
                
                applicationWindow.draw(glowRing);
            }

            sf::RectangleShape targetSquare(sf::Vector2f((float)(CELL_SIZE_PIXELS - 2), (float)(CELL_SIZE_PIXELS - 2)));
            targetSquare.setPosition(sf::Vector2f(goalX, goalY));
            
            sf::Color currentTargetColor = interpolateColor(targetColor, targetGlowColor, pulsingFactor);
            targetSquare.setFillColor(currentTargetColor);
            applicationWindow.draw(targetSquare);

            sf::RectangleShape horizontalCrosshair(sf::Vector2f((float)(CELL_SIZE_PIXELS - 10), 2.0f));
            horizontalCrosshair.setPosition(sf::Vector2f(goalX + 4.0f, goalY + CELL_SIZE_PIXELS / 2.0f - 1.0f));
            horizontalCrosshair.setFillColor(sf::Color::White);
            applicationWindow.draw(horizontalCrosshair);
            
            sf::RectangleShape verticalCrosshair(sf::Vector2f(2.0f, (float)(CELL_SIZE_PIXELS - 10)));
            verticalCrosshair.setPosition(sf::Vector2f(goalX + CELL_SIZE_PIXELS / 2.0f - 1.0f, goalY + 4.0f));
            verticalCrosshair.setFillColor(sf::Color::White);
            applicationWindow.draw(verticalCrosshair);
        }
        {
            sf::Color roverColorToDraw = theRover.getRoverDisplayColor();
            float centerPixelX = theRover.getDrawingColumn() * CELL_SIZE_PIXELS + CELL_SIZE_PIXELS / 2.0f;
            float centerPixelY = theRover.getDrawingRow() * CELL_SIZE_PIXELS + CELL_SIZE_PIXELS / 2.0f;
            float roverRadius = CELL_SIZE_PIXELS / 2.0f - 4.0f;

            sf::CircleShape roverShadow(roverRadius + 2.0f);
            roverShadow.setFillColor(sf::Color(0, 0, 0, 80));
            roverShadow.setPosition(sf::Vector2f(centerPixelX - roverRadius - 2.0f + 3.0f, centerPixelY - roverRadius - 2.0f + 3.0f));
            applicationWindow.draw(roverShadow);

            sf::CircleShape roverGlow(roverRadius + 5.0f);
            roverGlow.setFillColor(sf::Color(roverColorToDraw.r, roverColorToDraw.g, roverColorToDraw.b, 50));
            roverGlow.setPosition(sf::Vector2f(centerPixelX - roverRadius - 5.0f, centerPixelY - roverRadius - 5.0f));
            applicationWindow.draw(roverGlow);

            sf::CircleShape roverBody(roverRadius);
            roverBody.setFillColor(roverColorToDraw);
            roverBody.setOutlineColor(sf::Color(255, 255, 255, 120));
            roverBody.setOutlineThickness(1.5f);
            roverBody.setPosition(sf::Vector2f(centerPixelX - roverRadius, centerPixelY - roverRadius));
            applicationWindow.draw(roverBody);
            float innerRadius = roverRadius * 0.4f;

            sf::CircleShape innerDot(innerRadius);
            innerDot.setFillColor(sf::Color(0, 0, 0, 100));
            innerDot.setPosition(sf::Vector2f(centerPixelX - innerRadius, centerPixelY - innerRadius));
            applicationWindow.draw(innerDot);

            sf::CircleShape directionPip(3.0f);
            directionPip.setFillColor(sf::Color::White);
            directionPip.setPosition(sf::Vector2f(centerPixelX + roverRadius * 0.35f - 3.0f, centerPixelY - 3.0f));
            applicationWindow.draw(directionPip);
        }
        sf::Text startLabel = createTextObject(standardFont, "START", 9, sf::Color(150, 220, 150, 180), {3.0f, 2.0f});
        applicationWindow.draw(startLabel);
        float goalLabelX = (float)((TOTAL_COLUMNS - 1) * CELL_SIZE_PIXELS + 3);
        float goalLabelY = (float)((TOTAL_ROWS - 1) * CELL_SIZE_PIXELS + 2);
        sf::Text goalLabel = createTextObject(standardFont, "GOAL", 9, targetGlowColor, {goalLabelX, goalLabelY});
        applicationWindow.draw(goalLabel);
        if (triggerArrivalFlash == true) 
        {
            float alphaValue = (1.0f - timeSinceFlashStarted / MAXIMUM_FLASH_DURATION_SECONDS) * 110.0f;
            
            sf::RectangleShape flashRectangle(sf::Vector2f((float)WINDOW_WIDTH_PIXELS, (float)(TOTAL_ROWS * CELL_SIZE_PIXELS)));
            flashRectangle.setFillColor(sf::Color(40, 180, 255, (uint8_t)alphaValue));
            applicationWindow.draw(flashRectangle);

            sf::Text successMessage = createTextObject(standardFont, "TARGET REACHED SUCCESSFULLY!", 35, sf::Color(255, 255, 255, (uint8_t)(alphaValue * 2.1f)), {0.0f, 0.0f}, sf::Text::Bold);
            float messageWidth = successMessage.getLocalBounds().size.x;
            successMessage.setPosition(sf::Vector2f((WINDOW_WIDTH_PIXELS - messageWidth) / 2.0f, (TOTAL_ROWS * CELL_SIZE_PIXELS) / 2.0f - 30.0f));
            applicationWindow.draw(successMessage);
        }
        if (theRover.getCurrentStatus() == RoverState::PATH_IS_STUCK)
         {
            sf::Text stuckMessage = createTextObject(standardFont, "NO PATH FOUND DUE TO OBSTACLES — Press 'R' to generate new map", 16, dangerColor, {0.0f, 0.0f});
            float textWidth = stuckMessage.getLocalBounds().size.x;
            stuckMessage.setPosition(sf::Vector2f((WINDOW_WIDTH_PIXELS - textWidth) / 2.0f, (TOTAL_ROWS * CELL_SIZE_PIXELS) / 2.0f - 12.0f));
            applicationWindow.draw(stuckMessage);
        }
        theHUD.renderHUD(applicationWindow, theRover, isAutomaticModeActive, isSimulationPaused, totalElapsedGameTime);
        applicationWindow.display();
    }
    cout << "Program finished successfully. Returning 0." << endl;
    return 0;
}