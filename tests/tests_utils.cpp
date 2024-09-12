#include <iomanip>
#include "tests_utils.h"


using namespace std;
void printBufferHexa(const unsigned char *buffer, size_t len,
                     const std::string &message)  {
                        #ifndef NDEBUG
  cout << message << endl;
  for (size_t i = 0; i < len; ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(buffer[i]) << " ";

    // Print a new line every 16 bytes for better readability
    if ((i + 1) % 16 == 0) {
      std::cout << std::endl;
    }
  }
  std::cout << std::endl << endl;
  // Reset the stream format back to decimal
  std::cout << std::dec;
  #endif
}