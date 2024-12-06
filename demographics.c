#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LENGTH 100
#define MAX_LINE_LENGTH 1024
#define COUNTRY_ENTRIES 3143

// Structure definition (move this to the top)
typedef struct {
    // general info
    char county[STRING_LENGTH];
    char state[STRING_LENGTH];

    // education
    float education_bachelor_or_higher;
    float education_high_school_or_higher;

    // ethnicities
    float ethnicities_american_indian_and_alaska_native;
    float ethnicities_asian;
    float ethnicities_black;
    float ethnicities_hispanic_or_latino;
    float ethnicities_native_hawaiian_and_other_pacific_islander;
    float ethnicities_two_or_more_races;
    float ethnicities_white_alone;
    float ethnicities_white_alone_not_hispanic_or_latino;

    // income
    long int income_median_household;
    long int income_per_capita;
    float income_persons_below_poverty_level;

    // population in 2014
    int population_2014;
} Demographics;

// Function declarations (must be declared before use)
int load_data(char *filename, Demographics **data);
int parse_entry(char *line, Demographics *entry);
void remove_char(char *str, char c);

void execute_operation(char *line, Demographics *data, int *count);
int execute_operations_file(char *filename, Demographics *data, int *count);
void display_data(Demographics *data);
void filter_state(Demographics *data, int *count, char *state);
void filter_field(Demographics *data, int *count, char *field, char *comparison, float number);

int population_total(Demographics *data, int *count);
float population_field(Demographics *data, int *count, char *field);
void percent_field(Demographics *data, int *count, char *field);

// Load data 
int load_data(char *filename, Demographics **data) {
    FILE *file = fopen(filename, "r"); //open file
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    int count = 0;

    // memory for the data array
    *data = malloc(COUNTRY_ENTRIES * sizeof(Demographics));
    if (*data == NULL) {
        perror("Memory allocation error.");
        fclose(file);
        return -1;
    }

    // Read the file line by line
    while (fgets(line, sizeof(line), file)) {
        line_num++; //increment number of lines

        // Skip the first line
        if (line_num == 1) {
            continue;
        }

        Demographics entry; //struct for one entry
        // Parse into the structure
        if (parse_entry(line, &entry) == -1) {
            printf("Invalid formating at line %d.\n", line_num);
            continue;
        }

        // Store the parsed entry
        (*data)[count++] = entry;
    }

    
    fclose(file);
    return count; //entry count
}

int parse_entry(char *line, Demographics *entry) {
    // Array to store each separated value
    char *tokens[100];  // Assumes maximum of 100 values for a line

    // Tokenize the line by commas
    remove_char(line, '"');
    char *token = strtok(line, ",");
    int token_count = 0;
    
    while (token != NULL) {
        // Strip quotes and whitespace and store in the array
        tokens[token_count++] = token;

        token = strtok(NULL, ",");
    }

    // Now tokens array contains all separated values

    if (token_count < 41) { //the 39 element is population in 2014
        // Not enough tokens in the line
        return -1;
    }

    // Extract values into the structure from the tokens array
    // We need only the first 19, and 26-28 and the 39th
    strncpy(entry->county, tokens[0], STRING_LENGTH);
    strncpy(entry->state, tokens[1], STRING_LENGTH);

    // Parse the relevant numeric fields
    entry->education_bachelor_or_higher = atof(tokens[5]);
    entry->education_high_school_or_higher = atof(tokens[6]);
    entry->ethnicities_american_indian_and_alaska_native = atof(tokens[11]);
    entry->ethnicities_asian = atof(tokens[12]);
    entry->ethnicities_black = atof(tokens[13]);
    entry->ethnicities_hispanic_or_latino = atof(tokens[14]);
    entry->ethnicities_native_hawaiian_and_other_pacific_islander = atof(tokens[15]);
    entry->ethnicities_two_or_more_races = atof(tokens[16]);
    entry->ethnicities_white_alone = atof(tokens[17]);
    entry->ethnicities_white_alone_not_hispanic_or_latino = atof(tokens[18]);

    // Parse income fields
    //printf(",%s,%s,\n", tokens[25], tokens[26]);
    entry->income_median_household = atoi(tokens[25]);
    
    //printf("%d",entry->income_median_household);
    entry->income_per_capita = atoi(tokens[26]);
    entry->income_persons_below_poverty_level = atof(tokens[27]);

    // Parse population in 2014
    entry->population_2014 = atoi(tokens[38]);

    return 0;
}

void remove_char(char *str, char c) {
    int len = strlen(str);
    int j = 0;
    int i = 0;
    for (i = 0; i < len; i++) {
        if (str[i] != c) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';  // Null-terminate the modified string
}

void display(Demographics *data, int *count){
    for(int i = 0; i < *count; i++){
        display_data(&data[i]);
    }
    return;
}

void display_data(Demographics *data) {
    // Print the county and state
    printf("%s, %s\n", data->county, data->state);
    
    // Print the population
    printf("\tPopulation: %d\n", data->population_2014);
    
    // Print the Education section
    printf("\tEducation\n");
    printf("\t\t>= High School: %.6f\n", data->education_high_school_or_higher);
    printf("\t\t>= Bachelor's: %.6f\n", data->education_bachelor_or_higher);
    
    // Ethnicity 
    printf("\tEthnicity Percentages\n");
    printf("\t\tAmerican Indian and Alaska Native: %.6f\n", data->ethnicities_american_indian_and_alaska_native);
    printf("\t\tAsian Alone: %.6f\n", data->ethnicities_asian);
    printf("\t\tBlack Alone: %.6f\n", data->ethnicities_black);
    printf("\t\tHispanic or Latino: %.6f\n", data->ethnicities_hispanic_or_latino);
    printf("\t\tNative Hawaiian and Other Pacific Islander Alone: %.6f\n", data->ethnicities_native_hawaiian_and_other_pacific_islander);
    printf("\t\tTwo or More Races: %.6f\n", data->ethnicities_two_or_more_races);
    printf("\t\tWhite Alone: %.6f\n", data->ethnicities_white_alone);
    printf("\t\tWhite Alone, not Hispanic or Latino: %.6f\n", data->ethnicities_white_alone_not_hispanic_or_latino);
    
    //Income section
    printf("\tIncome\n");
    printf("\t\tMedian Household: %d\n", data->income_median_household);
    printf("\t\tPer Capita: %d\n", data->income_per_capita);
    printf("\t\tBelow Poverty Level: %.6f\n", data->income_persons_below_poverty_level);
    return;
}


int execute_operations_file(char *filename, Demographics *data, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening operations file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    
    // Read each line of the operations file
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // Skip empty lines or comments
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }
        
        execute_operation(line, data, count);
    }

    fclose(file);
    return 0;
}

void execute_operation(char *line, Demographics *data, int *count) {
    // Handle different operations
    char c = '\n';
    remove_char(line, c);
    if (strncmp(line, "display", 7) == 0) {
        display(data, count);
    } 
    else if (strncmp(line, "filter-state:", 13) == 0) {
        // filter-state:<state_abbreviation>
        char *state = line + 13; //skip the first 13 char
        filter_state(data, count, state);
    } 
    else if (strncmp(line, "filter:", 7) == 0) {
        // filter:<field>:<ge/le>:<number>
        char *field = strtok(line+7, ":");
        char *comparison = strtok(NULL, ":");
        char *number_str = strtok(NULL, "");

        // Convert the number to a float
        float number = atof(number_str);
        if (!number) {
        // If there are remaining characters after the number, it is an invalid number
            printf("Invalid number\n");
        } else {
            filter_field(data, count, field, comparison, number);
        }
    } 
    else if (strcmp(line, "population-total") == 0) {
        // population-total
        int total_pop = population_total(data, count);
        if(total_pop){
            printf("2014 population: %d\n", total_pop);
        }else{
            printf("Error calculation total population.");
        }
    }
    else if (strncmp(line, "population:", 11) == 0) {
        char *field = line + 11;
        float pop_field = population_field(data, count, field);
        if(pop_field){
            printf("2014 %s population: %.6f\n", field, pop_field);
        }else{
            printf("Unsupported field.");
        }
    }
    else if (strncmp(line, "percent:", 8) == 0) {
        // percent:<field>
        char *field = line + 8;
        percent_field(data, count, field);
    return;
}
}

void filter_state(Demographics *data, int *count, char *state) {
    int remaining_elements = 0;

    // Loop through the set and count
    for (int i = 0; i < *count; i++) {
        if (strcmp(data[i].state, state) == 0) {
            remaining_elements++;  // Increment the counter for matches
        }
    }
    if (remaining_elements == 0) {
        printf("Filter: state == %s (0 entries)\n", state);
        return;
    }

    // Move matching entries to the start of the array
    int j = 0;
    for (int i = 0; i < *count; i++) {
        if (strcmp(data[i].state, state) == 0) {
            data[j++] = data[i];  // Overwrite the original data with matching entries
        }
    }

    // Update the count to the number of matching entries
    *count = remaining_elements;
    
    printf("Filter: state == %s (%d entries)\n", state, remaining_elements);
    //display(data, count);

    return;
}

void filter_field(Demographics *data, int *count, char *field, char *comparison, float number) {
    int remaining_elements = 0;

    for (int i = 0; i < *count; i++) {
        float field_value = 0.0;  
        
        // find the correct field
        if (strcmp(field, "Education.High School or Higher") == 0) {
            field_value = data[i].education_high_school_or_higher;
        } else if (strcmp(field, "Education.Bachelor's Degree or Higher") == 0) {
            field_value = data[i].education_bachelor_or_higher;
        } else if (strcmp(field, "Income.Median Household") == 0) {
            field_value = (float)data[i].income_median_household;  
        } else if (strcmp(field, "Income.Per Capita") == 0) {
            field_value = (float)data[i].income_per_capita;  
        } else if (strcmp(field, "Income.Persons Below Poverty Level") == 0) {
            field_value = data[i].income_persons_below_poverty_level;
        } else if (strcmp(field, "Ethnicities.American Indian and Alaska Native Alone") == 0) {
            field_value = data[i].ethnicities_american_indian_and_alaska_native;
        } else if (strcmp(field, "Ethnicities.Asian Alone") == 0) {
            field_value = data[i].ethnicities_asian;
        } else if (strcmp(field, "Ethnicities.Black Alone") == 0) {
            field_value = data[i].ethnicities_black;
        } else if (strcmp(field, "Ethnicities.Hispanic or Latino") == 0) {
            field_value = data[i].ethnicities_hispanic_or_latino;
        } else if (strcmp(field, "Ethnicities.Native Hawaiian and Other Pacific Islander Alone") == 0) {
            field_value = data[i].ethnicities_native_hawaiian_and_other_pacific_islander;
        } else if (strcmp(field, "Ethnicities.Two or More Races") == 0) {
            field_value = data[i].ethnicities_two_or_more_races;
        } else if (strcmp(field, "Ethnicities.White Alone") == 0) {
            field_value = data[i].ethnicities_white_alone;
        } else if (strcmp(field, "Ethnicities.White Alone, not Hispanic or Latino") == 0) {
            field_value = data[i].ethnicities_white_alone_not_hispanic_or_latino;
        } else {
            printf("Unsupported field '%s'\n", field);
            return;
        }

        // comparison operator
        int matches = 0;
        if (strcmp(comparison, "ge") == 0) {
            if (field_value >= number) {
                matches = 1;
            }
        } else if (strcmp(comparison, "le") == 0) {
            if (field_value <= number) {
                matches = 1;
            }
        } else {
            printf("Unsupported comparison %s\n", comparison);
            return;
        }

        // move the entry to the filtered
        if (matches) {
            data[remaining_elements++] = data[i];  // overwrite with matching entries only
        }
    }

    // update count 
    *count = remaining_elements;

    printf("Filter: %s %s %.6f (%d entries)\n", field, comparison, number, remaining_elements);
    return;
}

int population_total(Demographics *data, int *count) {
    int total_population = 0;

    for (int i = 0; i < *count; i++) {
        total_population += data[i].population_2014;
    }

    return total_population;
}

float population_field(Demographics *data, int *count, char *field) {
    float total_value = 0.0;  // store the sum
    // loop through the data and compare based on field
    for (int i = 0; i < *count; i++) {
        float field_value = 0.0;

        if (strcmp(field, "Education.High School or Higher") == 0) {
            field_value = data[i].education_high_school_or_higher;
        } else if (strcmp(field, "Education.Bachelor's Degree or Higher") == 0) {
            field_value = data[i].education_bachelor_or_higher;
        } else if (strcmp(field, "Income.Median Household") == 0) {
            field_value = (float)data[i].income_median_household;
        } else if (strcmp(field, "Income.Per Capita") == 0) {
            field_value = (float)data[i].income_per_capita;
        } else if (strcmp(field, "Income.Persons Below Poverty Level") == 0) {
            field_value = data[i].income_persons_below_poverty_level;
        } else if (strcmp(field, "Ethnicities.American Indian and Alaska Native Alone") == 0) {
            field_value = data[i].ethnicities_american_indian_and_alaska_native;
        } else if (strcmp(field, "Ethnicities.Asian Alone") == 0) {
            field_value = data[i].ethnicities_asian;
        } else if (strcmp(field, "Ethnicities.Black Alone") == 0) {
            field_value = data[i].ethnicities_black;
        } else if (strcmp(field, "Ethnicities.Hispanic or Latino") == 0) {
            field_value = data[i].ethnicities_hispanic_or_latino;
        } else if (strcmp(field, "Ethnicities.Native Hawaiian and Other Pacific Islander Alone") == 0) {
            field_value = data[i].ethnicities_native_hawaiian_and_other_pacific_islander;
        } else if (strcmp(field, "Ethnicities.Two or More Races") == 0) {
            field_value = data[i].ethnicities_two_or_more_races;
        } else if (strcmp(field, "Ethnicities.White Alone") == 0) {
            field_value = data[i].ethnicities_white_alone;
        } else if (strcmp(field, "Ethnicities.White Alone, not Hispanic or Latino") == 0) {
            field_value = data[i].ethnicities_white_alone_not_hispanic_or_latino;
        } else {
            printf("Unsupported field %s\n", field);
            return -1;  // for invalid field
        }
        total_value += (float)(data[i].population_2014 *(field_value/100.0));
    }  // Sum 
    

    return total_value;  
}

void percent_field(Demographics *data, int *count, char *field) {
    int total_population = population_total(data, count);

    float field_value = population_field(data, count, field);
    
    if (field_value == -1) {
        printf("Invalid field %s.\n", field);
        return;  // invalid field
    }

    float percentage = (field_value / total_population) * 100;

    // Print in the required format
    printf("2014 %s percentage: %.6f\n", field, percentage);
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        perror("Invalid argument count.\n");
        return 1;
    }

    Demographics *information = NULL;  // pointer to store the loaded data
    int count = load_data(argv[1], &information);
    
    if(count == -1) {
        perror("Error loading data from file\n");
        return 1;
    }

    printf("%d records loaded\n", count);

    if(execute_operations_file(argv[2], information, &count)) {
        perror("Error: failed to process operations\n");
        free(information);  // Free memory
        return 1;
    }

    free(information);  // Free dynamic memory
    return 0;
}
