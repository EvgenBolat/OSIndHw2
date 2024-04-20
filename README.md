# Хавронич Евгений Алексеевич, БПИ226
---
## Вариант 18
### Условие:
Разработать программу, которая меняет на обратный порядок следование слов в ASCII–строке символов. То есть,слова остаются написанными правильно. К слову относится последовательность любых символов,неразделяемая пробелами,символами перевода строки,табуляциями.
### Программы:

### Требования на 8:
- Даны две программы, которые олицетворяют два типа независимых сущностей. Пользователь запускает обе программы (сначала SalesMans, затем Customer). Содержимое обеих программ аналогично той, что выполнена на 4-5 баллов с изменением в виде переписывания под семафоры в стандарте UNIX SYSTEM V. В ходе выполненя программ в них записываются соответствующие сообщения о состоянии на текущий момент. При нажатии ctr + c всё корректно удаляется.

### Требования на 7-6:
- Программа аналогична той, что описана для баллов 5-4, но переписана под неименованные POSIX семафоры. То есть создаётся разделяемая память на 2 семафора, там они инициализируются (в процессе продавца), а в процессе покупателя изымаются из разделяемой памяти и используются. При нажатии ctr + c всё корректно удаляется.

### Требования на 5-4:
- Даны программа. Пользователь запускает программу. Внутри происходит fork, тем самым создавая два родственных процесса. Процесс-родитель будет являться двумя отделами, где каждый отдел с продавцом является отдельным семафором (всего их 2). В процессе-родителе через функцию запускается создание двух семафоров и увеличение их счётчиков. Процес-ребёнок начинает плодить через fork новые процессы, где каждый новый процесс является отдельным покупателем и вызывает соответствующую функцию. В функции процесс рандомно выбирает количество покупок, необходимых ему, и в каждой покупке выбирает рандомно, к какому отделу она относится, после чего пытается занять семафор (или ждёт его, если он уже занят) и после этого за некоторое время происходит процесс покупки, после которого он освобождает семафор и завершается. При нажатии ctr + c всё корректно удаляется. В данной программе используются именованные семафоры.

![](https://i.pinimg.com/564x/d8/97/a8/d897a8106f35d2125e8d814178ba9322.jpg)
