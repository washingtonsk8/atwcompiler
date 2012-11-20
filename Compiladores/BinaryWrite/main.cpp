// Program that converts a number from decimal to binary and show the positions where the bit of the number in binary contains 1

#include <iostream>
#include <bitset>
using namespace std;
int main() {
  unsigned int dec, dec2;
  cout << "Write a number in decimal: ";
  cin >> dec;
  cin >> dec2;
  unsigned char _memoryPos [2];
  _memoryPos [0] = static_cast<unsigned char>(dec);
  _memoryPos [1] = static_cast<unsigned char>(dec2);
  cout << _memoryPos[0] << _memoryPos[1] << endl;  
  system("pause");
  return 0;
}