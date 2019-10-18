//uniform sampler2D texture;

uniform float threashold;

varying float v_amp;

void main() {
  if (v_amp >= threashold) {
    float norm_amp = min(255.0, max(v_amp, 0.0));
    gl_FragColor = vec4(0.34, 1.00, 0.41, norm_amp / 255.0);
    //gl_FragColor = texture2D(texture, vec2(0.0, norm_amp / 255.0));
  } else {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);
  }
}
