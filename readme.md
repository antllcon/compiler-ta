# Курс по теории автоматов

В репозитории представлены лабораторные работы Глухарева Степана за 3 курс. 
Формат входных данных для конечных автоматов описан ниже.

``` dot
digraph AutmoatonExample // Не обязательно, но всегда приятно
{
    start = 0; // Может быть только одно начальное состояние
    final = 2, 4; // Может быть несколько конечных вершин
    // GraphvizOnline - не поддерживает более однгого конечного состояния

    3 -> 1; // e-переход
    0 -> 1 [label = "a"];
    1 -> 2 [label = "b"];
    2 -> 3 [label = "c"]; // Обычные переходы
    3 -> 4 [label = "a, c"]; // Может быть несколько букв алфавита для перехода
}
```

### Ссылки

1. [Визуализация `.dot` файлов](https://dreampuf.github.io/GraphvizOnline/?engine=dot)
2. [Успеваемость](https://docs.google.com/spreadsheets/d/1MveN0XK32TYu8BAC9km3E9S3hN0OByp5jz3-Hcmxu2I/edit?pli=1&gid=0#gid=0) 
3. [Критерии сдачи](https://docs.google.com/document/d/1XSP9ryvPh3p6PtoL_BeCKyvZUkwQ-PgJ4TsTu3HgLBA/edit?tab=t.o1x1jw4l5ks8)