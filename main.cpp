/*
步骤：
1-初始化： GLFW窗口，GLAD
2-计算球体顶点：通过数学方法计算球体的每个顶点坐标
3-数据处理： 通过球体顶点坐标构造三角形网络，生成并绑定VAO&VBO&EBO（准备再GPU中进行处理)，设置顶点属性指针（本质上就是告诉OpenGL如何处理数据）
4-着色器：给出顶点和片段着色器，然后链接为着色器程序，渲染时使用着色器程序
5-渲染：使用画线模式画圆，开启面剔除，剔除背面，使用线框模式画球,清空缓冲，交换缓冲区检查触发事件
6-结束：释放资源
*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GL/Shader.h"
#include <iostream>
#include <cmath>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



const unsigned int screen_width = 780;
const unsigned int screen_height = 780;
const GLfloat PI = 3.14159265358979323846f;
//将球横纵划分成50*50的网格
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;

int main()
{

    //---------------------------------------初始化（Start）----------------------------
#pragma region
    /*1-初始化*/
    //初始化GLFW
    glfwInit();//初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl版本号3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// 次版本号3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式(无序向后兼容性)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//如果使用的是Mac OS X系统，需加上这行
    glfwWindowHint(GLFW_RESIZABLE, false);//不可改变窗口大小

    //创建窗口（宽、高、窗口名称）
    auto window = glfwCreateWindow(screen_width, screen_height, "Sphere", nullptr, nullptr);
    //检测窗口是否创建成功
    if (window == nullptr)
    {
        std::cout << "Failed to Create OpenGL Context" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);//将窗口的上下文设置为当前进程的主上下文

    //初始化GLAD，加载OpenGL指针地址的函数
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //指定当前视口尺寸（前两个参数为左下角位置，后两个参数是渲染窗口宽、高）
    glViewport(0, 0, screen_width, screen_height);
    std::vector<float> sphereVertices;
    std::vector<int> sphereIndices;\

    // 加载纹理图像
    int width, height, nrChannels;
    unsigned char *data = stbi_load(R"(E:\LEARN\Self-learnMod\Computer GL\LEARN\CLION Project\Global\images\earth.jpg)", &width, &height, &nrChannels, 0);

// 创建纹理对象
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

// 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 上传纹理数据到GPU
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
#pragma endregion
//---------------------------------------初始化（End）----------------------------

//---------------------------------------顶点数据（Start）----------------------------
#pragma region
    /*2-计算球体顶点*/
    //生成球的顶点
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            float u = (float)x / (float)X_SEGMENTS;
            float v = (float)y / (float)Y_SEGMENTS;

            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(u);
            sphereVertices.push_back(v);
        }
    }

    //生成球的Indices
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }


    /*3-数据处理*/
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //生成并绑定球体的VAO和VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //将顶点数据绑定至当前默认的缓冲中
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    GLuint element_buffer_object;//EBO
    glGenBuffers(1, &element_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //纹理坐标设置
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //解绑VAO和VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#pragma endregion
//---------------------------------------顶点数据（End）----------------------------
    const char* vs = R"(E:\LEARN\Self-learnMod\Computer GL\LEARN\CLION Project\Global\glsl\vertexShaderSource.glsl)";
    const char* fs = R"(E:\LEARN\Self-learnMod\Computer GL\LEARN\CLION Project\Global\glsl\fragmentShaderSource.glsl)";
    /*4-着色器*/
    Shader shader(vs, fs);

//---------------------------------------渲染循环（Start）----------------------------
#pragma region
    /*5-渲染*/
    //渲染循环
    while (!glfwWindowShouldClose(window))
    {
        //清空颜色缓冲
        glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        //绘制球
        //开启面剔除(只需要展示一个面，否则会有重合)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindVertexArray(VAO);
        //使用线框模式绘制是GL_FILL
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.setInt("texture1", 0);
        //点阵模式绘制
        //glPointSize(5);
        //glDrawElements(GL_POINTS, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
        //交换缓冲并且检查是否有触发事件(比如键盘输入、鼠标移动)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
//---------------------------------------渲染循环（End）----------------------------

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &element_buffer_object);

    //清理所有的资源并正确退出程序
    glfwTerminate();
    return 0;
}
