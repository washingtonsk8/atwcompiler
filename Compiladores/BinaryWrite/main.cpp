// Program that converts a number from decimal to binary and show the positions where the bit of the number in binary contains 1

#include <iostream>
#include <bitset>
using namespace std;
int main() {
  unsigned long long dec;
  cout << "Write a number in decimal: ";
  cin >> dec;
  const size_t number_of_bits = 2 * 8;
  bitset<number_of_bits> bs(dec);
  cout << bs << endl;  
  system("pause");
  return 0;
}