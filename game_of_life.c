
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 80       // ширина поля
#define HEIGHT 25      // высота поля
#define BASIC_SPEED 5  // начальная скорость

int start_of_game(int **pointer_array);                 // начало игры
void paint_pole(int **pointer_array);                   // отрисовка поля
int countNeighbors(int **pointer_array, int x, int y);  // подсчет живых соседей
void update_pointer_array(int **pointer_array);         // обновление массива
int check_over_game(int **pointer_array);  // проверка есть ли живые клетки
void clear_memory(int **arr, int length);  // очистка памяти
int get_symbols(int *time);                // изменение скорости или выход
void end_of_game(int **pointer_array, int **pointer_before);  // в конце игры
void copy(int **pointer_array, int **pointer_before);         // копирование массива
int check(int **pointer_array, int **pointer_before);  // проверка изменяются ли живые клетки
void input(int **pointer_array, FILE *fptr);  // ввод массива
int input_user(int **pointer_array);          // ввод массива самим юзером

/*
clear() - очистка экрана
refresh() - обновляет экран, например, его очистили и пока не рефрешним экран в терминале не очистится
mvprintw(x, y, ...) - выводит на экран что-то по координатам x и y
noecho() - когда мы вводим символы в терминал - мы видим что вводим - noecho() убирает это (теперь вводимые
символы не отображаются) halfdelay(time) - режим работы getch() - добавляет ожидание ввода пользователем
символа в течение time-десятых секунд (программа чиилит в это время) getch() - почти тоже самое, что и
getchar() - также считывает 1 символ usleep(time) - у программы тихий час на time-миллисекунд (time =
1_000_000  -  1 секунда)
*/

int main() {
    int **pointer_array = malloc(HEIGHT * sizeof(int *)),
        **pointer_before = malloc(HEIGHT * sizeof(int *));  // инициализация 2-х массивов
    for (int i = 0; i < HEIGHT; i++) {
        pointer_array[i] = malloc(WIDTH * sizeof(int));
        pointer_before[i] = malloc(WIDTH * sizeof(int));
    }
    initscr();    // выделяет память для нашего текущего окна
    curs_set(0);  // убирает курсор
    int flag = start_of_game(pointer_array), time = BASIC_SPEED;
    if (!flag) {
        int lost = 1;
        while (lost) {  // пока не проиграли или пока не вышли
            clear();
            refresh();
            paint_pole(pointer_array);            // рисуем поле
            refresh();                            // обнова экрана
            copy(pointer_array, pointer_before);  // копируем массив
            update_pointer_array(pointer_array);  // обновляем его
            lost = check(
                pointer_array,
                pointer_before);  // чекаем не равны ли они - если массивы равны то значит ничего не двигается
            int flag_symbols = get_symbols(&time);  // вводим скорость программы или выход
            if (flag_symbols == -1) {               // это если нажали букву q - выход
                lost = 0;
            }
            if (lost) {
                lost = check_over_game(pointer_array);  // проверка есть ли живые челы
            }
        }
    }
    end_of_game(pointer_array, pointer_before);
    endwin();
    printf("Game Over");
    return 0;
}

int start_of_game(int **pointer_array) {
    FILE *fptr;
    printw("Hello \"god\" of this world, again you will watch how cells are born and die\n");
    printw(
        "Well, what will you choose\n1) - select 1 ready-made matrix\n2) - select 2 ready-made matrix\n3) - "
        "select 3 ready-made matrix"
        "\n4) - select the 4th ready matrix\n5) - select the 5th ready matrix\n6) - Enter array\n");
    noecho();
    int ch = getch(), flag = 0;
    if (ch == '1') {
        fptr = fopen("init_state_ak47_reaction.txt", "r");
    } else if (ch == '2') {
        fptr = fopen("init_state_ark.txt", "r");
    } else if (ch == '3') {
        fptr = fopen("init_state_average.txt", "r");
    } else if (ch == '4') {
        fptr = fopen("init_state_cycle.txt", "r");
    } else if (ch == '5') {
        fptr = fopen("init_state_pulsar.txt", "r");
    } else if (ch == '6') {
        flag = 1;
        fptr = NULL;
    } else {
        flag = -1;
        fptr = NULL;
    }
    if (fptr != NULL) {
        input(pointer_array, fptr);
    } else if (flag == 1) {
        printw("\n");
        if (input_user(pointer_array) == -1) {
            flag = -1;
        } else {
            flag = 0;
        }
    }
    return flag;
}

void input(int **pointer_array, FILE *fptr) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            pointer_array[i][j] = (int)fgetc(fptr) - 48;
        }
    }
}

int input_user(int **pointer_array) {
    int flag = 0;
    printw(
        "Enter an array of cells, where 0 is dead, 1 is live\nEnter without spaces or other characters:\n");
    echo();
    curs_set(1);
    for (int i = 0; i < HEIGHT && !flag; i++) {
        for (int j = 0; j < WIDTH && !flag; j++) {
            pointer_array[i][j] = (int)getch() - 48;
            if (!(pointer_array[i][j] == 1 || pointer_array[i][j] == 0)) {
                flag = -1;
            }
        }
    }
    curs_set(0);
    return flag;
}

void paint_pole(int **pointer_array) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvprintw(i, j, "%c", (pointer_array[i][j] ? 'o' : '.'));
        }
    }
    mvprintw(HEIGHT, 0, "%c", ' ');
}

int countNeighbors(int **pointer_array, int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!(i == 0 && j == 0)) {
                int newX = (x + i + HEIGHT) % HEIGHT;
                int newY = (y + j + WIDTH) % WIDTH;
                count += pointer_array[newX][newY];
            }
        }
    }
    return count;
}

void update_pointer_array(int **pointer_array) {
    int **newpointer_array = malloc(HEIGHT * sizeof(int *));
    for (int i = 0; i < HEIGHT; i++) {
        newpointer_array[i] = malloc(WIDTH * sizeof(int));
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int neighbors = countNeighbors(pointer_array, i, j);
            if (pointer_array[i][j] == 1) {
                newpointer_array[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                newpointer_array[i][j] = (neighbors == 3) ? 1 : 0;
            }
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            pointer_array[i][j] = newpointer_array[i][j];
        }
    }
    clear_memory(newpointer_array, HEIGHT);
}

int check_over_game(int **pointer_array) {
    int count = 0, flag = 0;
    for (int i = 0; i < HEIGHT && !flag; i++) {
        for (int j = 0; j < WIDTH && !flag; j++) {
            if (pointer_array[i][j] == 1) {
                count++;
                flag = 1;
            }
        }
    }
    return count;
}

int get_symbols(int *time) {
    mvprintw(
        HEIGHT, 0,
        "Now is your speed: %d\nTo change speed press numbers 1, 2, 3, 4, 5. To exit the game press - \'q\'",
        13 - *time);
    refresh();
    noecho();
    halfdelay(*time);
    int ch = getch(), flag = 0;
    if (ch == '1') {
        *time = 12;
    } else if (ch == '2') {
        *time = 9;
    } else if (ch == '3') {
        *time = 6;
    } else if (ch == '4') {
        *time = 3;
    } else if (ch == '5') {
        *time = 1;
    } else if (ch == 'q' || ch == 'Q') {
        flag = -1;
    }
    return flag;
}

void copy(int **pointer_array, int **pointer_before) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            pointer_before[i][j] = pointer_array[i][j];
        }
    }
}

int check(int **pointer_array, int **pointer_before) {
    int flag = 0;
    for (int i = 0; i < HEIGHT && !flag; i++) {
        for (int j = 0; j < WIDTH && !flag; j++) {
            if (pointer_array[i][j] != pointer_before[i][j]) {
                flag = 1;
            }
        }
    }
    return flag;
}

void end_of_game(int **pointer_array, int **pointer_before) {
    clear_memory(pointer_array, HEIGHT);
    clear_memory(pointer_before, HEIGHT);
    usleep(2000000);
    clear();
    refresh();
    mvprintw(0, 0, "%s", "GAME OVER\n\nplz wait ...");
    refresh();
    usleep(2000000);
}

void clear_memory(int **arr, int length) {
    for (int i = 0; i < length; i++) {
        free(arr[i]);
    }
    free(arr);
}
