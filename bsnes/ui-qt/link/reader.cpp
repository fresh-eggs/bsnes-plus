Reader reader;

const char* Reader::direct_supported() {
  return "";
}

bool Reader::direct_load(string &filename, uint8_t *&data, unsigned &size) {
  fprintf(stderr, "[*][reader.cpp][direct_load] start\n");
  if(file::exists(filename) == false) return false;

  file fp;
  if(fp.open(filename, file::mode::read) == false) return false;

  data = new uint8_t[size = fp.size()];
  fp.read(data, size);
  fp.close();

  return true;
}

Reader::Reader() {
  //if(open("snesreader")) {
  //  supported = sym("snesreader_supported");
  //  load = sym("snesreader_load");
  //}

  //if(!supported || !load) {
  supported = { &Reader::direct_supported, this };
  load = { &Reader::direct_load, this };
  //}

  compressionList = supported();
  if(compressionList.length() > 0) compressionList = string() << " " << compressionList;

  if(opened()) {
    extensionList = string()
    << " *.smc *.swc *.fig"
    << " *.ufo *.gd3 *.gd7 *.dx2 *.mgd *.mgh"
    << " *.048 *.058 *.068 *.078 *.bin"
    << " *.usa *.eur *.jap *.aus *.bsx";
  }
}
