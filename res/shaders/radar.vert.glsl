uniform mat4 mvp_matrix;

attribute float position;
attribute float amplitude;

uniform float peleng_count;
uniform float peleng_size;

varying float v_amp;

void main() {
  float peleng_index = floor(position / peleng_size);
  float radius = mod(position, peleng_size);

  float angle = radians((360.0 * peleng_index) / peleng_count);

  float x =  radius * sin(angle);
  float y = -radius * cos(angle);

  gl_Position = mvp_matrix * vec4(x, y, -amplitude, 1.0);
  if (length(gl_Position.xy) > 1.0)
    gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
  v_amp = amplitude;
}
