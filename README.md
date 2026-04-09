# Cubo 3D - Wireframe

Visualizador de cubo 3D com wireframe usando OpenGL/GLUT. As transformações de translação, rotação e escala são implementadas manualmente, sem uso de `glTranslate`, `glRotate` ou `glScale`.

## Dependências

- OpenGL
- freeglut

## Compilação

```bash
g++ cubo3d.cpp -o cubo3d -lGL -lglut -lGLU -lm
```

## Controles

| Tecla | Ação |
|---|---|
| `W A S D` | Translação em X e Y |
| `Q / E` | Translação em Z |
| `I / K` | Rotação no eixo X |
| `J / L` | Rotação no eixo Y |
| `U / O` | Rotação no eixo Z |
| Setas | Rotação em X e Y |
| `+ / -` | Escala uniforme |
| `x X y Y z Z` | Escala por eixo (minúscula aumenta, maiúscula reduz) |
| Mouse (arrastar) | Rotação livre em X e Y |
| Scroll | Escala uniforme |
| `ESC` | Sair |
