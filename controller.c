#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

char** map;
int** race;
int rows;
int columns;
int start_row;
int start_column;
int laps;

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

//Depth-First Search Algorithm
int dfs(int row, int column) {
	int* current = &race[row][column]; //Current position of vehicle extracted from racetrack array

	if (*current == empty) { //Current position of the vehicle is within the racetrack coordinates
		*current = crumb; //Establish current position as a known location

		if (race[row][column] == crumb) { //Determine known location from racetrack array
			map[row][column] = '.'; //Display known location as a new symbol in map array
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					printf("%c", map[i][j]); //Iterate through map array and display map
				}
				printf("\n");
			}


			//system("cls");
		} else {
			printf("EMERGENCY STOP! Sensor terrain has failed to read data.\n");			
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
		get_race();
		dfs(start_row, start_column);
		reset_vehicle_path();		
	}
}

void print_map() {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int main() {
	get_map("mapFile.txt");
	print_map();
	printf("Enter number of laps: \n");	
	scanf("%d", &laps);
	race_laps(laps);
	return 0;
}