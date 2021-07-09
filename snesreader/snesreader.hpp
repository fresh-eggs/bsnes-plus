#include <stdint.h>
namespace nall { class string; }


const char* snesreader_supported();
bool snesreader_load(nall::string &filename, uint8_t *&data, unsigned &size);

