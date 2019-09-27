#ifndef RADARPALETTE_H
#define RADARPALETTE_H

#include <QOpenGLFunctions>
#include <QOpenGLTexture>

namespace RLI {

  // Класс для расчета радарной палитры
  class RadarPalette : public QObject, protected QOpenGLFunctions {
    Q_OBJECT

  public:
    RadarPalette(QOpenGLContext* context, QObject* parent = nullptr);
    ~RadarPalette();

    void setRgbVar(int var);
    void setBrightness(int br);

    inline GLuint texture() { return _tex_id; }

  private:
    // Расчёт зависимости RGBкодов цвета от амплитуды входного сигнала
    void updatePalette();

    // Параметры:
    int rgbRLI_Var;         //Текущая палитра (день/ночь)
    int brightness;      //Яркость РЛИ 0..255

    // Текущая палитра
    GLuint _tex_id;
  };

}


#endif // RADARPALETTE_H
