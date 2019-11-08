#include "radarpalette.h"

#include <cmath>

#include <QImage>
#include <QGLWidget>

using namespace RLI;

// Описание палитры РЛИ
typedef struct rgbRLI_struct {
  double gamma01_08;                //линейность яркости от 1й до 8й градации РЛИ
  double gamma08_15;                //линейность яркости от 8й до 15й градации РЛИ
  unsigned char Rbg,Gbg,Bbg;        //RGB фона
  unsigned char R01,G01,B01;        //RGB для 1й градации РЛИ
  unsigned char R08,G08,B08;        //RGB для 8й градации РЛИ
  unsigned char R15,G15,B15;        //RGB для 15й градации РЛИ
  unsigned char Rtk,Gtk,Btk;        //RGB следов
  unsigned char padding;
} rgbRLI_struct;


RadarPalette::RadarPalette(QOpenGLContext* context, QObject* parent) : QObject(parent), QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  rgbRLI_Var = 0;
  brightness = 255;

  glGenTextures(1, &_tex_id);

  glBindTexture(GL_TEXTURE_2D, _tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  updatePalette();
}

RadarPalette::~RadarPalette() {
  glDeleteTextures(1, &_tex_id);
}


void RadarPalette::setRgbVar(int val) {
  if (val < 0 || val > 1)
    return;

  rgbRLI_Var = val;
  updatePalette();
}

void RadarPalette::setBrightness(int val) {
  if (val < 0 || val > 255)
    return;

  brightness = val;
  updatePalette();
}

void RadarPalette::updatePalette() {
  int palette[16][3];

  // Палитра РЛИ
  // Первый индекс — день (1) и ночь (0)
  // В текущей реализации ПИКО используются только 0е вторые индексы. Остальные наборы данных прозапас
  static const rgbRLI_struct rgbRLI[2][4] = {
    // g01_08, g08_15, Rbg, Gbg, Bbg, R01, G01, B01, R08, G08, B08, R15, G15, B15, Rtk, Gtk, Btk, padding
    { {     5,     15,   0,   0,   0,  10,  25,  60,  30, 140,  35, 160, 255, 140,   0, 120, 150, 0       },
      {     0,      0,  10,  50,  50,   0,  72,  76,   0, 128, 160,   0, 184, 244, 150,  40,  80, 0       },
      {     0,      0,  60,  30,  10,  76,  24,   0, 160,  52,   0, 244,  80,   0,  20,  60, 170, 0       },
      {     0,      0,  50,  10,  50,  76,  24,  76, 160,  52, 160, 244,  80, 244,  60, 150,  60, 0       } },
    { {    10,     25,   0,   0,   0,  30,  30,  80,  70, 110,  80, 230, 255,  80,   0, 120, 150, 0       },
      {     0,      0,  10,  50,  50,   0,  72,  76,   0, 128, 160,   0, 184, 244, 150,  40,  80, 0       },
      {     0,      0,  60,  30,  10,  76,  24,   0, 160,  52,   0, 244,  80,   0,  20,  60, 170, 0       },
      {     0,      0,  50,  10,  50,  76,  24,  76, 160,  52, 160, 244,  80, 244,  60, 150,  60, 0       } }
    };

  double br = brightness / 255.0; // Вычисление коэффициента яркости

  // Расчёт цветового расстояния между точками на кривой преобразования амплитуды в цвет
  double kR[2][4], kG[2][4], kB[2][4];

  for (int i = 0; i < 4; i++) {
    kR[0][i] = (rgbRLI[rgbRLI_Var][i].R08 - rgbRLI[rgbRLI_Var][i].R01) / 7.0;
    kG[0][i] = (rgbRLI[rgbRLI_Var][i].G08 - rgbRLI[rgbRLI_Var][i].G01) / 7.0;
    kB[0][i] = (rgbRLI[rgbRLI_Var][i].B08 - rgbRLI[rgbRLI_Var][i].B01) / 7.0;

    kR[1][i] = (rgbRLI[rgbRLI_Var][i].R15 - rgbRLI[rgbRLI_Var][i].R08) / 7.0;
    kG[1][i] = (rgbRLI[rgbRLI_Var][i].G15 - rgbRLI[rgbRLI_Var][i].G08) / 7.0;
    kB[1][i] = (rgbRLI[rgbRLI_Var][i].B15 - rgbRLI[rgbRLI_Var][i].B08) / 7.0;
  }

  int n = 0;

  for (int j = 0; j < 16; j++) {
    double R, G, B;

    if (j == 0) {
      // Вычисление цвета фона
      R = br * rgbRLI[rgbRLI_Var][n].Rbg;
      G = br * rgbRLI[rgbRLI_Var][n].Gbg;
      B = br * rgbRLI[rgbRLI_Var][n].Bbg;
    } else if (j < 8) {
      // Вычисление RGBкодов для амплитуд j = 1..7 и наборов цветов n = 0..3
      double fj = (j - 1) ? (pow((j - 1) / 7.0, exp(rgbRLI[rgbRLI_Var][n].gamma01_08 / 32.0)) * 7.0) : 0.0;
      R = br * (rgbRLI[rgbRLI_Var][n].R01 + fj * kR[0][n]);
      G = br * (rgbRLI[rgbRLI_Var][n].G01 + fj * kG[0][n]);
      B = br * (rgbRLI[rgbRLI_Var][n].B01 + fj * kB[0][n]);
    } else {
      // Вычисление RGBкодов для амплитуд j = 8..15 и наборов цветов n = 0..3
      double fj = (j - 8) ? (pow((j - 8) / 7.0, exp(rgbRLI[rgbRLI_Var][n].gamma08_15 / 32.0)) * 7.0) : 0.0;
      R = br * (rgbRLI[rgbRLI_Var][n].R08) + fj * kR[1][n];
      G = br * (rgbRLI[rgbRLI_Var][n].G08) + fj * kG[1][n];
      B = br * (rgbRLI[rgbRLI_Var][n].B08) + fj * kB[1][n];
    }

    //Значения R, G, B сохраняются в таблице для каждой амплитуды (0..15).
    palette[j][0] = int(R);
    palette[j][1] = int(G);
    palette[j][2] = int(B);
  }

  QImage img(16, 1, QImage::Format_RGB888);
  for (int i = 0; i < 16; i++)
    img.setPixel(i, 0, qRgb(palette[i][0], palette[i][1], palette[i][2]));

  QImage imgGL = QGLWidget::convertToGLFormat(img);

  glBindTexture(GL_TEXTURE_2D, _tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits());
  glBindTexture(GL_TEXTURE_2D, 0);
}
