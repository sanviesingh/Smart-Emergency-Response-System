# LifeLine — Emergency Response Prioritization and Route Planning System

**Tagline:** Prioritize. Respond. Rescue.

LifeLine is an academic control-room dashboard prototype. An operator can register a reported incident, view the severity-sorted queue, assign an available response resource and view a shortest path through an illustrative graph.

> **Safety notice:** This is a classroom prototype. It is not connected to emergency services, live hospitals, GPS, traffic data or real dispatch systems. Do not use it to request or coordinate real emergency assistance.

## Technology
- C: emergency record, array-based priority queue, graph and Dijkstra algorithm
- C++: Emergency, Resource, Hospital and ResponseSystem classes; Winsock HTTP server
- HTML, CSS and JavaScript: browser dashboard
- CSV: sample dataset (included as reference data; this starter version keeps newly entered incidents in memory)

## Requirements
- Windows 10/11
- MSYS2 UCRT64 environment
- GCC and G++ installed
- Browser

## Build (run from the project root in MSYS2 UCRT64)
```bash
gcc -std=c11 -Wall -Wextra -c dsa/emergency.c -o emergency.o
gcc -std=c11 -Wall -Wextra -c dsa/priority_queue.c -o priority_queue.o
gcc -std=c11 -Wall -Wextra -c dsa/graph.c -o graph.o
gcc -std=c11 -Wall -Wextra -c dsa/dijkstra.c -o dijkstra.o

g++ -std=c++17 -Wall -Wextra backend/main.cpp \
  oop/Emergency.cpp oop/Resource.cpp oop/Hospital.cpp oop/ResponseSystem.cpp \
  emergency.o priority_queue.o graph.o dijkstra.o \
  -lws2_32 -o lifeline.exe
```

If your terminal does not recognize `gcc` or `g++`, open **MSYS2 UCRT64** (not the plain MSYS terminal) and install the toolchain if needed:
```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc
```

## Run
From the project root:
```bash
./lifeline.exe
```
Open **http://localhost:8080** in your browser. Keep the terminal open while using the dashboard. Press Ctrl+C in the terminal to stop the server.

## Demo flow
1. Register an incident. Severity values: 4 Critical, 3 High, 2 Medium, 1 Low.
2. Observe the queue ordered by severity.
3. Select Assign for a pending incident.
4. Review the selected resource, destination and Dijkstra route.
5. Register another incident to see how the resource availability changes.

## API
- `GET /api/health`
- `GET /api/emergencies`
- `GET /api/resources`
- `POST /api/emergencies`
- `POST /api/assign/{id}`

## Model assumptions and limitations
- Locations are mapped to one of four illustrative zones by searching for `North`, `East`, `South` or `West` in the text. An unrecognized location defaults to North Zone.
- Graph distances are fictional sample distances, not live navigation directions or travel times.
- Resource and hospital capacity data are in-memory demonstration values. Assignment is not a real-world clinical or dispatch decision.
- The C priority queue orders incidents by severity. The dashboard displays all incidents sorted by severity; assignment is initiated by the operator for a selected pending incident.
- The sample CSV is provided as a dataset reference; automatic CSV import and persistent storage are not implemented in this baseline.
- The HTTP server is a minimal local demo server, not production-grade or internet-facing. It handles one connection at a time and has no authentication.

