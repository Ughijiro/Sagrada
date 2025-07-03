#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_PURPLE "\x1b[35m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef enum
{
    RED = 0,
    YELLOW,
    GREEN,
    BLUE,
    PURPLE,
    NUM_COLORS
} Color;

typedef struct
{
    int number;
    Color color;
} Die;

typedef enum
{
    No_restriction = 0,
    Number_restriction,
    Color_restriction
} RestrictionType;

typedef struct
{
    RestrictionType restriction;
    union
    {
        Color color_restriction;
        unsigned int number_restriction;
    } value;
} Restriction;

typedef struct
{
    Die window[4][5];
    Restriction restrictions[4][5];
    int num_helper_gems;
    bool is_free[4][5];
} PlayerWindow;

typedef struct
{
    Color private_goal_color;
    char pub_goals[5][100];
} Goal;

typedef struct
{
    Die dice[90];
    int size; // Tracks the number of dice in the bag
} DiceBag;

const char *colorToString(Color colors)
{
    switch (colors)
    {
    case RED:
        return "R";
    case GREEN:
        return "G";
    case BLUE:
        return "B";
    case YELLOW:
        return "Y";
    case PURPLE:
        return "P";
    default:
        return "-1";
    }
}

void initializeDiceBag(DiceBag *bag)
{
    int index = 0;

    for (int c = 0; c < NUM_COLORS; c++)
    {
        for (int i = 0; i < 18; i++, index++)
        {
            bag->dice[index].color = c;
        }
    }

    for (int i = 0; i < 90; i++)
    {
        bag->dice[i].number = i % 6 + 1;
    }

    bag->size = 90; // Set the total number of dice in the bag
}

void printDiceBagToFile(const DiceBag *bag, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < bag->size; ++i)
    {
        switch (bag->dice[i].color)
        {
        case RED:
            printf(ANSI_COLOR_RED "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case YELLOW:
            printf(ANSI_COLOR_YELLOW "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case GREEN:
            printf(ANSI_COLOR_GREEN "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case BLUE:
            printf(ANSI_COLOR_BLUE "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case PURPLE:
            printf(ANSI_COLOR_PURPLE "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        default:
            printf("%d ", bag->dice[i].number); // No color
        }
        if ((i + 1) % 9 == 0)
        {
            printf("\n"); // New line after every 6 dice
        }
    }

    fclose(file);
}

void shuffleDiceBag(DiceBag *bag)
{
    if (bag == NULL || bag->size <= 1)
    {
        return; // No need to shuffle if the bag is null or contains 1 or fewer dice
    }

    for (int i = bag->size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1); // Random index from 0 to i

        // Swap dice[i] and dice[j]
        Die temp = bag->dice[i];
        bag->dice[i] = bag->dice[j];
        bag->dice[j] = temp;
    }
}

void assignPrivateGoal(Goal *goal, bool *usedColors)
{
    if (goal == NULL)
    {
        printf("Goal structure is null.\n");
        return;
    }

    // Count the number of available colors
    int availableColors = 0;
    for (int i = 0; i < NUM_COLORS; ++i)
    {
        if (!usedColors[i])
        {
            ++availableColors;
        }
    }

    // Check if all colors have been used
    if (availableColors == 0)
    {
        printf("Error: All colors have been used for private goals.\n");
        return;
    }

    // Find an unused color for the private goal
    int colorIndex;
    do
    {
        colorIndex = rand() % NUM_COLORS;
    } while (usedColors[colorIndex]);

    goal->private_goal_color = colorIndex;
    usedColors[colorIndex] = true;
}

void assignPublicGoals(Goal *goal)
{
    if (goal == NULL)
    {
        printf("Goal structure is null.\n");
        return;
    }

    // Define the public goals
    const char *publicGoalStrings[5] = {
        "Set of 3 and 4",
        "Column variety",
        "Row variety",
        "Set of 5 and 6",
        "Test Goal"};

    // Copy the public goal strings into the goal structure
    for (int i = 0; i < 5; ++i)
    {
        strncpy(goal->pub_goals[i], publicGoalStrings[i], 100);
    }
}

Color convert_char_to_color(const char c)
{
    switch (c)
    {
    case 'R':
        return RED;
    case 'Y':
        return YELLOW;
    case 'G':
        return GREEN;
    case 'B':
        return BLUE;
    case 'P':
        return PURPLE;
    default:
        return NUM_COLORS; // Indicates an error/invalid color
    }
}

PlayerWindow load_pattern(const char *pattern)
{
    FILE *input_file = fopen(pattern, "r");
    if (!input_file)
    {
        perror("Error encountered while opening the input file\n");
        exit(EXIT_FAILURE);
    }

    PlayerWindow player = {0}; // Initialize all elements to 0/false

    int c;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            fscanf(input_file, "%d", &c);
            if (c == 0)
            {
                player.restrictions[i][j].restriction = No_restriction;
            }
            else if (c == 1)
            {
                fscanf(input_file, " %d", &player.restrictions[i][j].value.number_restriction);
                player.restrictions[i][j].restriction = Number_restriction;
            }
            else
            {
                char colorChar;
                fscanf(input_file, " %c", &colorChar);
                player.restrictions[i][j].value.color_restriction = convert_char_to_color(colorChar);
                player.restrictions[i][j].restriction = Color_restriction;
            }
        }
    }
    fscanf(input_file, "%d", &player.num_helper_gems);

    fclose(input_file);
    return player;
}

void DicePool(DiceBag *bag)
{
    printf("Choose one out of these dice: \n");
    for (int i = 0; i < 9; i++)
    {
        switch (bag->dice[i].color)
        {
        case RED:
            printf(ANSI_COLOR_RED "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case YELLOW:
            printf(ANSI_COLOR_YELLOW "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case GREEN:
            printf(ANSI_COLOR_GREEN "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case BLUE:
            printf(ANSI_COLOR_BLUE "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        case PURPLE:
            printf(ANSI_COLOR_PURPLE "%d " ANSI_COLOR_RESET, bag->dice[i].number);
            break;
        default:
            printf("%d ", bag->dice[i].number); // No color
        }
    }
    printf("\n");
}

void printPlayerWindowToFile(const PlayerWindow *window, const Goal *goal, const char *filename)
{

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (window->is_free[i][j])
            {
                fprintf(file, "[%s%d] ", colorToString(window->window[i][j].color), window->window[i][j].number);
            }
            else
            {
                switch (window->restrictions[i][j].restriction)
                {
                case No_restriction:
                    fprintf(file, "[   ] ");
                    break;
                case Number_restriction:
                    fprintf(file, "[ %d ] ", window->restrictions[i][j].value.number_restriction);
                    break;
                case Color_restriction:
                    fprintf(file, "[ %s ] ", colorToString(window->restrictions[i][j].value.color_restriction));
                    break;
                }
            }
        }
        fprintf(file, "\n");
    }

    fprintf(file, "Private Goal Color: %s\n\n", colorToString(goal->private_goal_color));
    fprintf(file, "\n");

    fclose(file);
}

Die rollDie(Color color)
{
    Die die;
    die.color = color;
    die.number = rand() % 6 + 1; // Roll a number between 1 and 6
    return die;
}

void placeDie(PlayerWindow *window, int row, int col, Die die)
{
    if (row < 0 || row >= 4 || col < 0 || col >= 5)
    {
        printf("Invalid position.\n");
        return;
    }

    Restriction restriction = window->restrictions[row][col];
    bool canPlace = false;

    if (window->is_free[row][col])
    {
        printf("Cell is not free.\n");
    }
    else
    {
        switch (restriction.restriction)
        {
        case No_restriction:
            canPlace = true;
            break;
        case Number_restriction:
            if (die.number == restriction.value.number_restriction)
                canPlace = true;
            break;
        case Color_restriction:
            if (die.color == restriction.value.color_restriction)
                canPlace = true;
            break;
        }

        if (canPlace)
        {
            window->window[row][col] = die;
            window->is_free[row][col] = true;
        }
        else
        {
            printf("Die does not meet the restriction.\n");
        }
    }
}

void printRoundMarker(int round)
{
    printf("\nRound %d Marker\n", round);
}


int main()
{
    srand(time(NULL));

    return 0;
}
