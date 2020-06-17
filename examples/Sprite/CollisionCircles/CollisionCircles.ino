#if defined(ARDUINO_M5Stick_C)
#include <AXP192.h>
#endif

#include <LovyanGFX.hpp>

struct ball_info_t {
  int32_t x;
  int32_t y;
  int32_t dx;
  int32_t dy;
  int32_t r;
  int32_t m;
  uint32_t color;
};

static constexpr std::uint32_t SHIFTSIZE = 8;
static constexpr std::uint32_t BALL_MAX = 200;

static LGFX lcd;
static LGFX_Sprite _sprites[2];

static ball_info_t _balls[2][BALL_MAX];
static std::uint32_t _ball_count = 0, _fps = 0;
static std::uint32_t ball_count = 0;
static std::uint32_t sec, psec;
static std::uint32_t fps = 0, frame_count = 0;

static std::uint32_t _width = 320;
static std::uint32_t _height = 240;

volatile bool _is_running;
volatile std::uint32_t _draw_count;
volatile std::uint32_t _loop_count;


static void drawfunc(void)
{
  ball_info_t *balls;
  ball_info_t *a;
  LGFX_Sprite *sprite;

  auto width  = _sprites[0].width();
  auto height = _sprites[0].height();

  std::size_t flip = _draw_count & 1;
  balls = &_balls[flip][0];

  sprite = &(_sprites[flip]);
  sprite->fillSprite(TFT_BLACK);

  for (int32_t i = 8; i < width; i += 16) {
    sprite->drawFastVLine(i, 0, height, 0x1F);
  }
  for (int32_t i = 8; i < height; i += 16) {
    sprite->drawFastHLine(0, i, width, 0x1F);
  }
  for (std::uint32_t i = 0; i < _ball_count; i++) {
    a = &balls[i];
    sprite->fillCircle( a->x >> SHIFTSIZE
                      , a->y >> SHIFTSIZE
                      , a->r >> SHIFTSIZE
                      , a->color);
  }

  sprite->setCursor(0,0);
  sprite->printf("obj:%d fps:%d", _ball_count, _fps);

  auto s = (std::uint8_t*)sprite->getBuffer();
  auto p = (std::uint8_t*)_sprites[!flip].getBuffer();
  for (std::int32_t y = 0; y < height; ++y)
  {
    std::int32_t x = 0;
    do
    {
      do { if (s[x] != p[x]) break; } while (++x < width);
      if (x == width) break;

      std::int32_t hitcount = 0;
      std::int32_t xx = x;
      while (++xx < width)
      {
        if (s[xx] != p[xx]) { hitcount = 0; continue; }
        if (hitcount > 6) break; 
        ++hitcount;
      }
      lcd.pushImage(x, y, xx - x - hitcount, 1, &s[x]);
      x = xx+1;
    } while (x < width);
    s += width;
    p += width;
  }
  ++_draw_count;
}

static void mainfunc(void)
{
  float e = 1;

  sec = millis() / 1000;
  if (psec != sec) {
    psec = sec;
    fps = frame_count;
    frame_count = 0;

    if (++ball_count >= BALL_MAX) { ball_count = 1; }
    auto a = &_balls[_loop_count & 1][ball_count - 1];
    a->color = lgfx::color888(100+random(155), 100+random(155), 100+random(155));
    a->x = 0;
    a->y = 0;
    a->dx = random(1, 3 << SHIFTSIZE);
    a->dy = random(1, 3 << SHIFTSIZE);
    a->r = (4 + (ball_count & 0x07)) << SHIFTSIZE;
    a->m =  4 + (ball_count & 0x07);
  }

  frame_count++;
  _loop_count++;

  ball_info_t *a, *b, *balls;
  int32_t rr, len, vx2vy2;
  float vx, vy, distance, t;

  size_t f = _loop_count & 1;
  balls = a = &_balls[f][0];
  b = &_balls[!f][0];
  memcpy(a, b, sizeof(ball_info_t) * ball_count);

  for (int i = 0; i != ball_count; i++) {
    a = &balls[i];
    a->x += a->dx;
    if (a->x < a->r) {
      a->x = a->r;
      if (a->dx < 0) a->dx = - a->dx;
    } else if (a->x >= _width - a->r) {
      a->x = _width - a->r -1;
      if (a->dx > 0) a->dx = - a->dx;
    }
    a->y += a->dy;
    if (a->y < a->r) {
      a->y = a->r;
      if (a->dy < 0) a->dy = - a->dy;
    } else if (a->y >= _height - a->r) {
      a->y = _height - a->r -1;
      if (a->dy > 0) a->dy = - a->dy;
    }
    for (int j = i + 1; j != ball_count; j++) {
      b = &balls[j];

      rr = a->r + b->r;
      vx = a->x - b->x;
      if (abs(vx) > rr) continue;
      vy = a->y - b->y;
      if (abs(vy) > rr) continue;

      len = sqrt(vx * vx + vy * vy);
      if (len >= rr) continue;
      if (len == 0.0) continue;
      distance = (rr - len) >> 1;
      vx *= distance / len;
      vy *= distance / len;

      a->x += vx;
      b->x -= vx;
      vx = b->x - a->x;

      a->y += vy;
      b->y -= vy;
      vy = b->y - a->y;

      vx2vy2 = vx * vx + vy * vy;

      t = -(vx * a->dx + vy * a->dy) / vx2vy2;
      float arx = a->dx + vx * t;
      float ary = a->dy + vy * t;

      t = -(-vy * a->dx + vx * a->dy) / vx2vy2;
      float amx = a->dx - vy * t;
      float amy = a->dy + vx * t;

      t = -(vx * b->dx + vy * b->dy) / vx2vy2;
      float brx = b->dx + vx * t;
      float bry = b->dy + vy * t;

      t = -(-vy * b->dx + vx * b->dy) / vx2vy2;
      float bmx = b->dx - vy * t;
      float bmy = b->dy + vx * t;

      float adx = (a->m * amx + b->m * bmx + bmx * e * b->m - amx * e * b->m) / (a->m + b->m);
      float bdx = - e * (bmx - amx) + adx;
      float ady = (a->m * amy + b->m * bmy + bmy * e * b->m - amy * e * b->m) / (a->m + b->m);
      float bdy = - e * (bmy - amy) + ady;

      a->dx = round(adx + arx);
      a->dy = round(ady + ary);
      b->dx = round(bdx + brx);
      b->dy = round(bdy + bry);
    }
  }

  _fps = fps;
  _ball_count = ball_count;
}

#if defined (ESP32) || (CONFIG_IDF_TARGET_ESP32) || (ESP_PLATFORM)
static void taskDraw(void*)
{
  while ( _is_running )
  {
    while (_loop_count == _draw_count) { taskYIELD(); }
    drawfunc();
  }
  vTaskDelete(NULL);
}
#endif

void setup(void)
{
#if defined(ARDUINO_M5Stick_C)
  AXP192 axp;
  axp.begin();
#endif

  lcd.begin();
  lcd.setBrightness(160);
  lcd.startWrite();

  auto lcd_width = lcd.width();
  auto lcd_height = lcd.height();

  for (std::uint32_t i = 0; i < 2; ++i)
  {
    _sprites[i].setColorDepth(8);
    _sprites[i].createSprite(lcd_width, lcd_height);
    _sprites[i].setTextSize(2);
    _sprites[i].setTextFont(&fonts::Font0);
    _sprites[i].setTextColor(0xFFFFFFU);
  }
  _width = lcd_width << SHIFTSIZE;
  _height = lcd_height << SHIFTSIZE;

  for (std::uint32_t i = 0; i < ball_count; ++i)
  {
    auto a = &_balls[_loop_count & 1][i];
    a->color = lgfx::color888(100+random(155), 100+random(155), 100+random(155));
    a->x = 0;
    a->y = 0;
    a->dx = random(1, 3 << SHIFTSIZE);
    a->dy = random(1, 3 << SHIFTSIZE);
    a->r = (4 + (i & 0x07)) << SHIFTSIZE;
    a->m =  4 + (i & 0x07);
  }

  _is_running = true;
  _draw_count = 0;
  _loop_count = 0;

#if defined (ESP32) || (CONFIG_IDF_TARGET_ESP32) || (ESP_PLATFORM)
  disableCore0WDT();
  xTaskCreate(taskDraw, "taskDraw", 4096, NULL, 0, NULL);
#endif
}

void loop(void)
{
  mainfunc();
#if defined (ESP32) || (CONFIG_IDF_TARGET_ESP32) || (ESP_PLATFORM)
  while (_loop_count != _draw_count) taskYIELD();
#else
  drawfunc();
#endif
}
