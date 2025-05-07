#include "application.h"

#include <iostream>
#include <limits>
#include <map>
#include <queue> // priority_queue
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dist.h"
#include "graph.h"
#include "json.hpp"

using namespace std;

double INF = numeric_limits<double>::max();

void buildGraph(istream &input, graph<long long, double> &g,
                vector<BuildingInfo> &buildings,
                unordered_map<long long, Coordinates> &coords) {
  // TODO_STUDENT
  using json = nlohmann::json;
  json mapData;
  input >> mapData;

   
    for (const auto& structure : mapData["buildings"]) {
        BuildingInfo campusBldg;
        campusBldg.id = structure["id"];
        campusBldg.location = {structure["lat"], structure["lon"]};
        campusBldg.name = structure["name"];
        campusBldg.abbr = structure["abbr"];
        
        buildings.emplace_back(campusBldg);
        g.addVertex(campusBldg.id);
    }

    
    for (const auto& node : mapData["waypoints"]) {
        long long nodeId = node["id"];
        Coordinates position = {node["lat"], node["lon"]};
        coords[nodeId] = position;
        g.addVertex(nodeId);
    }

    
    for (const auto& campusBldg : buildings) {
        for (const auto& [nodeId, nodePos] : coords) {
            double separation = distBetween2Points(campusBldg.location, nodePos);
            if (separation < 0.036) {
                g.addEdge(campusBldg.id, nodeId, separation);
                g.addEdge(nodeId, campusBldg.id, separation);
            }
        }
    }

    
    for (const auto& footpath : mapData["footways"]) {
        for (size_t idx = 0; idx < footpath.size() - 1; ++idx) {
            long long current = footpath[idx];
            long long adjacent = footpath[idx + 1];
            
            double pathLength = distBetween2Points(coords[current], coords[adjacent]);
            g.addEdge(current, adjacent, pathLength);
            g.addEdge(adjacent, current, pathLength);
        }
    }
}

BuildingInfo getBuildingInfo(const vector<BuildingInfo> &buildings,
                             const string &query) {
  for (const BuildingInfo &building : buildings) {
    if (building.abbr == query) {
      return building;
    } else if (building.name.find(query) != string::npos) {
      return building;
    }
  }
  BuildingInfo fail;
  fail.id = -1;
  return fail;
}

BuildingInfo getClosestBuilding(const vector<BuildingInfo> &buildings,
                                Coordinates c) {
  double minDestDist = INF;
  BuildingInfo ret = buildings.at(0);
  for (const BuildingInfo &building : buildings) {
    double dist = distBetween2Points(building.location, c);
    if (dist < minDestDist) {
      minDestDist = dist;
      ret = building;
    }
  }
  return ret;
}

class prioritize {
  public:
   bool operator()(const pair<long long, double>& p1,
                   const pair<long long, double>& p2) const {
     return p1.second > p2.second;
   }
 };


vector<long long> dijkstra(const graph<long long, double>& G, long long start,
  long long target, const set<long long>& ignoreNodes) {

if (start == target) return {target};


unordered_map<long long, double> travel_cost;
unordered_map<long long, long long> path_origin;

priority_queue<pair<long long, double>, vector<pair<long long, double>>, prioritize>
      node_queue;


node_queue.emplace(start, 0.0);
travel_cost[start] = 0.0;


while (!node_queue.empty()) {
auto [current_node, current_cost] = node_queue.top();
node_queue.pop();


if (travel_cost[current_node] < current_cost) continue;


if (current_node == target) break;


for (const auto& adjacent : G.neighbors(current_node)) {

if (ignoreNodes.count(adjacent) && adjacent != target) continue;

double edge_weight;
if (!G.getWeight(current_node, adjacent, edge_weight)) continue;

double new_cost = current_cost + edge_weight;


if (!travel_cost.count(adjacent) || new_cost < travel_cost[adjacent]) {
travel_cost[adjacent] = new_cost;
path_origin[adjacent] = current_node;
node_queue.emplace(adjacent, new_cost);
}
}
}


if (!path_origin.count(target)) return {};


vector<long long> route;
for (long long node = target; node != start; node = path_origin[node]) {
route.push_back(node);
}
route.push_back(start);
reverse(route.begin(), route.end());

return route;
}

double pathLength(const graph<long long, double> &G,
                  const vector<long long> &path) {
  double length = 0.0;
  double weight;
  for (size_t i = 0; i + 1 < path.size(); i++) {
    bool res = G.getWeight(path.at(i), path.at(i + 1), weight);
    if (!res) {
      return -1;
    }
    length += weight;
  }
  return length;
}

void outputPath(const vector<long long> &path) {
  for (size_t i = 0; i < path.size(); i++) {
    cout << path.at(i);
    if (i != path.size() - 1) {
      cout << "->";
    }
  }
  cout << endl;
}

// Honestly this function is just a holdover from an old version of the project
void application(const vector<BuildingInfo> &buildings,
                 const graph<long long, double> &G) {
  string person1Building, person2Building;

  set<long long> buildingNodes;
  for (const auto &building : buildings) {
    buildingNodes.insert(building.id);
  }

  cout << endl;
  cout << "Enter person 1's building (partial name or abbreviation), or #> ";
  getline(cin, person1Building);

  while (person1Building != "#") {
    cout << "Enter person 2's building (partial name or abbreviation)> ";
    getline(cin, person2Building);

    // Look up buildings by query
    BuildingInfo p1 = getBuildingInfo(buildings, person1Building);
    BuildingInfo p2 = getBuildingInfo(buildings, person2Building);
    Coordinates P1Coords, P2Coords;
    string P1Name, P2Name;

    if (p1.id == -1) {
      cout << "Person 1's building not found" << endl;
    } else if (p2.id == -1) {
      cout << "Person 2's building not found" << endl;
    } else {
      cout << endl;
      cout << "Person 1's point:" << endl;
      cout << " " << p1.name << endl;
      cout << " " << p1.id << endl;
      cout << " (" << p1.location.lat << ", " << p1.location.lon << ")" << endl;
      cout << "Person 2's point:" << endl;
      cout << " " << p2.name << endl;
      cout << " " << p2.id << endl;
      cout << " (" << p2.location.lon << ", " << p2.location.lon << ")" << endl;

      Coordinates centerCoords = centerBetween2Points(p1.location, p2.location);
      BuildingInfo dest = getClosestBuilding(buildings, centerCoords);

      cout << "Destination Building:" << endl;
      cout << " " << dest.name << endl;
      cout << " " << dest.id << endl;
      cout << " (" << dest.location.lat << ", " << dest.location.lon << ")"
           << endl;

      vector<long long> P1Path = dijkstra(G, p1.id, dest.id, buildingNodes);
      vector<long long> P2Path = dijkstra(G, p2.id, dest.id, buildingNodes);

      // This should NEVER happen with how the graph is built
      if (P1Path.empty() || P2Path.empty()) {
        cout << endl;
        cout << "At least one person was unable to reach the destination "
                "building. Is an edge missing?"
             << endl;
        cout << endl;
      } else {
        cout << endl;
        cout << "Person 1's distance to dest: " << pathLength(G, P1Path);
        cout << " miles" << endl;
        cout << "Path: ";
        outputPath(P1Path);
        cout << endl;
        cout << "Person 2's distance to dest: " << pathLength(G, P2Path);
        cout << " miles" << endl;
        cout << "Path: ";
        outputPath(P2Path);
      }
    }

    //
    // another navigation?
    //
    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);
  }
}
