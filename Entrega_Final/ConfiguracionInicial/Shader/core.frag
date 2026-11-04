#version 330 core
in vec3 ourColor;
out vec4 color;

// En la practica de iluminacion ya se utiliza vec4 y transparencia.
uniform float alpha;

void main()
{
    color = vec4(ourColor, alpha);
}
