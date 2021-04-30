/**
 * Copyright (c) 2020, Cristina Ruiz Martin
 * ARSLab - Carleton University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/**
* Model developed by Sebastien Cook 
* based on the model by Hoda Khalil in Cell-DEVS CD++
* Implemented in Cadmium-cell-DEVS by Cristina Ruiz Martin
*/

#ifndef CADMIUM_CELLDEVS_CO2_CELL_HPP
#define CADMIUM_CELLDEVS_CO2_CELL_HPP

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/cell/grid_cell.hpp>

using nlohmann::json;
using namespace cadmium::celldevs;
using namespace std;

float cell_size = 25;
// Model Variables
int studentGenerateCount = 5; //Student generate speed (n count/student)

std::list<std::pair<int,int>> actionList; //List include the position for next CO2_Source movement action.

std::list<std::pair<int,int>> infected_actionList; //List include the position for next CO2_Source movement action.

std::list<std::pair<int,int>> susceptible_actionList; //List include the position for next CO2_Source movement action.
std::list<std::pair<std::pair<int,char>,std::pair<int,int>>> studentsList; //List include all CO2_Source that generated <<StudentID,state(+:Joining;-:Leaving)>,<xPosition,yPosition>>

std::list<std::pair<int,std::pair<int,int>>> workstationsList; //List include the information of exist workstations <workStationID,<xPosition,yPosition>>
int workstationNumber = 0; //Total number of exist workstations

int studentGenerated = 0; //Record the number of students the already generated
int counter = 0; //counter for studentGenerated
int numInfect = 0;
/************************************/
/******COMPLEX STATE STRUCTURE*******/
/************************************/
enum CELL_TYPE {AIR=-100, CO2_SOURCE=-200, IMPERMEABLE_STRUCTURE=-300, DOOR=-400, WINDOW=-500, VENTILATION=-600, WORKSTATION=-700, SUSCEPTIBLE_CO2_SOURCE=-800, INFECTED_CO2_SOURCE = -900};
struct co2 {
    int counter;
    int concentration;
    int risky_distance;
    int exposure_time=0; //time a co2 cell has been exposed to threashold co2 levels
    int distanceCount=0;
    CELL_TYPE type;
    co2() : counter(-1), concentration(500), type(AIR), risky_distance(0) {}  // a default constructor is required
    co2(int i_counter, int i_concentration, CELL_TYPE i_type, int i_risky_distance) : counter(i_counter), concentration(i_concentration), type(i_type), risky_distance(i_risky_distance) {}
};
// Required for comparing states and detect any change
inline bool operator != (const co2 &x, const co2 &y) {
    return x.counter != y.counter || x.concentration != y.concentration || x.type != y.type || x.risky_distance != y.risky_distance;
}
// Required if you want to use transport delay (priority queue has to sort messages somehow)
inline bool operator < (const co2& lhs, const co2& rhs){ return true; }

// Required for printing the state of the cell
std::ostream &operator << (std::ostream &os, const co2 &x) {
    os << "<" << x.counter << "," << x.concentration << "," << x.type << "," << x.risky_distance << ">";
    return os;
}

// Required for creating co2 objects from JSON file
void from_json(const json& j, co2 &s) {
    j.at("counter").get_to(s.counter);
    j.at("concentration").get_to(s.concentration);
    j.at("type").get_to(s.type);
    j.at("riskyDistance").get_to(s.risky_distance);
}

/************************************/
/******COMPLEX CONFIG STRUCTURE******/
/************************************/
struct conc {
    float conc_increase; //CO2 generated by one person
    int base; //CO2 base level
    int window_conc; //CO2 level at window
    int vent_conc; //CO2 level at vent
    int resp_time;
    int totalStudents; //Total CO2_Source in the model
    int risky_exposure_time;
    float risky_concentration; 
    int numInfected;
    int X_wind;
    int Y_wind;
    float wind;
    // Each cell is 25cm x 25cm x 25cm = 15.626 Liters of air each
    // CO2 sources have their concentration continually increased by default by 12.16 ppm every 5 seconds.
    conc(): conc_increase(121.6*2), base(500), resp_time(5), window_conc(400), vent_conc(300), totalStudents(36), risky_exposure_time(45), risky_concentration(650), numInfected(1), X_wind(0), Y_wind(0),wind(0) {}
    conc(float ci, int b, int wc, int vc, int r, int ts, int re, int rc, int ni, int xw, int yw, float w): conc_increase(ci), base(b), resp_time(r), window_conc(wc), vent_conc(vc), totalStudents(ts), risky_exposure_time(re), risky_concentration(rc), numInfected(ni), X_wind(xw), Y_wind(yw), wind(w) {}
};
void from_json(const json& j, conc &c) {
    j.at("conc_increase").get_to(c.conc_increase);
    j.at("base").get_to(c.base);
    j.at("resp_time").get_to(c.resp_time);
    j.at("window_conc").get_to(c.window_conc);
    j.at("vent_conc").get_to(c.vent_conc);
    j.at("vent_conc").get_to(c.totalStudents);
    j.at("risky_exposure_time").get_to(c.risky_exposure_time);
    j.at("risky_concentration").get_to(c.risky_concentration);
    j.at("numInfected").get_to(c.numInfected);
    j.at("X_wind").get_to(c.X_wind);
    j.at("Y_wind").get_to(c.Y_wind);
    j.at("wind").get_to(c.wind);
}


template <typename T>
class co2_lab_cell : public grid_cell<T, co2> {
public:
    using grid_cell<T, co2, int>::simulation_clock;
    using grid_cell<T, co2, int>::state;
    using grid_cell<T, co2, int>::map;
    using grid_cell<T, co2, int>::neighbors;
    using grid_cell<T, co2, int>::cell_id;

    using config_type = conc;  // IMPORTANT FOR THE JSON   
    float concentration_increase; //// CO2 sources have their concentration continually increased
    int base; //CO2 base level
    int resp_time; //Time used to calculate the concentration inscrease
    int window_conc; //CO2 level at window
    int vent_conc; //CO2 level at cent
    int totalStudents; //Total CO2_Source in the model
    int risky_exposure_time;
    float risky_concentration; 
    int numInfected;
    int X_wind;
    int Y_wind;
    float wind;
 
    co2_lab_cell() : grid_cell<T, co2, int>() {
    }

    co2_lab_cell(cell_position const &cell_id, cell_unordered<int> const &neighborhood, co2 initial_state,
        cell_map<co2, int> const &map_in, std::string const &delayer_id, conc config) :
            grid_cell<T, co2>(cell_id, neighborhood, initial_state, map_in, delayer_id) {

        float volume = cell_size*cell_size*cell_size;
        concentration_increase = (19*200000)/volume;
        base = config.base;
        resp_time = config.resp_time;
        window_conc = config.window_conc;
        vent_conc = config.vent_conc;
        totalStudents = config.totalStudents;
        risky_exposure_time = config.risky_exposure_time; 
        risky_concentration = config.risky_concentration;
        numInfected = config.numInfected;
        X_wind = config.X_wind;
        Y_wind = config.Y_wind;
        wind = config.wind;

        if(initial_state.type == WORKSTATION) {
            std::pair<int,std::pair<int,int>> workstationInfo;
            workstationInfo.first = workstationNumber;
            workstationInfo.second.first = cell_id[0];
            workstationInfo.second.second = cell_id[1];
            workstationNumber++;
            workstationsList.push_back(workstationInfo);
        }
    }

    co2 local_computation() const override {
        co2 new_state = state.current_state;
//        co2 new_state = state.neighbors_state.at(cell_id);

        std::pair<int,int> currentLocation;
        currentLocation.first = this->map.location[0];
        currentLocation.second = this->map.location[1];

        switch(state.current_state.type){
            case IMPERMEABLE_STRUCTURE: 
                new_state.concentration = 0;
                break;
            case DOOR:    
                new_state.concentration = base;
                break;
            case WINDOW:
                new_state.concentration = window_conc;
                break;
            case VENTILATION:
                new_state.concentration = vent_conc;
                break;
            case AIR:{
                int concentration = 0;
                int flow_concentration = 0;
                int num_neighbors = 0;
                vector<int> neighbor_relative_pos;
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                    	
                    	neighbor_relative_pos = map.relative(neighbors.first);
                    	if(X_wind == neighbor_relative_pos[0] && Y_wind == neighbor_relative_pos[1]){
                    		flow_concentration = (neighbors.second.concentration*wind);
                    	}
                    	else{
                        	concentration += neighbors.second.concentration;
                       }
                        num_neighbors +=1;
                       if(new_state.risky_distance == 0){
                       	if(neighbors.second.risky_distance > 0){
                    			new_state.risky_distance = neighbors.second.risky_distance - 1;
                    		}
                    	}
                    }
                    
                }
                if( flow_concentration != 0 ){
					new_state.concentration = (1-wind)*concentration/(num_neighbors-1) + flow_concentration;
		}
		else{
			new_state.concentration = concentration/num_neighbors;
		}
		
                //Appear CO2_Source at currentLocation
                if(std::find(actionList.begin(),actionList.end(),currentLocation) != actionList.end()){
                    new_state.type = CO2_SOURCE;
                 }
                else if(std::find(susceptible_actionList.begin(),susceptible_actionList.end(),currentLocation) != susceptible_actionList.end()){
                    new_state.type = SUSCEPTIBLE_CO2_SOURCE;
                 }
                 else if(std::find(infected_actionList.begin(),infected_actionList.end(),currentLocation) != infected_actionList.end()){
                    new_state.type = INFECTED_CO2_SOURCE;
                 }
		
		
		
                if (currentLocation.first == 8 && currentLocation.second == 5) {
                    if (counter == 0 && studentGenerated < totalStudents && studentGenerated < workstationNumber){
                        //Given student ID and record the location
                        std::pair<std::pair<int,char>,std::pair<int,int>> studentID;
                        studentID.first.first = studentGenerated;
                        studentID.first.second = '+';
                        studentID.second = currentLocation;
                        studentsList.push_back(studentID);
			
			 if(numInfect < numInfected){
			 	if(studentGenerated > 4){
			 		//spawn them at a random time
			 		//Arrange the next action
                        		infected_actionList.push_back(currentLocation);
                       	 	studentGenerated++;
                       	 	new_state.type = INFECTED_CO2_SOURCE;
                       	 	numInfect++; 
                       	 }
                       	 else{
                       	 	//Arrange the next action
                        		actionList.push_back(currentLocation);

                       	 	studentGenerated++;
                       	 	new_state.type = CO2_SOURCE;
                       	 }
			 }
			 else{
                       	//Arrange the next action
                        	actionList.push_back(currentLocation);

                       	 studentGenerated++;
                       	 new_state.type = CO2_SOURCE;
                        }
                    }
                    counter = (counter + 1) % studentGenerateCount;
                }
                if(new_state.distanceCount == 30 && new_state.risky_distance>0){
                	new_state.risky_distance--;
                	new_state.distanceCount = 0;
                }
                else if(new_state.risky_distance>0){
                	new_state.distanceCount++;
                }
                break;
            }
            case WORKSTATION:{
                int concentration = 0;
                int num_neighbors = 0;
                int flow_concentration = 0;
                vector<int> neighbor_relative_pos;
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                    	
                    	neighbor_relative_pos = map.relative(neighbors.first);
                    	if(X_wind == neighbor_relative_pos[0] && Y_wind == neighbor_relative_pos[1]){
                    		flow_concentration = (neighbors.second.concentration*wind);
                    	}
                    	else{
                        	concentration += neighbors.second.concentration;
                       }
                        num_neighbors +=1;
                       if(new_state.risky_distance == 0){
                       	if(neighbors.second.risky_distance > 0){
                    			new_state.risky_distance = neighbors.second.risky_distance - 1;
                    		}
                    	}
                    }
                    
                }
                if( flow_concentration != 0 ){
					new_state.concentration = (1-wind)*concentration/(num_neighbors-1) + flow_concentration;
		}
		else{
			new_state.concentration = concentration/num_neighbors;
		}
                if(new_state.distanceCount == 30 && new_state.risky_distance>0){
                	new_state.risky_distance--;
                	new_state.distanceCount = 0;
                }
                else if(new_state.risky_distance>0){
                	new_state.distanceCount++;
                }
                break;
            }
            case CO2_SOURCE:{
                int concentration = 0;
                int num_neighbors = 0;
                int flow_concentration = 0;
                vector<int> neighbor_relative_pos;
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                    	
                    	neighbor_relative_pos = map.relative(neighbors.first);
                    	if(X_wind == neighbor_relative_pos[0] && Y_wind == neighbor_relative_pos[1]){
                    		flow_concentration = (neighbors.second.concentration*wind);
                    	}
                    	else{
                        	concentration += neighbors.second.concentration;
                       }
                        num_neighbors +=1;
                       if(new_state.risky_distance == 0){
                       	if(neighbors.second.risky_distance > 0){
                    			new_state.risky_distance = neighbors.second.risky_distance - 1;
                    		}
                    	}
                    }
                    
                }
                if( flow_concentration != 0 ){
					new_state.concentration = (1-wind)*concentration/(num_neighbors-1) + flow_concentration;
		}
		else{
			new_state.concentration = concentration/num_neighbors;
		}
                new_state.counter += 1;

		
                //Remove CO2_Source at currentLocation
                if(std::find(actionList.begin(),actionList.end(),currentLocation) != actionList.end()){
                    //check if co2 generator is at risk
                if (new_state.concentration >= risky_concentration){
                	if(new_state.risky_distance > 0){
                		if(new_state.risky_distance <= 0){
					new_state.exposure_time++;
				}
				else if (new_state.risky_distance <= 2){
					new_state.exposure_time += 2;
				}
				else{
					new_state.exposure_time += 4;
				}
				if (new_state.exposure_time >= risky_exposure_time){ 
					new_state.type = SUSCEPTIBLE_CO2_SOURCE;
					//Arrange the next action
                        		susceptible_actionList.push_back(currentLocation);
                        		actionList.remove(currentLocation);
				}
			}
		}
                    srand(time(0));
                    int randomNumber = (rand() % 60) + 60;
                    std::list<std::pair<std::pair<int, char>, std::pair<int, int>>>::iterator i;
                    for (i = studentsList.begin(); i != studentsList.end(); i++) {
                        if (i->second == currentLocation) { //Find the corresponding student
                            if(state.current_state.counter >= randomNumber){
                                i->first.second = '-';
                            }
                            std::pair<int, int> nextLocation = setNextRoute(currentLocation, i->first);
                            i->second = nextLocation;

                            if(nextLocation == currentLocation){ //Stay at same location
                                //DO NOTHING
                            }else if(nextLocation.first == -1 && nextLocation.second == -1){
                                //Change the type
                                new_state.type = AIR;
                                actionList.remove(currentLocation);
                            }else {
                                //Arrangement next action and change the type
                                actionList.remove(currentLocation);
                                actionList.push_back(nextLocation);
                                new_state.type = AIR;
                            }
                        }
                    }
                }
                if(new_state.distanceCount == 30 && new_state.risky_distance>0){
                	new_state.risky_distance--;
                	new_state.distanceCount = 0;
                }
                else if(new_state.risky_distance>0){
                	new_state.distanceCount++;
                }
                break;
            }
            case SUSCEPTIBLE_CO2_SOURCE:{
            	int concentration = 0;
                int num_neighbors = 0;
                int flow_concentration = 0;
                vector<int> neighbor_relative_pos;
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                    	
                    	neighbor_relative_pos = map.relative(neighbors.first);
                    	if(X_wind == neighbor_relative_pos[0] && Y_wind == neighbor_relative_pos[1]){
                    		flow_concentration = (neighbors.second.concentration*wind);
                    	}
                    	else{
                        	concentration += neighbors.second.concentration;
                       }
                        num_neighbors +=1;
                       if(new_state.risky_distance == 0){
                       	if(neighbors.second.risky_distance > 0){
                    			new_state.risky_distance = neighbors.second.risky_distance - 1;
                    		}
                    	}
                    }
                    
                }
                if( flow_concentration != 0 ){
					new_state.concentration = (1-wind)*concentration/(num_neighbors-1) + flow_concentration;
		}
		else{
			new_state.concentration = concentration/num_neighbors;
		}
                new_state.counter += 1;

                //Remove CO2_Source at currentLocation
                if(std::find(susceptible_actionList.begin(),susceptible_actionList.end(),currentLocation) != susceptible_actionList.end()){
                    srand(time(0));
                    int randomNumber = (rand() % 60) + 60;
                    std::list<std::pair<std::pair<int, char>, std::pair<int, int>>>::iterator i;
                    for (i = studentsList.begin(); i != studentsList.end(); i++) {
                        if (i->second == currentLocation) { //Find the corresponding student
                            if(state.current_state.counter >= randomNumber){
                                i->first.second = '-';
                            }
                            std::pair<int, int> nextLocation = setNextRoute(currentLocation, i->first);
                            i->second = nextLocation;

                            if(nextLocation == currentLocation){ //Stay at same location
                                //DO NOTHING
                            }else if(nextLocation.first == -1 && nextLocation.second == -1){
                                //Change the type
                                new_state.type = AIR;
                                susceptible_actionList.remove(currentLocation);
                            }else {
                                //Arrangement next action and change the type
                                susceptible_actionList.remove(currentLocation);
                                susceptible_actionList.push_back(nextLocation);
                                new_state.type = AIR;
                            }
                        }
                    }
                }
                if(new_state.distanceCount == 30 && new_state.risky_distance>0){
                	new_state.risky_distance--;
                	new_state.distanceCount = 0;
                }
                else if(new_state.risky_distance>0){
                	new_state.distanceCount++;
                }
            	break;
            }
            case INFECTED_CO2_SOURCE:{
            	int concentration = 0;
                int num_neighbors = 0;
                int flow_concentration = 0;
                vector<int> neighbor_relative_pos;
                for(auto neighbors: state.neighbors_state) {
                    if( neighbors.second.concentration < 0){
                        assert(false && "co2 concentration cannot be negative");
                    }
                    if(neighbors.second.type != IMPERMEABLE_STRUCTURE){
                    	
                    	neighbor_relative_pos = map.relative(neighbors.first);
                    	if(X_wind == neighbor_relative_pos[0] && Y_wind == neighbor_relative_pos[1]){
                    		flow_concentration = (neighbors.second.concentration*wind);
                    	}
                    	else{
                        	concentration += neighbors.second.concentration;
                       }
                        num_neighbors +=1;
                       if(new_state.risky_distance == 0){
                       	if(neighbors.second.risky_distance > 0){
                    			new_state.risky_distance = neighbors.second.risky_distance - 1;
                    		}
                    	}
                    }
                    
                }
                if( flow_concentration != 0 ){
					new_state.concentration = (1-wind)*concentration/(num_neighbors-1) + flow_concentration + (concentration_increase);
		}
		else{
			new_state.concentration = concentration/num_neighbors + (concentration_increase);
		}
                
                
                new_state.counter += 1;
                new_state.risky_distance = 4;

		
                //Remove CO2_Source at currentLocation
                if(std::find(infected_actionList.begin(),infected_actionList.end(),currentLocation) != infected_actionList.end()){
                   
                    srand(time(0));
                    int randomNumber = (rand() % 60) + 60;
                    std::list<std::pair<std::pair<int, char>, std::pair<int, int>>>::iterator i;
                    for (i = studentsList.begin(); i != studentsList.end(); i++) {
                        if (i->second == currentLocation) { //Find the corresponding student
                            if(state.current_state.counter >= randomNumber){
                                i->first.second = '-';
                            }
                            std::pair<int, int> nextLocation = setNextRoute(currentLocation, i->first);
                            i->second = nextLocation;

                            if(nextLocation == currentLocation){ //Stay at same location
                                //DO NOTHING
                            }else if(nextLocation.first == -1 && nextLocation.second == -1){
                                //Change the type
                                new_state.type = AIR;
                                new_state.risky_distance = 4;
                                infected_actionList.remove(currentLocation);
                            }else {
                                //Arrangement next action and change the type
                                infected_actionList.remove(currentLocation);
                                infected_actionList.push_back(nextLocation);
                                new_state.type = AIR;
                                new_state.risky_distance = 4;
                            }
                        }
                    }
                }
                if(new_state.distanceCount == 30 && new_state.risky_distance>0){
                	new_state.risky_distance--;
                	new_state.distanceCount = 0;
                }
                else if(new_state.risky_distance>0){
                	new_state.distanceCount++;
                }
            	break;
            }
            default:{
                assert(false && "should never happen");
            }
        }
        return new_state;
    }

    /*
     * Calculate the position after the movement
     *
     * return: nextLocation
     */
    [[nodiscard]] std::pair<int,int> setNextRoute(std::pair<int,int> location, std::pair<int, char> studentIDNumber) const {
        std::pair<int, int> nextLocation;
        std::pair<int, int> destination;
        std::pair<int, int> locationChange;
        int destinationWSNum = studentIDNumber.first % workstationNumber;

        if(studentIDNumber.second == '-'){
            destination.first = 33;
            destination.second = 8;

            if(doorNearby(destination)){
                nextLocation.first = -1;
                nextLocation.second = -1;
                return nextLocation;
            }
        }else {
            //Get destination workstation location
            for (auto const i:workstationsList) {
                if (i.first == destinationWSNum) {
                    destination = i.second;
                }
            }
            if(WSNearby(destination)){
                nextLocation = location;
                return nextLocation;
            }
        }

        int x_diff = abs(location.first - destination.first);
        int y_diff = abs(location.second - destination.second);

        if(y_diff <= 2 && x_diff > 0) { // x as priority direction
            if (destination.first < location.first) { //move left
                locationChange = navigation(location,'x','-');
            }else{//move right
                locationChange = navigation(location,'x','+');
            }
        }else{ // y as priority direction
            if (destination.second < location.second) { //move up
                locationChange = navigation(location,'y','-');
            }else{//move down
                locationChange = navigation(location,'y','+');
            }
        }

        nextLocation.first = location.first + locationChange.first;
        nextLocation.second = location.second + locationChange.second;

        return nextLocation;
    }

    /*
     * Check if the destination workstation is nearby
     *
     * return true if Workstation nearby
     */
    [[nodiscard]] bool WSNearby(std::pair<int, int> destination) const {
        for(auto const neighbors: state.neighbors_state) {
            if(neighbors.second.type == WORKSTATION) {
                if (neighbors.first[0] == destination.first) {
                    if (neighbors.first[1] == destination.second) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /*
     * Check if the destination DOOR is nearby
     *
     * return true if DOOR is nearby
     */
    [[nodiscard]] bool doorNearby(std::pair<int, int> destination) const{
        for(auto const neighbors: state.neighbors_state) {
            if(neighbors.second.type == DOOR) {
//                if (neighbors.first[0] == destination.first) {
//                    if (neighbors.first[1] == destination.second) {
                        return true;
//                    }
//                }
            }
        }
        return false;
    }

    /*
     * Using movement rules to do the navigation
     *
     * return: the location change
     */
    [[nodiscard]] std::pair<int,int> navigation(std::pair<int,int> location, char priority, char direction) const {
        std::pair<int,int> locationChange;
        locationChange.first = 0;
        locationChange.second = 0;

        int change;
        if(direction == '-'){
            change = -1;
        } else{
            change = 1;
        }

        if(priority == 'x'){
            if(moveCheck(location.first + change, location.second)){
                locationChange.first = change;
            }else if(moveCheck(location.first, location.second + change)){
                locationChange.second = change;
            }else if(moveCheck(location.first, location.second - change)){
                locationChange.second = 0 - change;
            }else if(moveCheck(location.first - change, location.second)){
                locationChange.first = change;
            }
        }else{
            if(moveCheck(location.first, location.second + change)){
                locationChange.second = change;
            }else if(moveCheck(location.first + change, location.second)){
                locationChange.first = change;
            }else if(moveCheck(location.first - change, location.second)){
                locationChange.first = 0 - change;
            }else if(moveCheck(location.first, location.second - change)){
                locationChange.second = change;
            }
        }
        return locationChange;
    }

    /*
     * Check if the next location is occupied
     *
     * return true if it's available to move
     */
    [[nodiscard]] bool moveCheck(int xNext,int yNext) const {
        bool moveCheck = false;
        for(auto const neighbors: state.neighbors_state) {
            if(neighbors.first[0] == xNext){
                if(neighbors.first[1] == yNext){
                    if(neighbors.second.type == AIR) {
                        moveCheck = true;
                    }
                }
            }
        }

        for(auto const student: studentsList){
            if(student.second.first == xNext){
                if(student.second.second == yNext){
                    moveCheck = false;
                }
            }
        }

        return moveCheck;
    }

    // It returns the delay to communicate cell's new state.
    T output_delay(co2 const &cell_state) const override {
        switch(cell_state.type){
            case CO2_SOURCE: return resp_time;
            case SUSCEPTIBLE_CO2_SOURCE: return resp_time;
            default: return 1;
        }
    }

};

#endif //CADMIUM_CELLDEVS_CO2_CELL_HPP













