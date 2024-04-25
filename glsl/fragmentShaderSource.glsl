#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;
void main()
{
    // 使用这个函数来进行赋值
    FragColor = texture(texture1, TexCoord);
}