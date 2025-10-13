
https://dreampuf.github.io/GraphvizOnline/?engine=dot

``` dot
digraph example {
    start = 0; // Может быть только одно начальное состояние
    final = 2, 4; // Может быть несколько конечных вершин

    0 -> 1 [label = "a"];
    1 -> 2 [label = "b"];
    2 -> 3 [label = "c"];
    3 -> 1; // e-переход
    3 -> 4 [label = "a, c"]; // может быть несколько букв алфавита для перехода
}
```