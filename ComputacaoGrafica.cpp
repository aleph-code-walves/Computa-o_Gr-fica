#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>

using namespace std;

struct Vertice3D {
    double x, y, z;
};

struct Aresta {
    int origem, destino;
};

struct Cubo {
    vector<Vertice3D> vertices;
    vector<Aresta>    arestas;
    Vertice3D         centro;
};

Cubo cubo;
int  delay = 16;
bool   botaoEsquerdoPressionado = false;
int    mouseXAnterior = 0, mouseYAnterior = 0;
double sensibilidadeMouse = 0.5;

Cubo   criar_cubo(double cx, double cy, double cz, double lado);
void   desenhar(const Cubo& c);
void   movimentar(Cubo& c, double dx, double dy, double dz);
void   escalar(Cubo& c, double sx, double sy, double sz);
void   rotacionar(Cubo& c, double angulo, char eixo);
void   rotacionar_silencioso(Cubo& c, double angulo, char eixo);
void   log_estado(const Cubo& c, const string& operacao);
void   display();
void   redraw(int value);
void   keyboard(unsigned char key, int x, int y);
void   keyboard_special(int key, int x, int y);
void   mouse_button(int button, int state, int x, int y);
void   mouse_motion(int x, int y);

int main(int argc, char** argv) {

    cubo = criar_cubo(0.0, 0.0, 0.0, 100.0);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Cubo 3D - Wireframe | WASD/QE=move  IK/JL/UO=rot  +-=escala  Mouse=rotacao");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_special);
    glutMouseFunc(mouse_button);
    glutMotionFunc(mouse_motion);
    glutTimerFunc(delay, redraw, 0);

    cout << "=== CONTROLES ===\n";
    cout << "Translacao : WASD (X/Y)  |  Q/E (Z)\n";
    cout << "Rotacao    : I/K (X)  |  J/L (Y)  |  U/O (Z)\n";
    cout << "Escala     : + (aumentar)  |  - (diminuir)\n";
    cout << "Mouse      : arraste botao esquerdo para rotacionar\n";
    cout << "ESC        : sair\n";

    glutMainLoop();
    return 0;
}
//arrumar
Cubo criar_cubo(double cx, double cy, double cz, double lado) {
    Cubo c;
    double h = lado / 2.0;

    c.vertices = {
        { cx - h, cy - h, cz - h },
        { cx + h, cy - h, cz - h },
        { cx + h, cy - h, cz + h },
        { cx - h, cy - h, cz + h },
        { cx - h, cy + h, cz - h },
        { cx + h, cy + h, cz - h },
        { cx + h, cy + h, cz + h },
        { cx - h, cy + h, cz + h }
    };

    c.arestas = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    c.centro = { cx, cy, cz };

    return c;
}

void movimentar(Cubo& c, double dx, double dy, double dz) {
    for (auto& v : c.vertices) {
        v.x += dx;
        v.y += dy;
        v.z += dz;
    }
    c.centro.x += dx;
    c.centro.y += dy;
    c.centro.z += dz;
    log_estado(c, "TRANSLACAO");
}

void escalar(Cubo& c, double sx, double sy, double sz) {
    for (auto& v : c.vertices) {
        v.x = c.centro.x + (v.x - c.centro.x) * sx;
        v.y = c.centro.y + (v.y - c.centro.y) * sy;
        v.z = c.centro.z + (v.z - c.centro.z) * sz;
    }
    log_estado(c, "ESCALA");
}

void rotacionar_silencioso(Cubo& c, double angulo, char eixo) {
    double rad = angulo * (3.14159265358979 / 180.0);
    double cosA = cos(rad);
    double sinA = sin(rad);

    for (auto& v : c.vertices) {
        double dx = v.x - c.centro.x;
        double dy = v.y - c.centro.y;
        double dz = v.z - c.centro.z;
        double nx, ny, nz;

        switch (eixo) {
        case 'x': case 'X':
            nx = dx; ny = dy * cosA - dz * sinA; nz = dy * sinA + dz * cosA; break;
        case 'y': case 'Y':
            nx = dx * cosA + dz * sinA; ny = dy; nz = -dx * sinA + dz * cosA; break;
        case 'z': case 'Z':
            nx = dx * cosA - dy * sinA; ny = dx * sinA + dy * cosA; nz = dz; break;
        default: return;
        }

        v.x = c.centro.x + nx;
        v.y = c.centro.y + ny;
        v.z = c.centro.z + nz;
    }
}

void rotacionar(Cubo& c, double angulo, char eixo) {
    rotacionar_silencioso(c, angulo, eixo);
    log_estado(c, string("ROTACAO eixo=") + eixo);
}

void log_estado(const Cubo& c, const string& operacao) {
    const Vertice3D& v0 = c.vertices[0];
    const Vertice3D& v1 = c.vertices[1];
    double lado = sqrt(pow(v1.x - v0.x, 2) + pow(v1.y - v0.y, 2) + pow(v1.z - v0.z, 2));

    cout << fixed;
    cout.precision(2);
    cout << "[" << operacao << "]"
        << "  centro=(" << c.centro.x << ", " << c.centro.y << ", " << c.centro.z << ")"
        << "  lado=" << lado
        << "\n";
}

struct Ponto2D { float x, y; };

Ponto2D projetar(const Vertice3D& v) {
    double d = 300.0;
    double fator = d / (d + v.z + 200.0);
    return {
        (float)(v.x * fator),
        (float)(v.y * fator)
    };
}

void desenhar(const Cubo& c) {
    glColor3f(0.6f, 0.0f, 1.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (const auto& a : c.arestas) {
        Ponto2D p0 = projetar(c.vertices[a.origem]);
        Ponto2D p1 = projetar(c.vertices[a.destino]);
        glVertex2f(p0.x, p0.y);
        glVertex2f(p1.x, p1.y);
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto& v : c.vertices) {
        Ponto2D p = projetar(v);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-300, 300, -300, 300, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    desenhar(cubo);

    glFlush();
}

void redraw(int value) {
    glutPostRedisplay();
    glutTimerFunc(delay, redraw, 0);
}

void keyboard(unsigned char key, int x, int y) {
    double passo = 5.0;
    double angulo = 5.0;
    double fat = 0.05;

    switch (key) {
    case 27:
        exit(0);
        break;

    case 'a': case 'A': movimentar(cubo, -passo, 0, 0); break;
    case 'd': case 'D': movimentar(cubo, passo, 0, 0); break;
    case 'w': case 'W': movimentar(cubo, 0, passo, 0); break;
    case 's': case 'S': movimentar(cubo, 0, -passo, 0); break;
    case 'q': case 'Q': movimentar(cubo, 0, 0, -passo); break;
    case 'e': case 'E': movimentar(cubo, 0, 0, passo); break;

    case 'i': case 'I': rotacionar(cubo, angulo, 'x'); break;
    case 'k': case 'K': rotacionar(cubo, -angulo, 'x'); break;
    case 'j': case 'J': rotacionar(cubo, angulo, 'y'); break;
    case 'l': case 'L': rotacionar(cubo, -angulo, 'y'); break;
    case 'u': case 'U': rotacionar(cubo, angulo, 'z'); break;
    case 'o': case 'O': rotacionar(cubo, -angulo, 'z'); break;

    case '+': case '=': escalar(cubo, 1.0 + fat, 1.0 + fat, 1.0 + fat); break;
    case '-': case '_': escalar(cubo, 1.0 - fat, 1.0 - fat, 1.0 - fat); break;

    case 'x': escalar(cubo, 1.0 + fat, 1.0, 1.0); break;
    case 'X': escalar(cubo, 1.0 - fat, 1.0, 1.0); break;
    case 'y': escalar(cubo, 1.0, 1.0 + fat, 1.0); break;
    case 'Y': escalar(cubo, 1.0, 1.0 - fat, 1.0); break;
    case 'z': escalar(cubo, 1.0, 1.0, 1.0 + fat); break;
    case 'Z': escalar(cubo, 1.0, 1.0, 1.0 - fat); break;
    }
}

void keyboard_special(int key, int x, int y) {
    double angulo = 5.0;
    switch (key) {
    case GLUT_KEY_UP:    rotacionar(cubo, angulo, 'x'); break;
    case GLUT_KEY_DOWN:  rotacionar(cubo, -angulo, 'x'); break;
    case GLUT_KEY_LEFT:  rotacionar(cubo, angulo, 'y'); break;
    case GLUT_KEY_RIGHT: rotacionar(cubo, -angulo, 'y'); break;
    }
}

void mouse_button(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        botaoEsquerdoPressionado = (state == GLUT_DOWN);
        mouseXAnterior = x;
        mouseYAnterior = y;
    }
    if (button == 3) {
        escalar(cubo, 1.05, 1.05, 1.05);
    }
    if (button == 4) {
        escalar(cubo, 0.95, 0.95, 0.95);
    }
}

void mouse_motion(int x, int y) {
    if (!botaoEsquerdoPressionado) return;

    double dx = (x - mouseXAnterior) * sensibilidadeMouse;
    double dy = (y - mouseYAnterior) * sensibilidadeMouse;

    rotacionar_silencioso(cubo, dx, 'y');
    rotacionar_silencioso(cubo, -dy, 'x');

    mouseXAnterior = x;
    mouseYAnterior = y;
}