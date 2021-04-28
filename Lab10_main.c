//Yaroslav Yanin
//yyanin@ucsc.edu
//1620969
//Lab10  extra credit


//CMPE13 Support Library
#include "BOARD.h"
#include "Oled.h"
#include "Adc.h"
#include "Buttons.h"
#include "Leds.h"
#include "Ascii.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>
#include <time.h>

//Declare functions here
void GoalieStateMachine(void);
void BallStateMachine(void);
void UpdateOled();

//Enum for the goalie

typedef enum {
    WAITING,
    MOVE
} state;

//Enum for the game itself

typedef enum {
    IDLE,
    KICK,
    BLOCKED,
    FAILED,
} GameStatus;

//Struct

typedef struct {
    state GoalieMoveEvent; //For the state machine regarding the goalie
    GameStatus GameEvent; //For the main state machine of the game
    uint8_t ButtonEvent; //Stores the result from ButtonsCheckEvents
    int Position; //Position of the goalie
    int BottomLimit; //Bottom limit of the goals
    int UpperLimit; //Upper limit of the goals
    int Middle; //Middle of the goals
    int RandNum; //Random number that determines where the ball is coming from

    int BallAtTheGoals; //If this is equal to 1 then it is the goals
    int BallPositionCounter; //3 points until goals
    int EndCounter; //Breaks the loop when the game is finished
    int PauseAfterPrintingEnd; //So that it does not continue to print the end message forever

    //Score and lives
    int Score;
    int Lives;

} GameData;

GameData data;

int main()
{
    BOARD_Init();
    OledInit();
    ButtonsInit();
    AdcInit();

    //Goalie
    data.GoalieMoveEvent = WAITING; //Beginning of the goalie state machine
    data.BottomLimit = 2;
    data.UpperLimit = 1;
    data.Position = 2;

    //Main state machine
    data.GameEvent = IDLE; //Beginning of the state machine
    data.BallPositionCounter = 0;
    data.BallAtTheGoals = 0;
    data.PauseAfterPrintingEnd = 0;

    //Score
    data.Score = 0;

    //Lives
    data.Lives = 3;


    UpdateOled(data);
    OledUpdate();


    //UpdateOled();

    printf("Welcome to YYANIN's Goalie game.  Compiled on %s %s\n", __DATE__, __TIME__);

    while (1) {
        data.ButtonEvent = ButtonsCheckEvents();

        BallStateMachine();

        if (data.ButtonEvent) {
            GoalieStateMachine();

        }
    }

}

//Function that runs the state machine

void GoalieStateMachine(void)
{

    switch (data.GoalieMoveEvent) {
    case WAITING:
        if (data.ButtonEvent == BUTTON_EVENT_3DOWN || data.ButtonEvent == BUTTON_EVENT_4DOWN) {
            data.GoalieMoveEvent = MOVE;
        }
        break;
    case MOVE:

        if (data.ButtonEvent == BUTTON_EVENT_4UP) {
            if (data.Position == data.BottomLimit) {
                data.Position--;
            }
            UpdateOled(data);

        } else if (data.ButtonEvent == BUTTON_EVENT_3UP) {
            if (data.Position == data.UpperLimit) {
                data.Position++;
            }
            UpdateOled(data);
        }

        data.GoalieMoveEvent = WAITING;

        break;
    }
}

void BallStateMachine(void)
{
    switch (data.GameEvent) {
    case IDLE:
        if (data.EndCounter == 1 && data.PauseAfterPrintingEnd == 0) {
            printf("Your score is %i. Thanks for playing! Press B1 to play again! \n", data.Score);
            data.PauseAfterPrintingEnd = 1;

        } else {
            data.RandNum = rand() % 2; //Gives a random number between 0 and 1
            data.GameEvent = KICK;
        }
        break;
    case KICK:
        data.BallPositionCounter = data.BallPositionCounter + 1;
        if (data.BallPositionCounter > 500000) {
            data.BallPositionCounter = 0;
            data.BallAtTheGoals = 1; //Means that it has reached the goals
        }
        UpdateOled(data);

        //Reset
        if (data.ButtonEvent == BUTTON_EVENT_1UP) {
            data.RandNum = rand() % 2; //Gives a random number between 0 and 1
            data.EndCounter = 0;
            data.PauseAfterPrintingEnd = 0;
            data.Lives = 3;
            data.Score = 0;
            data.GameEvent = KICK;
        }

        if (data.BallAtTheGoals == 1) {
            data.BallAtTheGoals = 0;
            if (data.RandNum == 0 && data.Position == data.BottomLimit) {
                data.GameEvent = BLOCKED;
            } else if (data.RandNum == 1 && data.Position == data.UpperLimit) {
                data.GameEvent = BLOCKED;
            } else {
                data.GameEvent = FAILED;
            }
        }


        break;
    case BLOCKED:
        data.Score = data.Score + 100;
        data.GameEvent = IDLE;
        break;
    case FAILED:
        data.Lives = data.Lives - 1;
        if (data.Lives == 0) {
            data.EndCounter = 1;
        }
        data.GameEvent = IDLE;
        break;
    }

}

void UpdateOled()
{
    char string1[100];
    char string2[100];

    if (data.Lives == 0) {
        OledClear(0);
        sprintf(string1, "End: your score:%i \n  press B1 to\n play again", data.Score);
        OledDrawString(string1);
        OledUpdate();
    } else {
        //Which row is the ball in
        if (data.RandNum == 1) {
            if (data.BallPositionCounter == 0) { //Position 0 until the goals    
                if (data.Position == data.UpperLimit) {
                    OledClear(0);
                    sprintf(string1, "|   L:%i  S:%i   \n $                 *\n     \n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i   \n                 *\n $          \n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            } else if (data.BallPositionCounter == 150000) {
                if (data.Position == data.UpperLimit) {
                    OledClear(0);
                    sprintf(string1, "|   L:%i  S:%i   \n $           *\n     \n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i  \n           *\n $          \n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            } else if (data.BallPositionCounter == 300000) {
                if (data.Position == data.UpperLimit) {
                    OledClear(0);
                    sprintf(string1, "|   L:%i  S:%i  \n $    *\n     \n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i  \n    * \n $          \n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            } else if (data.BallPositionCounter == 450000) {
                if (data.Position == data.UpperLimit) {
                    OledClear(0);
                    sprintf(string1, "|   L:%i  S:%i  \n $*   \n     \n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i  \n*    \n $          \n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            }
        } else if (data.RandNum == 0) {
            if (data.BallPositionCounter == 0) { //Position 0 until the goals    
                if (data.Position == data.UpperLimit) {
                    OledClear(0);
                    sprintf(string1, "|   L:%i  S:%i \n $       \n                 *\n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i  \n      \n $                 *\n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            } else if (data.BallPositionCounter == 150000) {
                if (data.Position == data.UpperLimit) {
                    OledClear(0);
                    sprintf(string1, "|   L:%i  S:%i \n $       \n           *\n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i  \n      \n $           *\n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            } else if (data.BallPositionCounter == 300000) {
                if (data.Position == data.UpperLimit) {
                    OledClear(0);
                    sprintf(string1, "|   L:%i  S:%i \n $       \n    *\n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i  \n      \n $    *\n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            } else if (data.BallPositionCounter == 450000) {
                    OledClear(0);
                if (data.Position == data.UpperLimit) {
                    sprintf(string1, "|   L:%i  S:%i \n $       \n*   \n|", data.Lives, data.Score); //Keeping this much space between is the only way OledUpdate works
                    OledDrawString(string1);
                    OledUpdate();
                } else if (data.Position == data.BottomLimit) {
                    OledClear(0);
                    sprintf(string2, "|   L:%i  S:%i  \n      \n $*   \n|", data.Lives, data.Score);
                    OledDrawString(string2);
                    OledUpdate();
                }
            }
        }
    }

}


