#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX_TDisplayS3 : public lgfx::LGFX_Device {
  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Panel_ST7789  _panel_instance;
  lgfx::Light_PWM     _light_instance;

public:
  LGFX_TDisplayS3(void) {
    // --- 8-bit parallel bus config ---
    {
      auto cfg = _bus_instance.config();
      cfg.port          = 0;
      cfg.freq_write    = 20000000;
      cfg.pin_wr        = 8;
      cfg.pin_rd        = 9;
      cfg.pin_rs        = 7;   // D/C
      cfg.pin_d0        = 39;
      cfg.pin_d1        = 40;
      cfg.pin_d2        = 41;
      cfg.pin_d3        = 42;
      cfg.pin_d4        = 45;
      cfg.pin_d5        = 46;
      cfg.pin_d6        = 47;
      cfg.pin_d7        = 48;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    // --- Panel config ---
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs        = 6;
      cfg.pin_rst       = 5;
      cfg.pin_busy      = -1;
      cfg.panel_width   = 170;
      cfg.panel_height  = 320;
      cfg.offset_x      = 35;
      cfg.offset_y      = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;
      cfg.readable      = false;
      cfg.invert        = true;   // T-Display-S3 requires color inversion
      cfg.rgb_order     = false;
      cfg.dlen_16bit    = false;
      cfg.bus_shared    = false;
      _panel_instance.config(cfg);
    }

    // --- Backlight config ---
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl        = 38;
      cfg.invert        = false;
      cfg.freq          = 12000;
      cfg.pwm_channel   = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};