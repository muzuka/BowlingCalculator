
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Calculates bowling score
// Assuming 10 pins
// Input by argument

// By default computes two predetermined scores.

// Input should only contain the values in validChars below.

// expected: 103
char* scoreOne = "9/7/-8-56--77115XX14";
// expected: 196
char* scoreTwo = "9/X3/8/8/7/8/XX9/6";

char validChars[] = {'-', '1', '2', '3', '4', '5', '6', '7', '8', '9', '/', 'X'};
char validNums[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

char* rounds[10];
int   scores[10];

int debug = 0;

// returns segment of string
// method from Flesym in forum: http://www.linuxquestions.org/questions/programming-9/extract-substring-from-string-in-c-432620/
char* substring(char* str, int start, int end) {
  const char* from = str;
  char* to = (char*) malloc(end-start);
  return strncpy(to, from+start, end-start);
}

// Decides whether character is in an array
// Returns 1 when found, 0 otherwise
int isInArray(char ch, char arr[]) {
  int i;

  for(i = 0; i < 12; i++) {
    if(ch == arr[i]) {
      return 1;
    }
  }
  return 0;
}


// Decides whether all elements of the argument are in the array validChars
// Returns 1 when all are valid, 0 otherwise
int checkValid(char* score) {
  int i;
  
  for(i = 0; i < strlen(score); i++) {
    if(isInArray(score[i], validChars) == 0) {
      return 0;
    }
  }
  return 1;
}

// Decides whether a round is valid
// -/ and /# are invalid regular round scores
// X/ and #X are invalid 10th round scores
int checkRound(char* score) {
  // if round 10

  if(strlen(score) == 3) {

    // if X/ occurs return false
    // A spare cannot follow a strike
    if((score[0] == 'X' && score[1] == '/') || (score[1] == 'X' && score[2] == '/')) {
      return 0;
    }

    // if an incomplete throw is followed by a strike return false
    // A strike cannot follow a throw less than 10.
    else if((score[1] == 'X' && isInArray(score[0], validNums) == 1) || (score[2] == 'X' && isInArray(score[1], validNums) == 1)) {
      return 0;
    }

    // if all throws are incomplete
    else if(isInArray(score[0], validNums) == 1 && isInArray(score[1], validNums) == 1 && isInArray(score[2], validNums) == 1) {
      // if the sum is greater than 10
      if((atoi(substring(score, 0, 1)) + atoi(substring(score, 1, 2)) + atoi(substring(score, 2, 3))) >= 10) {
		    return 0;
      }
      else {
	      return 1;
      }
    }

    // if first char is a /
    // first throw cannot be a spare
    else if(score[0] == '/') {
      return 0;
    }
    else {
      return 1;
    }
  }
  // if a normal round
  else if(strlen(score) == 2) {

    // if round begins with a spare
    if(score[0] == '/' && isInArray(score[1], validNums) == 1) {
      return 0;
    }
    // if two spares are in one round
    else if(score[0] == '/' && score[1] == '/') {
      return 0;
    }
    // if both throws are incomplete
    else if(isInArray(score[0], validNums) == 1 && isInArray(score[1], validNums) == 1) {
      // if sum of throws is greater than 10
      if(atoi(substring(score, 0, 1)) + atoi(substring(score, 1, 2)) >= 10) {
	     return 0;
      }
      else {
		    return 1;
      }
    }
    else {
      return 1;
    }
  }
  // error
  else {
    return -1;
  }
}

// Evaluates the score of a single throw
// params: the score character 
//         the round number
//         the throw number
//         includeBonus, is a flag for whether the extra points should be calculated for a strike or a spare
// returns: the score of the throw or -1 for an error
int evalThrow(char throw, int pos, int place, int includeBonus) {
  int t;

  if(debug == 1) {
    printf("%s%d\n", "evalThrow: Evaluating a throw: ", pos);
  }
  // test throw
  switch(throw) {
    // if a strike
  case 'X':
    // if last round
    if(pos == 9 || includeBonus == 0) {
      return 10;
    }
    else {
      // if next round isn't strike
      if(rounds[pos+1][0] != 'X') {
	     return 10 + evalThrow(rounds[pos+1][0], pos+1, 1, 0) + evalThrow(rounds[pos+1][1], pos+1, 2, 0);
      }
      else {
	    // if next round is the last round
	    if(pos+1 == 9) {
	      return 10 + evalThrow(rounds[pos+1][0], pos+1, 1, 0) + evalThrow(rounds[pos+1][1], pos+1, 2, 0);
    	}
	    else {
	      return 10 + evalThrow(rounds[pos+1][0], pos+1, 1, 0) + evalThrow(rounds[pos+2][0], pos+2, 1, 0);
	    }
      }
    }
  case '/':
    // if last round
    if(pos == 9 || includeBonus == 0) {
      // spare can only happen in the 2nd or 3rd throws
      if(place == 2) {
	     return 10 - atoi(substring(rounds[pos], 0, 1));
      }
      else if(place == 3) {
	     return 10 - atoi(substring(rounds[pos], 1, 2));
      }
    }
    else {
      return (10 - atoi(substring(rounds[pos], 0, 1))) + evalThrow(rounds[pos+1][0], pos+1, 1, 0);
    }
    // if a gutterball
  case '-':
    return 0;
    // if an incomplete throw
  case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':

    t = throw - '0';
    return t;
    // else error
  default:
    return -1;
  }
}

// Calculates the score of a round
// param: string of the score
//        number of round
int calculateRoundScore(char* score, int pos) {
  if(pos == 9) {
    return evalThrow(score[0], pos, 1, 0) + evalThrow(score[1], pos, 2, 0) + evalThrow(score[2], pos, 3, 0);
  }
  else {
    if(score[0] != 'X') {
      return evalThrow(score[0], pos, 1, 1) + evalThrow(score[1], pos, 2, 1);
    }
    else {
      return evalThrow(score[0], pos, 1, 1);
    }
  }
}

// Calculates the final score of the game.
int calculateScore(char* score) {
  int i;
  int sum = 0;
  int round = 0;
  int scorelength= strlen(score);
  
  // if score contains only valid characters
  if(checkValid(score) == 1) {
 
    // extract rounds
    for(i = 0; i < scorelength; i++) {
      // Round size: 3 when last
      if(round == 9) {
	    rounds[round] = substring(score, scorelength-3, scorelength);
	    break;
      }
      // round size: 1 when a strike
      if(score[i] == 'X') {
	    rounds[round] = substring(score, i, i+1);
	    round += 1;
      }
      // else a normal round size: 2
      else {
	    rounds[round] = substring(score, i, i+2);
	    i += 1;
	    round += 1;
      }
    }

    // Check round validity (errors) 
    for(i = 0; i < 10; i++) {
      if(checkRound(rounds[i]) == 0) {
				printf("Invalid Round!\n");
				return -1;
      }
      else {
				scores[i] = calculateRoundScore(rounds[i], i);
      }
    }
    // sum the scores
    for(i = 0; i < 10; i++) {
      sum += scores[i];
    }
    return sum;
  }
  else {
		printf("Invalid Score!\n");
    return -1;
  }
}

// main function
int main(int argc, char * argv[]) {
  
  if(debug == 1) {
    printf("%d\n", calculateScore(scoreOne));
    printf("%s\n", "I didn't seg fault");
  }
  else {
    if(argc >= 2) {
      printf("%d\n", calculateScore(argv[1]));
    }
    else {
      printf("%d\n", calculateScore(scoreOne));
      printf("%d\n", calculateScore(scoreTwo));
    }
  }
}
