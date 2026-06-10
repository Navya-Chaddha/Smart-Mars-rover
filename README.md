Smart Mars Rover 

## Overview
This project is a C++-based Mars Rover Navigation Simulator that demonstrates autonomous path planning using the A* algorithm. The simulation models a rover navigating through a grid-based Martian terrain containing randomly generated obstacles while attempting to reach a predefined target location.
The project was developed to explore concepts in pathfinding,search algorithms, object-oriented programming, and real-time visualization using SFML.

## Objectives
1. Simulates rover navigation in an obstacle-filled environment.
2. Implement the A* algorithm for optimal path planning.
3. Visualize path generation and rover movement in real time.
4. Provide both manual and autonomous navigation modes.
5. Demonstrate the practical application of data structures and algorithms in robotics-inspired systems.

## Features
1. Autonomous navigation using the A* pathfinding algorithm.
2.  Random obstacle generation for different terrain layouts.
3. Manual rover control using keyboard input.
4. Real-time path visualization.
5. Obstacle proximity monitoring.
6. Mission status dashboard (HUD).
  
## Technologies Used
1. C++
2. SFML
3. Object-Oriented Programming (OOP)
4. Graph Search Algorithms
5. Priority Queues

## Controls
 Arrow Keys -> Move rover manually                        
 M          -> Toggle between Manual and Automatic mode   
 P          -> Pause or Resume simulation                
 R          -> Reset rover and generate a new environment 

### Prerequisites

* C++17 or later
* SFML installed and configured

### Build Command

```bash
g++ main.cpp -o MarsRover -lsfml-graphics -lsfml-window -lsfml-system
```

### Run

```bash
./MarsRover
```
## Learning Outcomes
Through this project, the following concepts were explored:
1.A* Pathfinding Algorithm
2. Heuristic Search Techniques
3. Graph Traversal
4. Real-Time Simulation Design

## Future Enhancements
Potential improvements include:
1. Diagonal movement support
2.  Dynamic obstacle detection
3. Terrain cost weighting
4. Multi-destination navigation
5.  Sensor-based real-time replanning
6.  3D environment visualization
7.  Integration with robotics hardware

## Author
Developed as an academic project to demonstrate autonomous navigation, path planning, and simulation development using C++ and SFML.
