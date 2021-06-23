/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#if defined (ESP32) || defined (CONFIG_IDF_TARGET_ESP32) || defined (CONFIG_IDF_TARGET_ESP32S2) || defined (ESP_PLATFORM)
#elif defined (ESP8266)
#elif defined (__SAMD21__) || defined (__SAMD51__)
#elif defined (STM32F2xx) || defined (STM32F4xx) || defined (STM32F7xx)
#elif defined ( ARDUINO )

#include "common.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  void pinMode(std::int_fast16_t pin, pin_mode_t mode)
  {
    switch (mode)
    {
    case pin_mode_t::output:
      ::pinMode(pin, OUTPUT);
      break;

#if defined (INPUT_PULLUP)
    case pin_mode_t::input_pullup:
      ::pinMode(pin, INPUT_PULLUP);
      break;
#endif
#if defined (INPUT_PULLDOWN)
    case pin_mode_t::input_pulldown:
      ::pinMode(pin, INPUT_PULLDOWN);
      break;
#endif
#if defined (INPUT_PULLDOWN_16)
    case pin_mode_t::input_pulldown:
      ::pinMode(pin, INPUT_PULLDOWN_16);
      break;
#endif

    default:
      ::pinMode(pin, INPUT);
      break;
    }
  }

//----------------------------------------------------------------------------

  /// unimplemented.
  namespace spi
  {
    cpp::result<void, error_t> init(int spi_host, int spi_sclk, int spi_miso, int spi_mosi)  { return cpp::fail(error_t::unknown_err); }
    void release(int spi_host) {}
    void beginTransaction(int spi_host, std::uint32_t freq, int spi_mode) {}
    void endTransaction(int spi_host) {}
    void writeBytes(int spi_host, const std::uint8_t* data, std::size_t length) {}
    void readBytes(int spi_host, std::uint8_t* data, std::size_t length) {}
  }

//----------------------------------------------------------------------------

  /// unimplemented.
  namespace i2c
  {
    cpp::result<void, error_t> init(int i2c_port, int pin_sda, int pin_scl) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> release(int i2c_port) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> restart(int i2c_port, int i2c_addr, std::uint32_t freq, bool read) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> beginTransaction(int i2c_port, int i2c_addr, std::uint32_t freq, bool read) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> endTransaction(int i2c_port) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> writeBytes(int i2c_port, const std::uint8_t *data, std::size_t length) { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> readBytes(int i2c_port, std::uint8_t *data, std::size_t length) { return cpp::fail(error_t::unknown_err); }

//--------

    cpp::result<void, error_t> transactionWrite(int i2c_port, int addr, const std::uint8_t *writedata, std::uint8_t writelen, std::uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> transactionRead(int i2c_port, int addr, std::uint8_t *readdata, std::uint8_t readlen, std::uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> transactionWriteRead(int i2c_port, int addr, const std::uint8_t *writedata, std::uint8_t writelen, std::uint8_t *readdata, std::size_t readlen, std::uint32_t freq)  { return cpp::fail(error_t::unknown_err); }

    cpp::result<std::uint8_t, error_t> registerRead8(int i2c_port, int addr, std::uint8_t reg, std::uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
    cpp::result<void, error_t> registerWrite8(int i2c_port, int addr, std::uint8_t reg, std::uint8_t data, std::uint8_t mask, std::uint32_t freq)  { return cpp::fail(error_t::unknown_err); }
  }

//----------------------------------------------------------------------------
 }
}

#endif
