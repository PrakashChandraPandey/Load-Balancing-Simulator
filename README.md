# Load Balancing Simulator

**Team T213** · Data Structures with C (TCS302) and Object Oriented Programming with C++ (TCS307)
Graphic Era (Deemed to be University), Dehradun

## About the project

The Load Balancing Simulator is a C++ program that shows how a load balancer distributes incoming requests among multiple servers, and how it handles changing traffic and server failures.

When many requests arrive at the same time, some servers can become overloaded while others stay idle. This project simulates a load balancer that manages itself:

- it **selects the algorithm automatically** (Round Robin, Weighted Round Robin or Least Connections) and shows the reason
- it **scales the servers automatically** (vertical and horizontal scaling) based on utilization
- it **handles server failure and recovery**
- it **saves every event in a log file**

The project has a **Qt GUI** and a **console version**.

## Features

- **Request generation:** each request gets a path (`/login`, `/browse`, `/search`, `/upload`, `/report`, `/video`) and a processing time that depends on the path. Most requests are short (70%), some are medium (20%) and a few are long (10%).
- **Circular queue:** waiting requests are stored in a circular queue (FIFO).
- **Three algorithms:**
  - Round Robin: servers take turns
  - Weighted Round Robin: weight = capacity, so a bigger server gets more requests
  - Least Connections: the request goes to the least loaded server (load ÷ capacity)
- **Automatic algorithm selection** (checked every 10 seconds):
  - longest of the last 10 request times > 3 × shortest → Least Connections
  - otherwise, servers have different capacities → Weighted Round Robin
  - otherwise → Round Robin
- **Auto-scaling:**
  - utilization above 80% for 3 seconds → scale up
  - utilization below 30% for 10 seconds → scale down
  - the admin chooses the scaling type (automatic, vertical only or horizontal only) and the capacity change per vertical step (1–4)
  - maximum server capacity is 8, and there is a 5-second wait (cooldown) after each change
- **Server failure and recovery:** a failed server gets no new requests, and its pending requests move back to the queue. A request moved more than 3 times is marked as failed.
- **Log file:** every event is saved in `load_balancer_log.txt`, with a summary (completed requests, average waiting time, algorithm changes, scaling actions, 5 longest waits).

## Project structure

```
LoadBalancerSimulator/
├── data_structures.h                      Request structure, circular queue, linked list (C)
├── ProjectHeaders.h                       All class declarations
├── Request_Module.cpp
├── Circular_Queue_Module.cpp
├── Server_Module.cpp
├── Request_Generation_Module.cpp
├── Round_Robin_Algorithm.cpp
├── Weighted_Round_Robin_Algorithm.cpp
├── Least_Connections_Algorithm.cpp
├── Algorithm_Selector_Module.cpp
├── Load_Balancer_Core_and_Algorithms.cpp
├── Server_Failure_Module.cpp
├── Server_Recovery_Module.cpp
├── Auto_Scaler_Module.cpp
├── Logger_Module.cpp
├── Server_Display_Module.cpp
├── Queue_Display_Module.cpp
├── Main_Menu_Module.cpp                   Console version (main function)
├── README.md
└── gui/
    ├── LoadBalancerGUI.pro                Qt project file
    ├── main.cpp                           Starts the GUI
    ├── mainwindow.h
    └── mainwindow.cpp                     GUI window
```

## Modules

| Module | File | What it does |
|---|---|---|
| Header file (data structures) | data_structures.h | Request structure, circular queue and linked list (with bubble sort), written in C |
| Request module | Request_Module.cpp | Creates a request and calculates its waiting time |
| Circular queue module | Circular_Queue_Module.cpp | C++ class that uses the circular queue from data_structures.h |
| Server module | Server_Module.cpp | Capacity, running requests, processing, failure, recovery, change of capacity |
| Request generation module | Request_Generation_Module.cpp | Creates requests with random paths and times |
| Round Robin module | Round_Robin_Algorithm.cpp | Servers take turns |
| Weighted Round Robin module | Weighted_Round_Robin_Algorithm.cpp | Bigger servers get more requests |
| Least Connections module | Least_Connections_Algorithm.cpp | Least loaded server gets the request |
| Algorithm selector module | Algorithm_Selector_Module.cpp | Selects the algorithm automatically |
| Load balancer module | Load_Balancer_Core_and_Algorithms.cpp | Connects all modules and runs the simulation every second |
| Server failure module | Server_Failure_Module.cpp | Fails a server and moves its requests back to the queue |
| Server recovery module | Server_Recovery_Module.cpp | Makes a failed server active again |
| Auto-scaler module | Auto_Scaler_Module.cpp | Vertical and horizontal scaling based on utilization |
| Logger module | Logger_Module.cpp | Writes every event to the log file |
| Server display module | Server_Display_Module.cpp | Shows server status (console) |
| Queue display module | Queue_Display_Module.cpp | Shows the waiting queue (console) |
| GUI module | gui/ | Qt window for the whole simulator |

## How to run

### Qt GUI (recommended)

1. Install **Qt** with **Qt Creator** and a **MinGW** compiler (Qt 5 or Qt 6).
2. Open Qt Creator → **File → Open File or Project** → select `gui/LoadBalancerGUI.pro`.
3. Click **Configure Project**, then the green **Run** button.

### Console version

In the project folder:

```
g++ *.cpp -o simulator
./simulator
```

On Windows: `g++ *.cpp -o simulator.exe` and then `simulator.exe`.

## How to use the GUI

1. Enter the number of servers and their capacities (example: `3, 4, 2`) → **Create Servers**.
   The simulation starts and moves forward one second every real second.
2. Enter a number → **Generate Requests**.
3. Choose a server → **Fail Server** or **Recover Server**.
4. Choose the scaling type and the capacity change per step → **Apply Scaling Settings**.
5. **Save Summary to Log File** writes the summary.

The screen shows the selected algorithm with the reason, utilization, request counts, each server with its running requests, the circular queue (with FRONT and REAR) and the activity log.

The log file `load_balancer_log.txt` is created in the folder where the program runs (for the GUI, this is Qt Creator's build folder).

## Test mode

With normal traffic, request times vary a lot, so Least Connections is selected most of the time. To show Round Robin and Weighted Round Robin clearly, set `TEST_MODE = true` (in `gui/mainwindow.cpp` for the GUI, or `Main_Menu_Module.cpp` for the console). Then every request takes 3–5 seconds:

- equal capacities → Round Robin
- Vertical only scaling → capacities become different → Weighted Round Robin
- Horizontal only scaling → equal servers are added → Round Robin continues

## Assumptions

- Request paths and their processing times are simulated values chosen by us, not measured from a real website.
- Processing is simulated: every second, each running request's remaining time decreases by 1.
- Servers differ only in capacity, so the weight of a server is equal to its capacity.

## Team

| Member | Roll No. |
|---|---|
| Prakash Chandra Pandey (Team Lead) | 2512510008 |
| Tirth Sharma | 2510370377 |
| Harsh Rastogi | 2510370310 |

Mentor: Mr. Manish Mehta
