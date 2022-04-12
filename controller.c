#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

char** map;
int** race;
int rows;
int columns;
int start_row;
int start_column;
int current_lap;
int speed = -1; //Sensor - speed

enum terrain {
	empty,
	wall,
	crumb
};

//Store map into an array within allocated memory
void alloc_map() {
	map = malloc(rows * sizeof(char*));

	for (int i = 0; i < rows; i++) {
		map[i] = malloc(columns * sizeof(char*));		
	}
}

//Read Map file
void get_map(char* filename) {
	char character;
	char rows_s[3] = { '\0' };
	char columns_s[3] = { '\0' };
	int rows_i = 0;
	int columns_i = 0;
	int swap = 0;

	FILE* file = fopen(filename, "r"); //Open map file in read-only mode
	if (file) {
		while((character = getc(file)) != EOF) { //Read file until end of file reached
			if (character == '\n') { //Skip processing when new line occurs
				break;
			} else if (character == ',') { //Read delimiter between number of rows and columns on first line of map file
				swap = 1;
			} else if (!swap) { //Read number of rows
				rows_s[rows_i] = character; //Store each digit of number of rows in array
				rows_i++;
			} else { //Read number of columns
				columns_s[columns_i] = character; //Store each digit of number of columns in array
				columns_i++;
			}
		}
	} else {
		printf("The map file could not be found at the given location.");
	}

	rows = atoi(rows_s); //Retain number of rows as an integer
	columns = atoi(columns_s); //Retain number of columns as an integer

	alloc_map();

	for(int i = 0; i < rows; i++) { //Convert map file into usable data
		for (int j = 0; j < columns; j++) {
			character = getc(file);

			if (character == '\n') {
				character = getc(file);
			}

			//Characters retrieved from map file can be modified and corresponding conversion mechanism below can be updated
			if (character == 'g') { //Convert endpoint of racetrack as looped path to connect to start point
				map[i][j] = ' ';
			} else if ((character == ' ') || (character == '+')){
				map[i][j] = character; //Store each character from map file into array
			} else if (character == 's') { //Retain position of start point from map file
				start_row = i;
				start_column = j;
			} else {
				printf("The map is not the expected file type and cannot be stored or converted.\n");
			}
		}
	}

	fclose(file); //Close map file after reading data
}

//Store racetrack into an array within allocated memory
void alloc_race() {
	race = malloc(rows * sizeof(char*));

	for (int i = 0; i < rows; i++) {
		race[i] = malloc(columns * sizeof(char*));
	}
}

//Read map array
void get_race() {
	alloc_race();

	//Iterate through the map array
	for(int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {			
			//Characters retrieved from map file can be modified and corresponding conversion mechanism below can be updated
			if (map[i][j] == '+') {
				race[i][j] = wall; //Identify boundaries of the map and place a corresponding integer value into racetrack array
			} else {
				race[i][j] = empty; //Identify racetrack in the map and place a corresponding integer value into racetrack array
			}
		}
	}
}

int turn_direction(int row, int column) {  //Determine left/right turns
	int current = race[row][column]; //State of current position of vehicle extracted from racetrack array	
	int up = race[row - 1][column]; //State of up unit relative current position of vehicle extracted from racetrack array
	int left = race[row][column - 1]; //State of left unit relative current position of vehicle extracted from racetrack array

	if (up == crumb) { //Identify intersection for turning right
		if (left == empty) {
			return 2; //Turn right - This integer value can be replaced with any integer value that is not the same as the opposing integer value in this function
		}
	}

	return 1; //Turn Left - This integer value can be replaced with any integer value that is not the same as the opposing integer value in this function
}

int corner_track(int row, int column) { //Determine left/right turns by finding intersections in racetrack
	int current = race[row][column]; //State of current position of vehicle extracted from racetrack array
	int right = race[row][column + 1]; //State of right unit relative current position of vehicle extracted from racetrack array
	int up = race[row - 1][column]; //State of up unit relative current position of vehicle extracted from racetrack array
	int left = race[row][column - 1]; //State of left unit relative current position of vehicle extracted from racetrack array
	int down = race[row + 1][column]; //State of down unit relative current position of vehicle extracted from racetrack array

	//Modification of identifying corners in the track requires listing all possible states of entering an intersection, then selecting required states of entering an intersection based on the map file then processing correct turn
	if (((right == crumb) || (right == empty)) && ((down == crumb) || (down == empty)) && (up == wall) && (left == wall)) { //Right direction, Down direction are vacant options at intersection
		return turn_direction(row, column); //Turn Left, Turn Left
	} else if (((up == crumb) || (up == empty)) && ((left == crumb) || (left == empty)) && (right == wall) && (down == wall)) {//Up direction, Left direction are vacant options at intersection
		return turn_direction(row, column); //Turn Left, Turn Right	
	} else if (((left == crumb) || (left == empty)) && ((down == crumb) || (down == empty)) && (up == wall) && (right == wall)) {//Left direction, Down direction are vacant options at intersection
		return turn_direction(row, column); //Turn Left
	} else if (((right == crumb) || (right == empty)) && ((up == crumb) || (up == empty)) && (left == wall) && (down == wall)) {//Right direction, Up direction are vacant options at intersection
		return turn_direction(row, column); //Turn Left
	}

	return 0;
}

void apply_brakes(int brake) { //Control brakes output
	if (brake == 1) {  //This integer value can be replaced with any integer value that is not the same as the opposing integer value in this function
		printf("BRAKE\n"); //Vehicle slows by engaging brakes
	} else if (brake == 0) { //This integer value can be replaced with any integer value that is not the same as the opposing integer value in this function
		printf("ACCELERATE\n"); //Vehicle increases speed by engaging accelerator
	} else {
		printf("Sensor braking/acceleration has failed to read data.\n"); //Brake sensor failure
		brake = 1; //Apply brakes for emergency stopping - Ensure this integer value is the same as the first integer value compared in this function
		printf("EMERGENCY BRAKE!\n");
	}
}

void apply_steering(int steer) { //Control steering output
	if (steer == 1) { //This integer value can be replaced with any integer value that is not the same as the opposing integer value in this function
		printf("Turn Left\n"); //Steer vehicle in left direction
	} else if (steer == 0) { //This integer value can be replaced with any integer value that is not the same as the opposing integer value in this function
		printf("Turn Right\n");  //Steer vehicle in right direction
	} else {
		printf("Sensor steering has failed to read data.\n"); //Steer sensor failure
		apply_brakes(1); //Apply brakes for emergency stopping - Ensure this integer value is the same as the first integer value compared in the apply_brakes function
		printf("EMERGENCY STOP!\n");
	}
}

//Depth-First Search Algorithm
int dfs(int row, int column) {
	int* current = &race[row][column]; //Current position of vehicle extracted from racetrack array
	int corner_found = 0; //Determine corners of the racetrack
	int steer_left = -1; //Determine turn direction from corners of the racetrack (Default: No turn)

	if (*current == empty) { //Current position of the vehicle is within the racetrack coordinates
		*current = crumb; //Establish current position as a known location

		if (race[row][column] == crumb) { //Determine known location from racetrack array
			map[row][column] = '.'; //Display known location as a new symbol in map array
			
			if (corner_track(row, column) == 1) { //Identify corner of racetrack - Ensure this integer value is the same as the integer value identifying left turn in the turn_direction function
		 		map[row][column] = 'o'; //Display position identified as corner with new symbol in map array
		 		corner_found = 1; //Retain knowledge of finding a corner in racetrack
		 		steer_left = 1; //Retain knowledge of turning left at this corner in racetrack
		 	} else if (corner_track(row, column) == 2) { //Identify corner of racetrack - Ensure this integer value is the same as the integer value identifying right turn in the turn_direction function
		 		map[row][column] = 'o'; //Display position identified as corner with new symbol in map array
		 		corner_found = 1; //Retain knowledge of finding a corner in racetrack
		 		steer_left = 0; //Retain knowledge of turning right at this corner in racetrack	 		
		 	}

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					printf("%c", map[i][j]); //Iterate through map array and display map
				}
				printf("\n");
			}

			printf("Vehicle current position: (%d, %d)\n", row, column); //Display vehicle current position
			printf("Lap %d\n", current_lap); //Display current lap vehicle is racing

			if (corner_found && steer_left) { //Turn left at a corner
		 		printf("Corner found!\n");
		 		apply_brakes(1); //Apply brakes - Ensure this integer value is the same as the integer value identifying applying brakes in the apply_brakes function
		 		apply_steering(steer_left); //Turn steering wheel left
		 		speed -= 5; //Reduce speed - This integer value can be replaced by any integer
		 	} else if (corner_found && !steer_left){ //Turn right at a corner
		 		printf("Corner found!\n");
		 		apply_brakes(1); //Apply brakes - Ensure this integer value is the same as the integer value identifying applying brakes in the apply_brakes function
		 		apply_steering(steer_left); //Turn steering wheel right
		 		speed -= 5; //Reduce speed - This integer value can be replaced by any integer
		 	} else { //Vehicle travelling on straight section of racetrack
		 		apply_brakes(0); //Apply accelerator - Ensure this integer value is the same as the integer value identifying disengaging brakes in the apply_brakes function
		 		if (speed <= 100) { //Increase speed (limited to 105km/h) - This integer value can be replaced by any integer
		 			speed += 5; //This integer value can be replaced by any integer
		 		}
		 	}

		 	printf("Speed %d\n", speed); //Display speed
			//system("cls");
		} else {
			printf("EMERGENCY STOP! Sensor terrain has failed to read data.\n");			
			apply_brakes(1); //Apply brakes - Ensure this integer value is the same as the integer value identifying applying brakes in the apply_brakes function
			exit(1);
		}

		if (dfs(row, (column + 1))) { //Check one unit right of current position
			*current = crumb; //Establish current position as known location for racetrack
			return 1;
		}

		if (dfs((row - 1), column)) { //Check one unit above current position
			*current = crumb; //Establish current position as known location for racetrack
			return 1;
		}

		if (dfs(row, (column - 1))) { //Check one unit left of current position
			*current = crumb; //Establish current position as known location for racetrack
			return 1;
		}

		if (dfs((row + 1), column)) { //Check one unit below current position
			*current = crumb; //Establish current position as known location for racetrack
			return 1;
		}
	}

	return 0; //Boundary has been detected
}

void reset_vehicle_path() { //Recalculate path upon completing each lap/race
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			//Characters reset within map array file can be modified by updating corresponding conversion mechanism below
			if (race[i][j] == crumb) { //Find known travel points of vehicle - update path array comparison		
				map[i][j] = ' '; //Update character that re-initialises map array
			}
		}
	}
}

void race_laps(int laps) { //Display vehicle progressing through racetrack
	for (int i = 0; i < laps; i++) { //Calculate path with algorithm during each race
		current_lap = i + 1;
		get_race();
		dfs(start_row, start_column);
		reset_vehicle_path();		
	}
}

int main() {
	int laps;
	speed = 0;
	get_map("mapFile.txt");	
	printf("Enter number of laps: \n");	
	scanf("%d", &laps);
	race_laps(laps);
	apply_brakes(1); //Apply brakes after completing session
	while (speed > 0) { //Reduce speed
		if (speed < 10) {
			speed -= 1;
		} else {
			speed -= 10;
		}
	}
	printf("Speed %d\n", speed); //Vehicle now stationary, speed is displayed
	return 0;
}