#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord; // 添加纹理坐标的输入变量
// 二维输出变量
out vec2 TexCoord;
uniform float yOffset;
uniform float xOffset;
void main()
{
    gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);
    TexCoord = aTexCoord; // 将纹理坐标传递给片段着色器
}