#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define XBAND_REGS 0xE0

class XBANDBase : public Coprocessor, public Memory  {
public:
  MappedRAM sram;

  void init();
  void load();
  void unload();
  void power();
  void reset();
  void enable();
  static void Enter();
  void enter();

  uint8 memory_access(bool write, Memory &memory, unsigned addr, uint8 data);
  uint8 memory_read(Memory &memory, unsigned addr);
  void memory_write(Memory &memory, unsigned addr, uint8 data);

  uint8 mcu_access(bool write, unsigned addr, uint8 data = 0x00);
  uint8 mcu_read(unsigned addr);
  void mcu_write(unsigned addr, uint8 data);

  uint8 mmio_read(unsigned addr);
  void mmio_write(unsigned addr, uint8 data);

  uint8 read(unsigned addr);
  void write(unsigned addr, uint8 data);

  void serialize(serializer&);

  typedef struct {
    uint8_t line_relay;
    uint8_t regs[0x20];
    uint8_t set_ATV25;
  } xband_modem;

  typedef struct {
    uint16_t cart_space[0x200000];
    uint8_t regs[XBAND_REGS];
    uint8_t kill;
    uint8_t control;
    struct sockaddr_in server;
    int conn;
    uint8_t modem_line_relay;
    uint8_t modem_regs[0x20];
    uint8_t modem_set_ATV25;
    uint8_t net_step;
    uint8_t rxbuf[16384];
    uint32_t rxbufpos;
    uint32_t rxbufused;
    uint8_t txbuf[16384];
    uint32_t txbufpos;
    uint32_t txbufused;
  } xband;

private:
  uint8 mapset;
  
  bool read_96;
};

class XBANDCart : public Memory {
  public:
    void init();
    void load();
    void unload();
    void power();
    void reset();
    void enable();

    void serialize(serializer&);

    uint8 read(unsigned addr);
    void write(unsigned addr, uint8 data);

    uint8 memory_read(Memory &memory, unsigned addr);
    void memory_write(Memory &memory, unsigned addr, uint8 data);

    XBANDCart();
    ~XBANDCart();
  
  private:
    uint8 mapset;
  
    bool read_96;
};

extern XBANDBase  xband_base;
extern XBANDCart  xband_cart;
