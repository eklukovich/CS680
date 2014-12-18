attribute vec3 v_position;
attribute vec3 v_color;

varying vec3 color;

void main(void){
	gl_Position = vec4(v_position, 1.0);
	color = v_color;
}
