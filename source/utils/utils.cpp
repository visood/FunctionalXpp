#include "util.h"

std::string wordyOneToNine(uint x) {
  if (x > 9) throw std::invalid_argument(
      "wordyOneToNine cannot wordify number larger than 9");
  if (x == 0) throw std::invalid_argument(
      "wordyOneToNine cannot wordify 0");
  switch (x) {
  case 9:
    return "nine";
  case 8:
    return "eight";
  case 7:
    return "seven";
  case 6:
    return "six";
  case 5:
    return "five";
  case 4:
    return "four";
  case 3:
    return "three";
  case 2:
    return "two";
  case 1:
    return "one";
  case 0:
    return "";
  }
  return "";
}

std::string wordyInteger(uint x) {
  if (x > 1000) throw std::invalid_argument(
      "wordInteger cannot wordify numbers that are larger than 1000");
  std::string wordy;
  if (x > 100) wordy = wordyOneToNine(x/100) + " hundred";
  if (x > 100 and x % 100 > 0)  wordy += " and ";
  x = x % 100;
  switch (x / 10) {
  case (9):
    wordy += "ninety";
    break;
  case (8):
    wordy += "eighty";
    break;
  case (7):
    wordy += "seventy";
    break;
  case (6):
    wordy += "sixty";
    break;
  case (5):
    wordy += "fifty";
    break;
  case (4):
    wordy += "forty";
    break;
  case (3):
    wordy += "thirty";
    break;
  case (2):
    wordy += "twenty";
    break;
  case (1):
    switch (x % 10) {
    case (9):
      wordy += "nineteen";
      return wordy;
    case (8):
      wordy += "eighteen";
      return wordy;
    case (7):
      wordy += "seventeen";
      return wordy;
    case (6):
      wordy += "sixteen";
      return wordy;
    case (5):
      wordy += "fifteen";
      return wordy;
    case (4):
      wordy += "fourteen";
      return wordy;
    case (3):
      wordy += "thirteen";
      return wordy;
    case (2):
      wordy += "twelve";
      return wordy;
    case (1):
      wordy += "eleven";
      return wordy;
    }
  case (0):
    break;
  }
  if (x % 10 == 0) return wordy;
  if (x > 10 ) wordy += " ";
  wordy += wordyOneToNine( x % 10 );
  return wordy;
}

template<>
std::string trivial() { return "";}
template<>
uint trivial() { return (uint) 0;}
template<>
int trivial() { return 0;}
template<>
double trivial() { return 0.0;}

