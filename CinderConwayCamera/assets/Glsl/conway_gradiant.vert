#version 150

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;
in vec4 ciColor;

out highp vec2 texCoord;
out vec4 globalColor;

void main() {
	texCoord 	= ciTexCoord0.st;
	globalColor = ciColor;
	gl_Position = ciModelViewProjection * ciPosition;
}
