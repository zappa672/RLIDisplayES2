#ifndef TARGETENGINE_H
#define TARGETENGINE_H

#include "../common/state.h"
#include "../common/math.h"

#include "../common/properties.h"
#include "../common/structures.h"

#include "layerbase.h"

#include <QPoint>
#include <QMutex>
#include <QList>
#include <QTimer>
#include <QVector2D>

#include <QOpenGLTexture>

namespace RLI {

  class TargetEngine : public LayerBase {
    Q_OBJECT

  public:
    explicit TargetEngine(QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~TargetEngine() override;

    void paint(const State& state, const Layout& layout) override;

    inline int targetCount() const { return _targets.size(); }
    inline const QString& selectedTag() const { return _selected; }
    inline Target selectedTrgt() const { return _targets.value(_selected); }

  signals:
    void targetCountChanged(int count);
    void selectedTargetUpdated(const QString& tag, const Target& trgt);

  protected slots:
    void timerEvent(QTimerEvent* e) override;

  public slots:
    void onTailsModeChanged(int mode, int minutes);

    void select(const GeoPos& coords, double scale);

    void deleteTarget(QString tag);
    void updateTarget(QString tag, Target target);

  private:
    void bindBuffers();
    void initBuffersTrgts(QString tag);
    int initBuffersTails();

    void initShader();
    QOpenGLTexture* initTexture(QString path);

    QMutex _trgtsMutex;
    QString _selected;
    QMap<QString, Target> _targets;

    QTimer _tailsTimer;
    QMap<QString, QList<QVector2D> > _tails;
    int _tailsTime; // Maximum tails time in minutes

    enum {TRG_TAIL_NUM = 4};

    // Mask shader programs
    QOpenGLTexture* _asset_tex;
    QOpenGLTexture* _selection_tex;

    // -----------------------------------------------
    enum { ATTR_COORDS = 0
         , ATTR_ORDER = 1
         , ATTR_HEADING = 2
         , ATTR_COURSE = 3
         , ATTR_ROTATION = 4
         , ATTR_SPEED = 5
         , ATTR_COUNT = 6 } ;
    enum { UNIF_MVP = 0
         , UNIF_CENTER = 1
         , UNIF_SCALE = 2
         , UNIF_TYPE = 3
         , UNIF_COUNT = 4 } ;

    GLuint _ind_vbo_id;
    GLuint _vbo_ids[ATTR_COUNT];
  };

}

#endif // TARGETENGINE_H

