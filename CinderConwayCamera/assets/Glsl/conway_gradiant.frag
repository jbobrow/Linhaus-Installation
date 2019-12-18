#version 150

// input texture
uniform sampler2D   uTexture;

// inputs
in vec2 texCoord;
in vec4 globalColor;
out vec4 color;

// conway inputs
uniform vec2 resolution;
uniform float time;
uniform float ageSpeed;
uniform float minThreshold;
uniform float maxThreshold;

// vars
uniform float maxGreen;
uniform float maxRed;

vec4 live = vec4(1.,1.,1.,1.);
vec4 dead = vec4(0.,0.,0.,1.);
vec4 dying = vec4(0.902,0.784,0.,1.);

void main()
{
	vec2 position = ( texCoord.xy / resolution.xy );
	vec2 pixel = 1./resolution;
	
	// normalized for comparison as float (0-1)
	float brightness = texture(uTexture, texCoord).r
	+ texture(uTexture, texCoord).g
	+ texture(uTexture, texCoord).b;
	if( brightness < maxThreshold && brightness > minThreshold ) {//isPersonPresent) {

		float rnd1 = mod(fract(sin(dot(texCoord + time, vec2(14.9898,78.233))) * 43758.5453), 1.0);

		if (rnd1 > 0.5) {
			color = live;
		} else {
			color = dying;
		}
	} else {
		float sum = 0.;
		sum += texture(uTexture, texCoord + pixel * vec2(-1., -1.)).b;
		sum += texture(uTexture, texCoord + pixel * vec2(-1., 0.)).b;
		sum += texture(uTexture, texCoord + pixel * vec2(-1., 1.)).b;
		sum += texture(uTexture, texCoord + pixel * vec2(1., -1.)).b;
		sum += texture(uTexture, texCoord + pixel * vec2(1., 0.)).b;
		sum += texture(uTexture, texCoord + pixel * vec2(1., 1.)).b;
		sum += texture(uTexture, texCoord + pixel * vec2(0., -1.)).b;
		sum += texture(uTexture, texCoord + pixel * vec2(0., 1.)).b;
		vec4 me = texture(uTexture, texCoord);
		
		if (me.b <= 0.1) {  // if I'm dead
			
			if (sum == 3.0) { // am I reborn?
				color = live;
			}
			else if (me.r > 0.784) {  // otherwise, let's animate death
				color = vec4(me.r - ageSpeed, me.g - 4.0 * ageSpeed, 0.0, 1.);
			}
			else if (me.r >= 2.5 * ageSpeed && me.g >= ageSpeed){
				color = vec4(me.r - 2.5 * ageSpeed, me.g - ageSpeed, 0.0, 1.);
			}
			else {
				color = dead;
			}
		}
		else {  // I'm alive!
			
			if (sum >= 2.0 && sum <= 3.0) { // stay alive
				color = live;
			}
			else {
				color = dying;
			}
		}
		//for debugging values calculated
		//color.a = sum/8.0f;
		
	}
}
