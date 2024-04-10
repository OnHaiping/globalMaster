#version 330 core
out vec4 FragColor;
in vec2 TexCoord; // 添加纹理坐标的输入变量
uniform sampler2D texture1;
void main()
{
    FragColor = texture(texture1, TexCoord); // 使用纹理坐标来采样纹理
}