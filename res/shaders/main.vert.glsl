uniform mat4 mvp_matrix;

attribute vec2 position;
attribute vec2 texcoord;

varying vec2 v_texcoord;

void main() {
  gl_Position = mvp_matrix * vec4(position, 0.0, 1.0);
  v_texcoord = texcoord;
}
