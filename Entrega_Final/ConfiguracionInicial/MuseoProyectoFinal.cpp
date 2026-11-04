#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();

const GLuint WIDTH = 1000, HEIGHT = 700;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Se colocó la cámara frente a la escalera.
Camera camera(glm::vec3(8.0f, 8.0f, 27.0f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024] = { false };
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool puertasAbiertas = false;
float anguloPuertas = 0.0f;

bool puertasNuevasAbiertas = false;
float anguloPuertasNuevas = 0.0f;

// Se definieron los estados del robot guía.
int estadoRobot = 0;
float avanceRobot = 0.0f;
float pasoRobot = 0.0f;
bool pasoSubiendoRobot = true;
float brazoSaludoRobot = 0.0f;
bool saludoSubiendoRobot = true;
int saludosRobot = 0;
float cabezaRobot = 0.0f;

// EXTERIOR: movimiento del coche y animacion sencilla de la fuente.
// C pausa/reanuda el coche; F activa/desactiva el chorro.
float cochePosX = -28.0f;
bool cocheHaciaDerecha = true;
bool cocheEnMovimiento = true;
float chorroFuente = 0.55f;
bool chorroSubiendo = true;
bool fuenteEncendida = true;


// Se agruparon las piezas fijas del robot.
GLfloat piezasFijasRobot[][9] = {
    {0.00f,1.42f,0.00f, 0.90f,0.98f,0.53f, 0.21f,0.65f,0.78f},
    {0.00f,1.45f,0.29f, 0.68f,0.58f,0.08f, 0.09f,0.22f,0.39f},
    {0.00f,1.47f,0.35f, 0.24f,0.20f,0.04f, 0.93f,0.76f,0.29f},
    {0.00f,1.82f,0.30f, 0.67f,0.06f,0.06f, 0.70f,0.88f,0.93f},
    {0.00f,0.87f,0.00f, 0.82f,0.20f,0.43f, 0.13f,0.34f,0.54f},
    {0.00f,1.99f,0.00f, 0.23f,0.19f,0.23f, 0.23f,0.35f,0.48f},
    {-0.61f,1.75f,0.00f, 0.26f,0.25f,0.32f, 0.16f,0.43f,0.56f},
    { 0.61f,1.75f,0.00f, 0.26f,0.25f,0.32f, 0.16f,0.43f,0.56f}
};

// Se distribuyeron cuatro luces por el museo.
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f, 3.0f, 0.0f),
    glm::vec3(16.0f, 3.0f, 0.0f),
    glm::vec3(8.0f, 3.0f, -20.0f),
    glm::vec3(8.0f, 7.4f, -20.0f)
};

// Se definieron los vértices y normales del cubo.
GLfloat vertices[] = {
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
     0.5f,-0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
     0.5f, 0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
     0.5f, 0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
    -0.5f, 0.5f,-0.5f, 0.0f, 0.0f,-1.0f,
    -0.5f,-0.5f,-0.5f, 0.0f, 0.0f,-1.0f,

    -0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, 0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f,-0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f, 0.5f,-1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,-1.0f, 0.0f, 0.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f,-0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,-0.5f,-0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f,-0.5f,-0.5f, 0.0f,-1.0f, 0.0f,
     0.5f,-0.5f,-0.5f, 0.0f,-1.0f, 0.0f,
     0.5f,-0.5f, 0.5f, 0.0f,-1.0f, 0.0f,
     0.5f,-0.5f, 0.5f, 0.0f,-1.0f, 0.0f,
    -0.5f,-0.5f, 0.5f, 0.0f,-1.0f, 0.0f,
    -0.5f,-0.5f,-0.5f, 0.0f,-1.0f, 0.0f,

    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f,-0.5f, 0.0f, 1.0f, 0.0f
};

// Se diseñaron siete tipos de objetos con cubos.
GLfloat piezas[][10] = {
    // Se armó el basurero.
    {1, 0.00f,0.45f,0.00f, 0.65f,0.90f,0.60f, 0.13f,0.43f,0.60f},
    {1, 0.00f,0.94f,0.00f, 0.72f,0.09f,0.68f, 0.08f,0.14f,0.20f},
    {1, 0.00f,1.00f,0.00f, 0.40f,0.02f,0.25f, 0.02f,0.02f,0.03f},

    // Se armó la banca.
    {2, 0.00f,0.54f,0.00f, 1.80f,0.17f,0.60f, 0.46f,0.26f,0.12f},
    {2, 0.00f,1.04f,-0.25f,1.80f,0.86f,0.13f, 0.46f,0.26f,0.12f},
    {2,-0.71f,0.25f,-0.19f,0.13f,0.50f,0.13f, 0.22f,0.22f,0.24f},
    {2, 0.71f,0.25f,-0.19f,0.13f,0.50f,0.13f, 0.22f,0.22f,0.24f},
    {2,-0.71f,0.25f, 0.19f,0.13f,0.50f,0.13f, 0.22f,0.22f,0.24f},
    {2, 0.71f,0.25f, 0.19f,0.13f,0.50f,0.13f, 0.22f,0.22f,0.24f},

    // Se armó la mesa.
    {3, 0.00f,0.82f,0.00f, 1.45f,0.15f,1.05f, 0.63f,0.41f,0.22f},
    {3,-0.56f,0.39f,-0.38f,0.13f,0.78f,0.13f, 0.32f,0.20f,0.12f},
    {3, 0.56f,0.39f,-0.38f,0.13f,0.78f,0.13f, 0.32f,0.20f,0.12f},
    {3,-0.56f,0.39f, 0.38f,0.13f,0.78f,0.13f, 0.32f,0.20f,0.12f},
    {3, 0.56f,0.39f, 0.38f,0.13f,0.78f,0.13f, 0.32f,0.20f,0.12f},

    // Se armó el pedestal.
    {4, 0.00f,0.08f,0.00f, 0.90f,0.16f,0.90f, 0.76f,0.75f,0.71f},
    {4, 0.00f,0.68f,0.00f, 0.70f,1.05f,0.70f, 0.86f,0.84f,0.79f},
    {4, 0.00f,1.26f,0.00f, 0.95f,0.13f,0.95f, 0.65f,0.64f,0.61f},

    // Se armó la estantería.
    {5,-0.67f,1.08f,0.00f, 0.13f,2.16f,0.45f, 0.38f,0.23f,0.12f},
    {5, 0.67f,1.08f,0.00f, 0.13f,2.16f,0.45f, 0.38f,0.23f,0.12f},
    {5, 0.00f,0.08f,0.00f, 1.46f,0.15f,0.48f, 0.52f,0.33f,0.16f},
    {5, 0.00f,0.77f,0.00f, 1.46f,0.11f,0.48f, 0.52f,0.33f,0.16f},
    {5, 0.00f,1.47f,0.00f, 1.46f,0.11f,0.48f, 0.52f,0.33f,0.16f},
    {5, 0.00f,2.10f,0.00f, 1.46f,0.15f,0.48f, 0.52f,0.33f,0.16f},
    {5, 0.00f,1.09f,-0.21f,1.35f,2.03f,0.06f, 0.29f,0.19f,0.10f},

    // Se armó el cuadro.
    {6, 0.00f,1.91f,0.00f, 1.46f,1.08f,0.07f, 0.08f,0.14f,0.29f},
    {6,-0.77f,1.91f,0.03f,0.12f,1.18f,0.14f, 0.64f,0.40f,0.17f},
    {6, 0.77f,1.91f,0.03f,0.12f,1.18f,0.14f, 0.64f,0.40f,0.17f},
    {6, 0.00f,2.49f,0.03f,1.64f,0.12f,0.14f, 0.64f,0.40f,0.17f},
    {6, 0.00f,1.33f,0.03f,1.64f,0.12f,0.14f, 0.64f,0.40f,0.17f},
    {6, 0.00f,1.90f,0.05f,0.80f,0.43f,0.05f, 0.15f,0.49f,0.62f},

    // Se armó el letrero.
    {7, 0.00f,0.08f,0.00f, 0.75f,0.16f,0.60f, 0.20f,0.22f,0.27f},
    {7, 0.00f,0.91f,0.00f, 0.14f,1.66f,0.14f, 0.20f,0.22f,0.27f},
    {7, 0.00f,1.77f,0.00f, 1.30f,0.84f,0.15f, 0.14f,0.34f,0.54f},
    {7, 0.00f,2.05f,0.09f, 0.95f,0.09f,0.04f, 0.90f,0.84f,0.42f},
    {7, 0.00f,1.77f,0.09f, 0.88f,0.08f,0.04f, 0.90f,0.90f,0.90f}
};

// Se registraron las posiciones de los objetos.
GLfloat ubicaciones[][5] = {
    // Se distribuyeron los objetos en las salas laterales.
    {1, -3.6f, 0.0f,   2.6f},
    {1, 19.0f, 0.0f,   2.7f},
    {2, -1.0f, 0.0f,  -2.6f},
    {2, 13.2f, 0.0f,  -2.5f},
    {3,  1.8f, 0.0f,   2.0f},
    {3, 17.3f, 0.0f,   2.0f},
    {4,  2.8f, 0.0f,  -2.1f},
    {4, 18.0f, 0.0f,  -1.9f},
    {4,  0.0f, 0.0f,   2.0f},
    {4, 14.0f, 0.0f,   2.0f},
    {5, -4.0f, 0.0f,  -3.3f},
    {5, 20.0f, 0.0f,  -3.3f},
    {6, -2.0f, 0.0f,  -4.2f},
    {6, 20.80f,0.0f, -2.50f,-90.0f},
    {6, -2.0f, 0.0f,   4.15f, 180.0f},
    {6, 20.80f,0.0f,   2.20f, -90.0f},

    // Se colocaron exhibiciones en la sala posterior.
    {1,  9.0f, 0.0f, -21.7f},
    {4, 13.0f, 0.0f, -20.0f},
    {4,  0.0f, 0.0f, -21.0f},
    {4,  5.8f, 0.0f, -22.4f},
    {4, 18.0f, 0.0f, -21.9f},
    {3,  7.5f, 0.0f, -16.3f},
    {6, -1.3f, 0.0f, -23.65f},
    {6,  8.0f, 0.0f, -23.65f},
    {6, 18.5f, 0.0f, -23.65f},
    {6, -4.80f,0.0f, -22.35f,90.0f},
    {6, 20.80f,0.0f, -18.2f,-90.0f},

    // Se añadieron objetos en el exterior y la azotea.
    {7, 11.7f, 0.0f,   9.8f},
    {2,  9.0f, 8.4f, -22.3f},
    {7,  7.0f, 8.4f, -22.4f},

    // Se organizaron los objetos de la planta superior.
    {2,  3.80f,4.2f, -22.75f, -55.0f},
    {2, -4.00f,4.2f, -16.95f, 125.0f},
    {4, -3.90f,4.2f, -22.45f},
    {4,  4.10f,4.2f, -16.15f},
    {7,  1.15f,4.2f, -16.00f, 180.0f}, // Cartel mirando hacia la piramide
    {5, 17.20f,4.2f, -22.65f}, // Estanteria separada de la pared derecha
    {6, -1.20f,4.2f, -23.65f},
    {6,  7.20f,4.2f, -15.10f,180.0f},
    {6, -4.80f,4.2f, -20.00f, 90.0f},
    {6, 20.80f,4.2f, -20.50f,-90.0f}
};

// ------------------------------------------------------------
// CUBOS EXTERIORES: se reutilizan las operaciones y shaders
// de las practicas. Todos los objetos usan el mismo cubo.
// ------------------------------------------------------------
void CuboExteriorTextura(Shader& shader, GLuint vao, GLint uniformModelo,
    GLuint textura, glm::mat4 base, glm::vec3 posicion, glm::vec3 tamano)
{
    glm::mat4 modelo = glm::translate(base, posicion);
    modelo = glm::scale(modelo, tamano);
    shader.Use();
    glUniformMatrix4fv(uniformModelo, 1, GL_FALSE, glm::value_ptr(modelo));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textura);
    glUniform1i(glGetUniformLocation(shader.Program, "texture_diffuse1"), 0);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CuboExteriorColor(Shader& shader, GLuint vao, GLint uniformModelo,
    GLint uniformColor, glm::mat4 base, glm::vec3 posicion,
    glm::vec3 tamano, glm::vec3 color)
{
    glm::mat4 modelo = glm::translate(base, posicion);
    modelo = glm::scale(modelo, tamano);
    shader.Use();
    glUniformMatrix4fv(uniformModelo, 1, GL_FALSE, glm::value_ptr(modelo));
    glUniform3f(uniformColor, color.r, color.g, color.b);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main()
{
    if (!glfwInit()) return EXIT_FAILURE;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Museo - ProyectoFinal", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "No se pudo crear la ventana" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "No se pudo iniciar GLEW" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Se cargaron los shaders y modelos.
    Shader coreShader("Shader/core.vs", "Shader/core.frag");
    Shader museoShader("Shader/modelLoading.vs", "Shader/modelLoading.frag");
    Model Museo((char*)"Models/Museo.obj");
    Model Fondo((char*)"Models/Fondo.obj");

    Model Bandera((char*)"Models/Bandera.obj");
    Model Chile((char*)"Models/chile.obj");
    Model Molcajete((char*)"Models/molcajete.obj");
    Model Nopal((char*)"Models/Nopal.obj");
    Model Piramide((char*)"Models/Piramide.obj");
    Model Shot((char*)"Models/shot.obj");
    Model Sombrero((char*)"Models/Sombrero.obj");
    Model Taco((char*)"Models/Taco.obj");

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // ================================================================
    // CUBO CON TEXTURA: mismas 36 posiciones/normales, ahora con UV.
    // Se usa un VAO diferente para NO alterar el robot ni las puertas.
    // Atributos: 0=posicion, 1=normal, 2=coordenada de textura.
    // ================================================================
    GLfloat uvPorCara[] = {
        0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 0.0f
    };
    GLfloat verticesConUV[36 * 8];
    for (int v = 0; v < 36; v++) {
        for (int dato = 0; dato < 6; dato++) {
            verticesConUV[v * 8 + dato] = vertices[v * 6 + dato];
        }
        verticesConUV[v * 8 + 6] = uvPorCara[(v % 6) * 2];
        verticesConUV[v * 8 + 7] = uvPorCara[(v % 6) * 2 + 1];
    }

    GLuint VAOTextura, VBOTextura;
    glGenVertexArrays(1, &VAOTextura);
    glGenBuffers(1, &VBOTextura);
    glBindVertexArray(VAOTextura);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTextura);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesConUV), verticesConUV, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
        (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
        (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Se reutiliza TextureFromFile() de tu propio Model.h (SOIL2).
    // Todos estos archivos YA existen en Models/Texturas, segun tu ZIP.
    GLuint texMadera = TextureFromFile("Texturas/madera.jpg", "Models");
    GLuint texMetal = TextureFromFile("Texturas/metal.jpg", "Models");
    GLuint texMarmol = TextureFromFile("Texturas/marmol.jpg", "Models");
    GLuint texOro = TextureFromFile("Texturas/oro.jpg", "Models");
    GLuint texYeso = TextureFromFile("Texturas/yeso_claro.jpg", "Models");
    GLuint texCuadro = TextureFromFile("Texturas/atardecer.jpg", "Models");

    // Nuevas texturas incluidas en el ZIP (se copian a Models/Texturas).
    // Puedes reemplazar asfalto.jpg por la imagen que descargues.
    GLuint texAsfalto = TextureFromFile("Texturas/asfalto.jpg", "Models");
    GLuint texAguaFuente = TextureFromFile("Texturas/agua_fuente.jpg", "Models");
    GLuint texTierra = TextureFromFile("Texturas/tierra_jardinera.jpg", "Models");
    GLuint texTechoKiosco = TextureFromFile("Texturas/techo_kiosco.jpg", "Models");
    GLuint texPinturaCoche = TextureFromFile("Texturas/pintura_coche.jpg", "Models");
    GLuint texCristalCoche = TextureFromFile("Texturas/cristal_coche.jpg", "Models");

    // Texturas que ya estan en tu carpeta original.
    GLuint texPiedraExterior = TextureFromFile("Texturas/piedra.jpg", "Models");
    GLuint texBarroExterior = TextureFromFile("Texturas/barro.jpg", "Models");
    GLuint texVerdeExterior = TextureFromFile("Texturas/verde.jpg", "Models");


    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        DoMovement();
        Animation();

        glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
        if (SCREEN_WIDTH == 0 || SCREEN_HEIGHT == 0) continue;
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        glClearColor(0.88f, 0.58f, 0.55f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.GetZoom(),
            (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 500.0f);
        glm::mat4 model(1.0f);

        museoShader.Use();
        GLint modelMuseo = glGetUniformLocation(museoShader.Program, "model");
        GLint viewMuseo = glGetUniformLocation(museoShader.Program, "view");
        GLint projMuseo = glGetUniformLocation(museoShader.Program, "projection");
        glUniformMatrix4fv(viewMuseo, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projMuseo, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        // Se configuraron las luces de las salas.
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[0].position"),
            pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[0].ambient"), 0.08f, 0.07f, 0.05f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[0].diffuse"), 0.55f, 0.49f, 0.38f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[0].specular"), 0.20f, 0.20f, 0.20f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[0].linear"), 0.07f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[0].quadratic"), 0.025f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[1].position"),
            pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[1].ambient"), 0.08f, 0.07f, 0.05f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[1].diffuse"), 0.55f, 0.49f, 0.38f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[1].specular"), 0.20f, 0.20f, 0.20f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[1].linear"), 0.07f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[1].quadratic"), 0.025f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[2].position"),
            pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[2].ambient"), 0.08f, 0.07f, 0.05f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[2].diffuse"), 0.55f, 0.49f, 0.38f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[2].specular"), 0.20f, 0.20f, 0.20f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[2].linear"), 0.07f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[2].quadratic"), 0.025f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[3].position"),
            pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[3].ambient"), 0.08f, 0.07f, 0.05f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[3].diffuse"), 0.55f, 0.49f, 0.38f);
        glUniform3f(glGetUniformLocation(museoShader.Program, "pointLights[3].specular"), 0.20f, 0.20f, 0.20f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[3].linear"), 0.07f);
        glUniform1f(glGetUniformLocation(museoShader.Program, "pointLights[3].quadratic"), 0.025f);
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Fondo.Draw(museoShader);

        Museo.Draw(museoShader);

        // Se colocaron los modelos de exhibición.

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(16.0f, 15.5f, -19.4f));
        model = glm::scale(model, glm::vec3(0.80f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Bandera.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.8f, 1.33f, -2.1f));
        model = glm::scale(model, glm::vec3(0.24f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Chile.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(18.0f, 1.50f, -1.9f));
        model = glm::scale(model, glm::vec3(0.42f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Molcajete.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.55f, -21.0f));
        model = glm::scale(model, glm::vec3(0.22f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Nopal.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.70f, 4.34f, -19.45f));
        model = glm::scale(model, glm::vec3(0.095f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Piramide.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.33f, 2.0f));
        model = glm::scale(model, glm::vec3(0.17f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Shot.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(14.0f, 1.345f, 2.0f));
        model = glm::scale(model, glm::vec3(0.18f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Sombrero.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(13.0f, 1.33f, -20.0f));
        model = glm::scale(model, glm::vec3(0.12f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Taco.Draw(museoShader);

        // Se reutilizaron modelos en la planta baja.
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.8f, 1.33f, -22.4f));
        model = glm::scale(model, glm::vec3(0.17f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Shot.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(18.0f, 1.345f, -21.9f));
        model = glm::scale(model, glm::vec3(0.14f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Sombrero.Draw(museoShader);

        // Se añadieron exhibiciones a la planta superior.
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.9f, 5.71f, -22.45f));
        model = glm::scale(model, glm::vec3(0.18f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Nopal.Draw(museoShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.10f, 5.53f, -16.15f));
        model = glm::scale(model, glm::vec3(0.16f));
        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
        Chile.Draw(museoShader);

        coreShader.Use();
        GLint modelLoc = glGetUniformLocation(coreShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(coreShader.Program, "view");
        GLint projLoc = glGetUniformLocation(coreShader.Program, "projection");
        GLint colorLoc = glGetUniformLocation(coreShader.Program, "color");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(coreShader.Program, "alpha"), 1.0f);

        glBindVertexArray(VAO);

        // Se construyó la base de la pirámide.
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.70f, 4.26f, -19.45f));
        model = glm::scale(model, glm::vec3(3.85f, 0.12f, 3.85f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.66f, 0.61f, 0.52f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ================================================================
        // SIETE MODELOS TEXTURIZADOS: bote, banca, mesa, pedestal,
        // estanteria, cuadro y letrero. Se conserva la distribucion.
        // ================================================================
        for (GLuint i = 0; i < sizeof(ubicaciones) / sizeof(ubicaciones[0]); i++) {
            for (GLuint j = 0; j < sizeof(piezas) / sizeof(piezas[0]); j++) {
                if (ubicaciones[i][0] == piezas[j][0]) {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(
                        ubicaciones[i][1], ubicaciones[i][2], ubicaciones[i][3]));
                    model = glm::rotate(model, glm::radians(ubicaciones[i][4]),
                        glm::vec3(0.0f, 1.0f, 0.0f));
                    model = glm::translate(model, glm::vec3(
                        piezas[j][1], piezas[j][2], piezas[j][3]));
                    model = glm::scale(model, glm::vec3(
                        piezas[j][4], piezas[j][5], piezas[j][6]));

                    // Se elige una textura para cada parte del mueble.
                    // Los indices corresponden a la matriz piezas[] original.
                    int tipo = (int)piezas[j][0];
                    GLuint texturaSeleccionada = 0;
                    if (tipo == 1) { // Bote: metal; ranura oscura sin textura.
                        if (j != 2) texturaSeleccionada = texMetal;
                    }
                    else if (tipo == 2) { // Banca: asiento/respaldo de madera.
                        texturaSeleccionada = (j <= 4) ? texMadera : texMetal;
                    }
                    else if (tipo == 3) { // Mesa: madera en tablero y patas.
                        texturaSeleccionada = texMadera;
                    }
                    else if (tipo == 4) { // Pedestal: marmol en las 3 piezas.
                        texturaSeleccionada = texMarmol;
                    }
                    else if (tipo == 5) { // Estanteria: madera.
                        texturaSeleccionada = texMadera;
                    }
                    else if (tipo == 6) { // Cuadro: marco y fotografia.
                        texturaSeleccionada = (j == 29) ? texCuadro : texMadera;
                    }
                    else if (tipo == 7) { // Letrero: poste metalico y panel.
                        if (j <= 31) texturaSeleccionada = texMetal;
                        else if (j == 32) texturaSeleccionada = texMadera;
                        else if (j == 33) texturaSeleccionada = texOro;
                        else texturaSeleccionada = texYeso;
                    }

                    if (texturaSeleccionada != 0) {
                        // El shader modelLoading ya acepta UV y texture_diffuse1.
                        museoShader.Use();
                        glUniformMatrix4fv(modelMuseo, 1, GL_FALSE, glm::value_ptr(model));
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, texturaSeleccionada);
                        glUniform1i(glGetUniformLocation(
                            museoShader.Program, "texture_diffuse1"), 0);
                        glBindVertexArray(VAOTextura);
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        // Restaurar el shader y VAO de color para robot/puertas.
                        coreShader.Use();
                        glBindVertexArray(VAO);
                    }
                    else {
                        // Conservar la ranura negra del bote sin imagen.
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                        for (GLuint cara = 0; cara < 6; cara++) {
                            GLfloat brillo = 0.85f;
                            if (cara == 0) brillo = 0.65f;
                            if (cara == 2) brillo = 0.73f;
                            if (cara == 4) brillo = 0.55f;
                            if (cara == 5) brillo = 1.00f;
                            glUniform3f(colorLoc, piezas[j][7] * brillo,
                                piezas[j][8] * brillo, piezas[j][9] * brillo);
                            glDrawArrays(GL_TRIANGLES, cara * 6, 6);
                        }
                    }
                }
            }
        }

        // ==============================================================
        // PLAZA EXTERIOR - FUENTE, KIOSCO, JARDINERAS, CALLE Y COCHE.
        // Solo se utilizan cubos, texturas, translate, rotate y scale.
        // La calle esta frente a las escaleras, sin tapar la entrada.
        // ==============================================================
        glm::mat4 identidad(1.0f);

        // CALLE frente al museo. Se repite en tramos para no estirar
        for (int tramo = 0; tramo < 16; tramo++) {
            float xTramo = -67.5f + 9.0f * tramo;
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texAsfalto, identidad, glm::vec3(xTramo, -0.005f, 36.0f),
                glm::vec3(9.02f, 0.14f, 16.0f));
        }

        // Banqueta del lado del museo y guarniciones de la calle.
        for (int tramoBanqueta = 0; tramoBanqueta < 22; tramoBanqueta++) {
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texPiedraExterior, identidad,
                glm::vec3(-44.5f + 5.0f * tramoBanqueta, 0.06f, 25.4f),
                glm::vec3(5.02f, 0.17f, 4.4f));
        }
        for (int borde = 0; borde < 2; borde++) {
            CuboExteriorColor(coreShader, VAO, modelLoc, colorLoc, identidad,
                glm::vec3(0.0f, 0.11f, borde == 0 ? 28.0f : 44.0f),
                glm::vec3(144.0f, 0.15f, 0.24f),
                glm::vec3(0.74f, 0.73f, 0.69f));
        }

        // Linea central discontinua, junto con lineas laterales.
        for (int raya = 0; raya < 16; raya++) {
            CuboExteriorColor(coreShader, VAO, modelLoc, colorLoc, identidad,
                glm::vec3(-66.0f + 9.0f * raya, 0.078f, 36.0f),
                glm::vec3(4.2f, 0.014f, 0.16f),
                glm::vec3(0.97f, 0.85f, 0.34f));
        }
        for (int linea = 0; linea < 2; linea++) {
            CuboExteriorColor(coreShader, VAO, modelLoc, colorLoc, identidad,
                glm::vec3(0.0f, 0.078f, linea == 0 ? 29.2f : 42.8f),
                glm::vec3(142.0f, 0.014f, 0.10f),
                glm::vec3(0.90f, 0.91f, 0.89f));
        }

        // FUENTE ORNAMENTAL. A la izquierda de las escaleras.
        glm::mat4 fuente = glm::translate(identidad,
            glm::vec3(-17.0f, 0.0f, 15.0f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texPiedraExterior, fuente, glm::vec3(0.0f, 0.11f, 0.0f),
            glm::vec3(5.7f, 0.22f, 5.7f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texAguaFuente, fuente, glm::vec3(0.0f, 0.245f, 0.0f),
            glm::vec3(4.6f, 0.055f, 4.6f));
        for (int lado = 0; lado < 4; lado++) {
            float x = (lado == 0 ? -2.55f : (lado == 1 ? 2.55f : 0.0f));
            float z = (lado == 2 ? -2.55f : (lado == 3 ? 2.55f : 0.0f));
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texMarmol, fuente, glm::vec3(x, 0.43f, z),
                glm::vec3(lado < 2 ? 0.45f : 5.5f, 0.62f,
                    lado < 2 ? 5.5f : 0.45f));
        }
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texMarmol, fuente, glm::vec3(0.0f, 0.95f, 0.0f),
            glm::vec3(0.85f, 1.5f, 0.85f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texMarmol, fuente, glm::vec3(0.0f, 1.72f, 0.0f),
            glm::vec3(2.3f, 0.24f, 2.3f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texAguaFuente, fuente, glm::vec3(0.0f, 1.87f, 0.0f),
            glm::vec3(1.85f, 0.06f, 1.85f));
        if (fuenteEncendida) {
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texAguaFuente, fuente,
                glm::vec3(0.0f, 1.92f + chorroFuente * 0.5f, 0.0f),
                glm::vec3(0.22f, chorroFuente, 0.22f));
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texAguaFuente, fuente,
                glm::vec3(0.0f, 1.94f + chorroFuente, 0.0f),
                glm::vec3(0.42f, 0.12f, 0.42f));
        }

        // KIOSCO: una pequena taquilla/punto de informacion exterior.
        glm::mat4 kiosco = glm::translate(identidad,
            glm::vec3(31.5f, 0.0f, 14.0f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texPiedraExterior, kiosco, glm::vec3(0.0f, 0.10f, 0.0f),
            glm::vec3(4.4f, 0.20f, 3.4f));
        for (int px = 0; px < 2; px++) {
            for (int pz = 0; pz < 2; pz++) {
                CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                    texMadera, kiosco,
                    glm::vec3(px == 0 ? -1.65f : 1.65f, 1.58f,
                        pz == 0 ? -1.20f : 1.20f),
                    glm::vec3(0.24f, 3.0f, 0.24f));
            }
        }
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texMadera, kiosco, glm::vec3(0.0f, 1.15f, 0.98f),
            glm::vec3(3.15f, 0.22f, 0.55f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texMadera, kiosco, glm::vec3(0.0f, 0.57f, 1.07f),
            glm::vec3(2.95f, 1.0f, 0.25f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texMadera, kiosco, glm::vec3(0.0f, 1.0f, -1.05f),
            glm::vec3(2.95f, 1.65f, 0.20f));
        for (int mitad = 0; mitad < 2; mitad++) {
            glm::mat4 mitadTecho = glm::translate(kiosco,
                glm::vec3(mitad == 0 ? -1.0f : 1.0f, 3.35f, 0.0f));
            mitadTecho = glm::rotate(mitadTecho,
                glm::radians(mitad == 0 ? 17.0f : -17.0f),
                glm::vec3(0.0f, 0.0f, 1.0f));
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texTechoKiosco, mitadTecho, glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(2.35f, 0.18f, 3.7f));
        }
        // Letrero frontal del kiosco: placa de metal sobre el mostrador.
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texMetal, kiosco, glm::vec3(0.0f, 1.55f, 1.21f),
            glm::vec3(1.25f, 0.38f, 0.08f));

        // JARDINERAS RECTANGULARES: dos delante, dos a los costados
        GLfloat lugaresJardinera[][2] = {
            {-11.0f, 8.0f}, {26.0f, 8.0f},
            {-11.0f, -17.0f}, {26.0f, -17.0f}
        };
        for (int ja = 0; ja < 4; ja++) {
            glm::mat4 jardinera = glm::translate(identidad,
                glm::vec3(lugaresJardinera[ja][0], 0.0f,
                    lugaresJardinera[ja][1]));
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texBarroExterior, jardinera, glm::vec3(0.0f, 0.15f, 0.0f),
                glm::vec3(2.6f, 0.30f, 1.3f));
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texTierra, jardinera, glm::vec3(0.0f, 0.72f, 0.0f),
                glm::vec3(2.15f, 0.06f, 0.85f));
            for (int borde = 0; borde < 4; borde++) {
                CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                    texBarroExterior, jardinera,
                    glm::vec3(borde == 0 ? -1.23f :
                        (borde == 1 ? 1.23f : 0.0f), 0.52f,
                        borde == 2 ? -0.58f :
                        (borde == 3 ? 0.58f : 0.0f)),
                    glm::vec3(borde < 2 ? 0.18f : 2.6f, 0.58f,
                        borde < 2 ? 1.3f : 0.18f));
            }
            for (int flor = 0; flor < 3; flor++) {
                float fx = (flor - 1) * 0.68f;
                float fz = flor == 1 ? -0.15f : 0.14f;
                CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                    texVerdeExterior, jardinera,
                    glm::vec3(fx, 1.06f, fz),
                    glm::vec3(0.10f, 0.73f, 0.10f));
                for (int hoja = 0; hoja < 2; hoja++) {
                    glm::mat4 hojaBase = glm::translate(jardinera,
                        glm::vec3(fx, 1.07f, fz));
                    hojaBase = glm::rotate(hojaBase,
                        glm::radians(hoja == 0 ? 40.0f : -40.0f),
                        glm::vec3(0.0f, 0.0f, 1.0f));
                    CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                        texVerdeExterior, hojaBase,
                        glm::vec3(hoja == 0 ? -0.14f : 0.14f, 0.0f, 0.0f),
                        glm::vec3(0.34f, 0.13f, 0.13f));
                }
                CuboExteriorColor(coreShader, VAO, modelLoc, colorLoc,
                    jardinera, glm::vec3(fx, 1.48f, fz),
                    glm::vec3(0.24f, 0.24f, 0.24f),
                    flor == 1 ? glm::vec3(0.97f, 0.75f, 0.20f)
                    : glm::vec3(0.88f, 0.24f, 0.33f));
            }
        }

        // COCHE SENCILLO: se traslada sobre la carretera.
        // Al llegar a un extremo, cambia de sentido y gira 180 grados.
        glm::mat4 coche = glm::translate(identidad,
            glm::vec3(cochePosX, 0.0f, 32.5f));
        coche = glm::rotate(coche,
            glm::radians(cocheHaciaDerecha ? 0.0f : 180.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texPinturaCoche, coche, glm::vec3(0.0f, 0.84f, 0.0f),
            glm::vec3(3.9f, 0.68f, 1.75f));
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texPinturaCoche, coche, glm::vec3(-0.15f, 1.38f, 0.0f),
            glm::vec3(2.05f, 0.62f, 1.46f));
        for (int lado = 0; lado < 2; lado++) {
            float zLado = lado == 0 ? -0.765f : 0.765f;
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texCristalCoche, coche, glm::vec3(-0.15f, 1.44f, zLado),
                glm::vec3(1.58f, 0.36f, 0.075f));
            // Ruedas en forma de cubos, como el resto del proyecto.
            for (int rueda = 0; rueda < 2; rueda++) {
                float xRueda = rueda == 0 ? -1.18f : 1.18f;
                CuboExteriorColor(coreShader, VAO, modelLoc, colorLoc,
                    coche, glm::vec3(xRueda, 0.36f, lado == 0 ? -0.91f : 0.91f),
                    glm::vec3(0.64f, 0.58f, 0.23f),
                    glm::vec3(0.10f, 0.11f, 0.13f));
                CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                    texMetal, coche, glm::vec3(xRueda, 0.36f,
                        lado == 0 ? -1.045f : 1.045f),
                    glm::vec3(0.27f, 0.27f, 0.035f));
            }
        }
        // Parabrisas y ventana trasera.
        for (int frente = 0; frente < 2; frente++) {
            CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
                texCristalCoche, coche,
                glm::vec3(frente == 0 ? 0.91f : -1.19f, 1.45f, 0.0f),
                glm::vec3(0.065f, 0.38f, 1.20f));
        }
        // Defensa delantera y luces.
        CuboExteriorTextura(museoShader, VAOTextura, modelMuseo,
            texMetal, coche, glm::vec3(2.00f, 0.57f, 0.0f),
            glm::vec3(0.16f, 0.16f, 1.85f));
        for (int faro = 0; faro < 2; faro++) {
            CuboExteriorColor(coreShader, VAO, modelLoc, colorLoc,
                coche, glm::vec3(1.98f, 0.96f,
                    faro == 0 ? -0.55f : 0.55f),
                glm::vec3(0.07f, 0.22f, 0.28f),
                glm::vec3(0.98f, 0.88f, 0.53f));
        }

        // Dejar activo el shader y el VAO original del robot y puertas.
        coreShader.Use();
        glBindVertexArray(VAO);

        // Se construyó el robot con piezas articuladas.
        glm::mat4 robotBase = glm::mat4(1.0f);
        robotBase = glm::translate(robotBase,
            glm::vec3(16.0f, 0.0f, 14.0f + avanceRobot));

        for (int r = 0; r < sizeof(piezasFijasRobot) / sizeof(piezasFijasRobot[0]); r++) {
            model = glm::translate(robotBase,
                glm::vec3(piezasFijasRobot[r][0],
                    piezasFijasRobot[r][1], piezasFijasRobot[r][2]));
            model = glm::scale(model,
                glm::vec3(piezasFijasRobot[r][3],
                    piezasFijasRobot[r][4], piezasFijasRobot[r][5]));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, piezasFijasRobot[r][6],
                piezasFijasRobot[r][7], piezasFijasRobot[r][8]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glm::mat4 cabezaBase = glm::translate(robotBase,
            glm::vec3(0.0f, 2.04f, 0.0f));
        cabezaBase = glm::rotate(cabezaBase,
            glm::radians(cabezaRobot), glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::translate(cabezaBase, glm::vec3(0.0f, 0.28f, 0.0f));
        model = glm::scale(model, glm::vec3(0.85f, 0.68f, 0.68f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.88f, 0.93f, 0.96f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::translate(cabezaBase, glm::vec3(0.0f, 0.28f, 0.36f));
        model = glm::scale(model, glm::vec3(0.70f, 0.43f, 0.055f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.06f, 0.16f, 0.29f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (int ojo = 0; ojo < 2; ojo++) {
            model = glm::translate(cabezaBase,
                glm::vec3(ojo == 0 ? -0.18f : 0.18f, 0.34f, 0.405f));
            model = glm::scale(model, glm::vec3(0.11f, 0.11f, 0.05f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.99f, 0.84f, 0.30f);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        model = glm::translate(cabezaBase, glm::vec3(0.0f, 0.15f, 0.405f));
        model = glm::scale(model, glm::vec3(0.26f, 0.035f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.45f, 0.86f, 0.87f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::translate(cabezaBase, glm::vec3(0.0f, 0.73f, 0.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.24f, 0.07f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.15f, 0.31f, 0.46f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::translate(cabezaBase, glm::vec3(0.0f, 0.88f, 0.0f));
        model = glm::scale(model, glm::vec3(0.18f, 0.14f, 0.18f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.99f, 0.78f, 0.25f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (int brazo = 0; brazo < 2; brazo++) {
            bool derecho = brazo == 1;
            glm::mat4 hombro = glm::translate(robotBase,
                glm::vec3(derecho ? 0.61f : -0.61f, 1.75f, 0.0f));
            if (derecho && estadoRobot >= 2 && estadoRobot <= 4) {
                hombro = glm::rotate(hombro, glm::radians(brazoSaludoRobot),
                    glm::vec3(0.0f, 0.0f, 1.0f));
            }
            else {
                hombro = glm::rotate(hombro,
                    glm::radians(derecho ? pasoRobot : -pasoRobot),
                    glm::vec3(1.0f, 0.0f, 0.0f));
            }
            model = glm::translate(hombro, glm::vec3(0.0f, -0.39f, 0.0f));
            model = glm::scale(model, glm::vec3(0.26f, 0.74f, 0.32f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.26f, 0.62f, 0.72f);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            model = glm::translate(hombro, glm::vec3(0.0f, -0.79f, 0.0f));
            model = glm::scale(model, glm::vec3(0.31f, 0.17f, 0.35f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.98f, 0.77f, 0.31f);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (int pierna = 0; pierna < 2; pierna++) {
            bool derecha = pierna == 1;
            glm::mat4 cadera = glm::translate(robotBase,
                glm::vec3(derecha ? 0.22f : -0.22f, 0.86f, 0.0f));
            cadera = glm::rotate(cadera,
                glm::radians(derecha ? -pasoRobot : pasoRobot),
                glm::vec3(1.0f, 0.0f, 0.0f));

            model = glm::translate(cadera, glm::vec3(0.0f, -0.35f, 0.0f));
            model = glm::scale(model, glm::vec3(0.28f, 0.69f, 0.34f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.19f, 0.51f, 0.65f);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            model = glm::translate(cadera, glm::vec3(0.0f, -0.70f, 0.15f));
            model = glm::scale(model, glm::vec3(0.37f, 0.17f, 0.57f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.09f, 0.19f, 0.35f);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Se configuraron dos grupos de puertas.
        GLint alphaLoc = glGetUniformLocation(coreShader.Program, "alpha");
        glUniform1f(alphaLoc, 1.0f);

        for (int puerta = 0; puerta < 4; puerta++) {
            glm::mat4 bisagra = glm::mat4(1.0f);
            float angulo = puerta < 2 ? anguloPuertas : anguloPuertasNuevas;

            if (puerta == 0) {
                bisagra = glm::translate(bisagra, glm::vec3(-5.30f, 0.0f, -1.0f));
                bisagra = glm::rotate(bisagra, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (puerta == 1) {
                bisagra = glm::translate(bisagra, glm::vec3(15.0f, 0.0f, -4.64f));
                bisagra = glm::rotate(bisagra, glm::radians(-angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (puerta == 2) {
                bisagra = glm::translate(bisagra, glm::vec3(15.0f, 0.0f, -14.65f));
                bisagra = glm::rotate(bisagra, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else {
                bisagra = glm::translate(bisagra, glm::vec3(-5.30f, 0.0f, -20.40f));
                bisagra = glm::rotate(bisagra, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            for (int parte = 0; parte < 4; parte++) {
                GLfloat alto = parte < 2 ? 3.0f : 0.10f;
                GLfloat posicion = parte == 0 ? 0.06f : (parte == 1 ? 1.94f : 1.0f);
                GLfloat altura = parte == 2 ? 0.05f : (parte == 3 ? 2.95f : 1.50f);
                model = bisagra;

                if (puerta == 0 || puerta == 3) {
                    model = glm::translate(model, glm::vec3(0.0f, altura, posicion));
                    model = glm::scale(model, glm::vec3(0.10f, alto, parte < 2 ? 0.10f : 2.0f));
                }
                else {
                    model = glm::translate(model, glm::vec3(posicion, altura, 0.0f));
                    model = glm::scale(model, glm::vec3(parte < 2 ? 0.10f : 2.0f, alto, 0.10f));
                }
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3f(colorLoc, 0.20f, 0.21f, 0.23f);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            model = bisagra;
            if (puerta == 0 || puerta == 3)
                model = glm::translate(model, glm::vec3(-0.10f, 1.50f, 1.75f));
            else
                model = glm::translate(model, glm::vec3(1.75f, 1.50f, -0.10f));
            model = glm::scale(model, glm::vec3(0.11f, 0.13f, 0.11f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.86f, 0.72f, 0.32f);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Se añadieron los vidrios transparentes.
        glUniform1f(alphaLoc, 0.25f);
        for (int puerta = 0; puerta < 4; puerta++) {
            glm::mat4 bisagra = glm::mat4(1.0f);
            float angulo = puerta < 2 ? anguloPuertas : anguloPuertasNuevas;

            if (puerta == 0) {
                bisagra = glm::translate(bisagra, glm::vec3(-5.30f, 0.0f, -1.0f));
                bisagra = glm::rotate(bisagra, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (puerta == 1) {
                bisagra = glm::translate(bisagra, glm::vec3(15.0f, 0.0f, -4.64f));
                bisagra = glm::rotate(bisagra, glm::radians(-angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (puerta == 2) {
                bisagra = glm::translate(bisagra, glm::vec3(15.0f, 0.0f, -14.65f));
                bisagra = glm::rotate(bisagra, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else {
                bisagra = glm::translate(bisagra, glm::vec3(-5.30f, 0.0f, -20.40f));
                bisagra = glm::rotate(bisagra, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
            }

            if (puerta == 0 || puerta == 3) {
                model = glm::translate(bisagra, glm::vec3(0.0f, 1.50f, 1.0f));
                model = glm::scale(model, glm::vec3(0.045f, 2.78f, 1.79f));
            }
            else {
                model = glm::translate(bisagra, glm::vec3(1.0f, 1.50f, 0.0f));
                model = glm::scale(model, glm::vec3(1.79f, 2.78f, 0.045f));
            }
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.58f, 0.79f, 0.91f);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glUniform1f(alphaLoc, 1.0f);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAOTextura);
    glDeleteBuffers(1, &VBOTextura);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        puertasAbiertas = !puertasAbiertas;
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
        puertasNuevasAbiertas = !puertasNuevasAbiertas;
    if (key == GLFW_KEY_R && action == GLFW_PRESS && estadoRobot == 0)
        estadoRobot = 1;
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        cocheEnMovimiento = !cocheEnMovimiento;
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        fuenteEncendida = !fuenteEncendida;

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }

    // Se asignaron atajos para recorrer el museo.
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1) camera = Camera(glm::vec3(8.0f, 2.0f, 14.0f));
        if (key == GLFW_KEY_2) camera = Camera(glm::vec3(-1.0f, 1.7f, 3.2f));
        if (key == GLFW_KEY_3) camera = Camera(glm::vec3(16.0f, 1.7f, 3.2f));
        if (key == GLFW_KEY_4) camera = Camera(glm::vec3(7.0f, 1.7f, -17.0f));
        if (key == GLFW_KEY_5) camera = Camera(glm::vec3(8.0f, 10.2f, -13.0f));
        if (key == GLFW_KEY_6) camera = Camera(glm::vec3(8.0f, 8.0f, 27.0f));
        if (key == GLFW_KEY_7) camera = Camera(glm::vec3(-1.0f, 5.7f, -17.5f));
        if (key == GLFW_KEY_8) camera = Camera(glm::vec3(16.0f, 2.5f, 20.5f));
        // Vista panoramica de la plaza y la calle.
        if (key == GLFW_KEY_9) camera = Camera(glm::vec3(8.0f, 8.5f, 56.0f));
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) firstMouse = true;
    }
}

// Se animó la apertura y el cierre de las puertas.
void Animation()
{
    // Movimiento automatico del coche con velocidad independiente de FPS.
    if (cocheEnMovimiento) {
        if (cocheHaciaDerecha) cochePosX += 7.5f * deltaTime;
        else cochePosX -= 7.5f * deltaTime;
        if (cochePosX >= 57.0f) {
            cochePosX = 57.0f;
            cocheHaciaDerecha = false;
        }
        if (cochePosX <= -57.0f) {
            cochePosX = -57.0f;
            cocheHaciaDerecha = true;
        }
    }

    // Chorro que sube y baja suavemente en la fuente.
    if (fuenteEncendida) {
        if (chorroSubiendo) chorroFuente += 0.60f * deltaTime;
        else chorroFuente -= 0.60f * deltaTime;
        if (chorroFuente >= 1.50f) {
            chorroFuente = 1.50f;
            chorroSubiendo = false;
        }
        if (chorroFuente <= 0.50f) {
            chorroFuente = 0.50f;
            chorroSubiendo = true;
        }
    }

    if (puertasAbiertas && anguloPuertas < 80.0f)
        anguloPuertas += 0.4f;
    if (!puertasAbiertas && anguloPuertas > 0.0f)
        anguloPuertas -= 0.4f;

    if (puertasNuevasAbiertas && anguloPuertasNuevas < 80.0f)
        anguloPuertasNuevas += 0.4f;
    if (!puertasNuevasAbiertas && anguloPuertasNuevas > 0.0f)
        anguloPuertasNuevas -= 0.4f;

    // Se programaron el recorrido y saludo del robot.
    if (estadoRobot == 1 || estadoRobot == 5) {
        if (pasoSubiendoRobot) pasoRobot += 1.0f;
        else pasoRobot -= 1.0f;
        if (pasoRobot >= 20.0f) pasoSubiendoRobot = false;
        if (pasoRobot <= -20.0f) pasoSubiendoRobot = true;
    }

    if (estadoRobot == 1) {
        avanceRobot += 0.012f;
        if (avanceRobot >= 1.50f) {
            avanceRobot = 1.50f;
            pasoRobot = 0.0f;
            estadoRobot = 2;
        }
    }
    else if (estadoRobot == 2) {
        if (brazoSaludoRobot < 67.0f) brazoSaludoRobot += 1.0f;
        if (cabezaRobot < 12.0f) cabezaRobot += 0.25f;
        if (brazoSaludoRobot >= 67.0f) estadoRobot = 3;
    }
    else if (estadoRobot == 3) {
        if (saludoSubiendoRobot) brazoSaludoRobot += 1.0f;
        else brazoSaludoRobot -= 1.0f;
        if (brazoSaludoRobot >= 84.0f) saludoSubiendoRobot = false;
        if (brazoSaludoRobot <= 57.0f) {
            saludoSubiendoRobot = true;
            saludosRobot++;
            if (saludosRobot >= 3) estadoRobot = 4;
        }
    }
    else if (estadoRobot == 4) {
        if (brazoSaludoRobot > 0.0f) brazoSaludoRobot -= 1.0f;
        if (cabezaRobot > 0.0f) cabezaRobot -= 0.25f;
        if (brazoSaludoRobot <= 0.0f) {
            brazoSaludoRobot = 0.0f;
            cabezaRobot = 0.0f;
            estadoRobot = 5;
        }
    }
    else if (estadoRobot == 5) {
        avanceRobot -= 0.012f;
        if (avanceRobot <= 0.0f) {
            avanceRobot = 0.0f;
            pasoRobot = 0.0f;
            estadoRobot = 0;
            saludosRobot = 0;
            saludoSubiendoRobot = true;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse) {
        lastX = (GLfloat)xPos;
        lastY = (GLfloat)yPos;
        firstMouse = false;
    }
    GLfloat xOffset = (GLfloat)xPos - lastX;
    GLfloat yOffset = lastY - (GLfloat)yPos;
    lastX = (GLfloat)xPos;
    lastY = (GLfloat)yPos;
    camera.ProcessMouseMovement(xOffset, yOffset);
}
