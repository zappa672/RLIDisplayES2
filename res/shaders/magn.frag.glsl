uniform sampler2D texture;
uniform vec4  color;
uniform float threashold;

varying float v_amp;

void main() {
  if (v_amp < threashold)
    gl_FragColor = color;
  else {
    float norm_amp = min(255.0, max(v_amp, 0.0));
    gl_FragColor = texture2D(texture, vec2(0.5, 1.0 - v_amp / 255.0));
  }
}
