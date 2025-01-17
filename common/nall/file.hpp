#ifndef NALL_FILE_HPP
#define NALL_FILE_HPP

#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
  #include <unistd.h>
#else
  #include <io.h>
#endif

#include <nall/stdint.hpp>
#include <nall/string.hpp>
#include <nall/utf8.hpp>
#include <nall/utility.hpp>

namespace nall {
  inline FILE* fopen_utf8(const char *utf8_filename, const char *mode) {
    #if !defined(_WIN32)
    return fopen(utf8_filename, mode);
    #else
    return _wfopen(utf16_t(utf8_filename), utf16_t(mode));
    #endif
  }

  class file {
  public:
    enum class mode : unsigned { read, write, readwrite, writeread };
    enum class index : unsigned { absolute, relative };

    uint8_t read() {
      if(!fp) return 0xff;                       //file not open
      if(file_mode == mode::write) return 0xff;  //reads not permitted
      if(file_offset >= file_size) return 0xff;  //cannot read past end of file
      buffer_sync();
      return buffer[(file_offset++) & buffer_mask];
    }

    uintmax_t readl(unsigned length = 1) {
      uintmax_t data = 0;
      for(int i = 0; i < length; i++) {
        data |= (uintmax_t)read() << (i << 3);
      }
      return data;
    }

    uintmax_t readm(unsigned length = 1) {
      uintmax_t data = 0;
      while(length--) {
        data <<= 8;
        data |= read();
      }
      return data;
    }

    void read(uint8_t *buffer, unsigned length) {
      while(length--) *buffer++ = read();
    }

    void write(uint8_t data) {
      if(!fp) return;                      //file not open
      if(file_mode == mode::read) return;  //writes not permitted
      buffer_sync();
      buffer[(file_offset++) & buffer_mask] = data;
      buffer_dirty = true;
      if(file_offset > file_size) file_size = file_offset;
    }

    void writel(uintmax_t data, unsigned length = 1) {
      while(length--) {
        write(data);
        data >>= 8;
      }
    }

    void writem(uintmax_t data, unsigned length = 1) {
      for(int i = length - 1; i >= 0; i--) {
        write(data >> (i << 3));
      }
    }

    void write(const uint8_t *buffer, unsigned length) {
      while(length--) write(*buffer++);
    }

    template<typename... Args> void print(Args... args) {
      string data(args...);
      const char *p = data;
      while(*p) write(*p++);
    }

    void flush() {
      buffer_flush();
      fflush(fp);
    }

    void seek(int offset, index index_ = index::absolute) {
      if(!fp) return;  //file not open
      buffer_flush();

      unsigned req_offset;
      switch(index_) {
        case index::absolute: req_offset = offset; break;
        case index::relative: req_offset = (offset < 0 && -offset > file_offset) ? 0 : file_offset + offset; break;
      }

      if(req_offset > file_size) {
        if(file_mode == mode::read) {     //cannot seek past end of file
          req_offset = file_size;
        } else {                          //pad file to requested location
          file_offset = file_size;
          while(file_size < req_offset) write(0x00);
        }
      }

      file_offset = req_offset;
    }

    int offset() {
      if(!fp) return -1;  //file not open
      return file_offset;
    }

    int size() {
      if(!fp) return -1;  //file not open
      return file_size;
    }

    bool truncate(unsigned size) {
      if(!fp) return false;  //file not open
      #if !defined(_WIN32)
      return ftruncate(fileno(fp), size) == 0;
      #else
      return _chsize(fileno(fp), size) == 0;
      #endif
    }

    bool end() {
      if(!fp) return true;  //file not open
      return file_offset >= file_size;
    }

    static bool exists(const char *fn) {
      #if !defined(_WIN32)
      FILE *fp = fopen(fn, "rb");
      #else
      FILE *fp = _wfopen(utf16_t(fn), L"rb");
      #endif
      if(fp) {
        fclose(fp);
        return true;
      }
      return false;
    }

    static unsigned size(const char *fn) {
      #if !defined(_WIN32)
      FILE *fp = fopen(fn, "rb");
      #else
      FILE *fp = _wfopen(utf16_t(fn), L"rb");
      #endif
      unsigned filesize = 0;
      if(fp) {
        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        fclose(fp);
      }
      return filesize;
    }

    bool open() {
      return fp;
    }

    bool open(const char *fn, mode mode_) {
      if(fp) return false;

      switch(file_mode = mode_) {
        #if !defined(_WIN32)
        case mode::read:      fp = fopen(fn, "rb");  break;
        case mode::write:     fp = fopen(fn, "wb+"); break;  //need read permission for buffering
        case mode::readwrite: fp = fopen(fn, "rb+"); break;
        case mode::writeread: fp = fopen(fn, "wb+"); break;
        #else
        case mode::read:      fp = _wfopen(utf16_t(fn), L"rb");  break;
        case mode::write:     fp = _wfopen(utf16_t(fn), L"wb+"); break;
        case mode::readwrite: fp = _wfopen(utf16_t(fn), L"rb+"); break;
        case mode::writeread: fp = _wfopen(utf16_t(fn), L"wb+"); break;
        #endif
      }
      if(!fp) return false;
      buffer_offset = -1;  //invalidate buffer
      file_offset = 0;
      fseek(fp, 0, SEEK_END);
      file_size = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      return true;
    }

    void close() {
      if(!fp) return;
      buffer_flush();
      fclose(fp);
      fp = 0;
    }

    file() {
      memset(buffer, 0, sizeof buffer);
      buffer_offset = -1;
      buffer_dirty = false;
      fp = 0;
      file_offset = 0;
      file_size = 0;
      file_mode = mode::read;
    }

    ~file() {
      close();
    }

    file& operator=(const file&) = delete;
    file(const file&) = delete;

  private:
    enum { buffer_size = 1 << 12, buffer_mask = buffer_size - 1 };
    char buffer[buffer_size];
    int buffer_offset;
    bool buffer_dirty;
    FILE *fp;
    unsigned file_offset;
    unsigned file_size;
    mode file_mode;

    void buffer_sync() {
      if(!fp) return;  //file not open
      if(buffer_offset != (file_offset & ~buffer_mask)) {
        buffer_flush();
        buffer_offset = file_offset & ~buffer_mask;
        //fprintf(stderr, "[*][file.hpp][buffer_sync()] buffer_offset: 0x%x| file_offset: 0x%x| buffer_mask: 0x%x\n",buffer_offset, file_offset, buffer_mask);
        fseek(fp, buffer_offset, SEEK_SET);
        unsigned length = (buffer_offset + buffer_size) <= file_size ? buffer_size : (file_size & buffer_mask);
        if(length) unsigned unused = fread(buffer, 1, length, fp);
      }
    }

    void buffer_flush() {
      if(!fp) return;                      //file not open
      if(file_mode == mode::read) return;  //buffer cannot be written to
      if(buffer_offset < 0) return;        //buffer unused
      if(buffer_dirty == false) return;    //buffer unmodified since read
      fseek(fp, buffer_offset, SEEK_SET);
      unsigned length = (buffer_offset + buffer_size) <= file_size ? buffer_size : (file_size & buffer_mask);
      if(length) unsigned unused = fwrite(buffer, 1, length, fp);
      buffer_offset = -1;                  //invalidate buffer
      buffer_dirty = false;
    }
  };
}

#endif
