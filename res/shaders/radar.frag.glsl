uniform sampler2D texture;

uniform float threashold;

varying float v_norm_amp;

void main() {
  if (v_norm_amp >= threashold) {
    gl_FragColor = texture2D(texture, vec2(v_norm_amp, 0.5));
  } else {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);
  }
}
