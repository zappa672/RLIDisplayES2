#include "layerbase.h"

using namespace RLI;

LayerBase::LayerBase(QOpenGLContext* context) : QOpenGLExtraFunctions(context) {
  initializeOpenGLFunctions();
  _program = new QOpenGLShaderProgram();
}

LayerBase::~LayerBase() {  
  delete _program;
}

void LayerBase::initShaderProgram(const QString& glsl_path, const QMap<QString, int>& attr_inds, const QMap<QString, int>& unif_inds) {
  _program->removeAllShaders();

  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, glsl_path + ".vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, glsl_path + ".frag.glsl");
  _program->link();

  glUseProgram(progId());

  for (const QString& attr_name: attr_inds.keys())
    _attr_ids.insert(attr_inds[attr_name], static_cast<GLuint>(_program->attributeLocation(attr_name)));

  for (const QString& unif_name: unif_inds.keys())
    _unif_ids.insert(unif_inds[unif_name], _program->uniformLocation(unif_name));

  glUseProgram(0);
}
