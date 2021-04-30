Dynamic CO2 Producer Expation simulation was implemented and tested in Linux ubuntu terminal using the Cadmium Cell-DEVSE toolkit.
The following will be a description of the folder organisation, followed by step to visualize after running the simulation.

Written by Sebastien Cook 101022643

/***************************************/
Folder Organisation

README -> instructions 
CADMIUM Cell-DEVS Model of CO2 as a Proxy for COVID Infection Within a Dynamic Setting.pdf -> model description, analysis and simulation examples.

Videos -> Contains videos of each simulation using the visualization tool: http://ec2-3-235-245-192.compute-1.amazonaws.com:8080/devs-viewer/app-simple/
Dynamic_susceptible_Lab -> First iteration model.
Dynamic_infected_Lab -> Second iteration model set for the lab configuration (spawn point must be chaged in the AIR type if a new scenario is presented).
Dynamic_infected_Buss -> Second iteration model set for the buss configuration.
Dynamic_infected_complex_model_Lab -> Third iteration model set for the lab configuration.
Dynamic_infected_complex_model_buss -> Third iteration model set for the buss configuration.
	Withing each model folder above:
	CMakeLists -> File to be executed first to create make file. DO NOT DELET.
	Config -> Contains .json simulation configuraton file.
	model -> Contains the model implementation files.
	visualization -> Contains pallet file and visualisation ready folder of simulations presented in the .pdf document.
		style_Options -> Palet Styles options that must be inserted into options file for proper visualisation. 
		Folder(s) within -> Containd visualization ready files.
/***************************************/

Simulation and visualization instructions

1. Place entire folder from the model selection above in the Cadmium Cell-DEVSE environment

2. Open terminal in folder directory. ex. ../Dynamic_susceptible_Lab

3. Run following command: cmake ./

4. Drag or create "results" folder in "bin" folder

5. Run following command: make

6. Open terminal in bin directory: cd bin

7. Run following command: ./co2_lab ../config/name_of_configuration_file.json

8. Verify that output_messages.txt and state.txt present in ../bin/results folder

9. Open visualization tool: http://ec2-3-235-245-192.compute-1.amazonaws.com:8080/devs-viewer/app-simple/

10. Add output_messages.txt and configuration.json file and load simulation

11. Click "Download normalized simulation files" bellow the tool button

12. Extract normalized simulation folder and open options.json

13. Open stule_options in the config folder, copy all, and replace: "styles":[{...}] 

14. save file and reopen visualization tool

15. Click load simulation, remove previous files and now upload: messages.log, options.json, structure.json

16. Load simulation

17. Open tools, modify grids, change pot styles to: counter -> 0, concentration -> 1, type -> 2, riskyDistance -> 3 (if available)

18. Click apply and enjoy the simulation visualization


 

