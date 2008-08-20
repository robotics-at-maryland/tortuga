[vertex]

void main()
{
	gl_Position = gl_Vertex;
}

[pixel]

void main()
{
	gl_FragColor = vec4(0.5, 0.5, 1.0, 1.0);
}
