uniform vec2 u_resolution;
uniform vec2 u_initial_z;
uniform vec2 u_offset;
uniform float u_zoom;
uniform float u_edgeParam;
uniform float u_complexPlaneRuleQ1;
uniform float u_complexPlaneRuleQ2;
uniform int u_iter;

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution;
    vec2 c = (uv - vec2(0.5)) * vec2(3.5, 2.0) / u_zoom + u_offset;

    vec2 z = u_initial_z;
    int iter = 0;

    for (int i = 0; i < u_iter; i++) {

        float x = u_complexPlaneRuleQ2 * (z.x * z.x - z.y * z.y )+ c.x;
        float y = u_complexPlaneRuleQ1 * z.x * z.y + c.y;
        z = vec2(x, y);

        if (dot(z, z) > u_edgeParam)
            break;

        iter++;
    }

    float t = float(iter) / float(u_iter);

    gl_FragColor = vec4(
        cos(1.6 + 4*t * 6.28),
        cos(2.6 + 4*t * 6.28 + 2),
        cos(6.6 + 4*t * 6.28 + 4),
        1.0
    );
}