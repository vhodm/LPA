#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

// Estrutura para representar um ponto
typedef struct {
    float x, y;
} Point;

// Estrutura para representar um segmento de linha
typedef struct {
    Point p1, p2;
} Segment;

// Variáveis globais
int windowWidth, windowHeight;
int numSegments;
Segment *segments = NULL;

// Função para verificar a orientação de três pontos (Counter Clockwise)
int ccw(Point A, Point B, Point C) {
    return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
}

// Função para verificar se dois segmentos se intersectam
bool intersect(Segment s1, Segment s2) {
    Point A = s1.p1, B = s1.p2;
    Point C = s2.p1, D = s2.p2;
    
    return (ccw(A, C, D) != ccw(B, C, D)) && 
           (ccw(A, B, C) != ccw(A, B, D));
}

// Função para verificar se um novo segmento intersecta com algum existente
bool intersectsAny(Segment newSegment) {
    for (int i = 0; i < numSegments; i++) {
        if (intersect(newSegment, segments[i])) {
            return true;
        }
    }
    return false;
}

// Função para gerar um ponto aleatório dentro da janela
Point randomPoint() {
    Point p;
    p.x = rand() % windowWidth;
    p.y = rand() % windowHeight;
    return p;
}

// Função para gerar segmentos aleatórios sem interseção
void generateSegments() {
    if (segments) free(segments);
    segments = (Segment*)malloc(numSegments * sizeof(Segment));
    
    for (int i = 0; i < numSegments; i++) {
        Segment newSegment;
        int attempts = 0;
        bool valid = false;
        
        while (!valid && attempts < 1000) {
            newSegment.p1 = randomPoint();
            newSegment.p2 = randomPoint();
            
            // Verificar se os pontos são diferentes
            if (fabs(newSegment.p1.x - newSegment.p2.x) < 1e-5 && 
                fabs(newSegment.p1.y - newSegment.p2.y) < 1e-5) {
                attempts++;
                continue;
            }
            
            // Verificar interseções
            if (!intersectsAny(newSegment)) {
                valid = true;
            }
            attempts++;
        }
        
        if (!valid) {
            // Se não encontrou um segmento válido, usa um padrão
            newSegment.p1.x = 0; newSegment.p1.y = 0;
            newSegment.p2.x = windowWidth; newSegment.p2.y = windowHeight;
        }
        
        segments[i] = newSegment;
    }
}

// Função de display para renderização OpenGL
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Desenhar todos os segmentos
    for (int i = 0; i < numSegments; i++) {
        glColor3f(0.0, 0.0, 0.0); // Cor preta
        glBegin(GL_LINES);
        glVertex2f(segments[i].p1.x, segments[i].p1.y);
        glVertex2f(segments[i].p2.x, segments[i].p2.y);
        glEnd();
    }
    
    glFlush();
}

int main(int argc, char **argv) {
    // Verificar argumentos
    if (argc != 4) {
        printf("Uso: %s <largura> <altura> <num_segmentos>\n", argv[0]);
        return 1;
    }
    
    windowWidth = atoi(argv[1]);
    windowHeight = atoi(argv[2]);
    numSegments = atoi(argv[3]);
    
    // Inicializar gerador de números aleatórios
    srand(time(NULL));
    
    // Gerar segmentos
    generateSegments();
    
    // Inicializar OpenGL/GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Segmentos Aleatorios sem Interseccao");
    
    // Configurar projeção ortográfica
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
    
    // Configurar cor de fundo
    glClearColor(1.0, 1.0, 1.0, 1.0); // Fundo branco
    
    // Registrar função de display
    glutDisplayFunc(display);
    
    // Iniciar loop principal
    glutMainLoop();
    
    // Liberar memória
    free(segments);
    
    return 0;
}