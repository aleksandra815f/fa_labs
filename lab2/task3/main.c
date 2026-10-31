#include <stdio.h>
#include <stdlib.h>
#include "logic.h"
#include "status.h"

static void print_reports(const SearchReport *reports, int file_count) {
    for (int i = 0; i < file_count; i++) {
        if (reports[i].filename != NULL) {
            printf("Файл: %s\n", reports[i].filename);
        } else {
            printf("Файл: NULL\n");
        }
        if (reports[i].status != STATUS_OK) {
            printf("    Ошибка: %s\n", status_message(reports[i].status));
        } else if (reports[i].match_count == 0) {
            printf(" Совпадений не найдено\n");
        } else {
            for (size_t j = 0; j < reports[i].match_count; j++) {
                printf(" Найдено: строка %zu, символ %zu\n", reports[i].matches[j].line, reports[i].matches[j].col);
            }
        }
    }
}

int main(void) {
    SearchReport *reports = NULL;

    printf("Вывод результатов работы функции\n");

    printf("\n1. Подстрока \"hello\" в test1.txt:\n");
    search_in_files(&reports, "hello", 1, "test1.txt" );
    print_reports(reports, 1);
    free_search_reports(reports, 1);

    printf("\n2. Пересекающиеся вхождения \"aa\" в test2.txt:\n");
    search_in_files(&reports, "aa", 1, "test2.txt");
    print_reports(reports, 1);
    free_search_reports(reports, 1);

    printf("\n3. Подстрока с переносом строки \"d\\nhe\":\n");
    search_in_files(&reports, "\\n", 1, "test3.txt");
    print_reports(reports, 1);
    free_search_reports(reports, 1);

    printf("\n4. Один файл существует, другой нет:\n");
    search_in_files(&reports, "hello", 3, "test1.txt", "missing.txt", "empty.txt");
    print_reports(reports, 3);
    free_search_reports(reports, 3);

    printf("\n5. Валидация (NULL и отрицательное число):\n");
    if (search_in_files(NULL, "hello", 1, "test1.txt") != STATUS_OK) {
        printf("Ошибка NULL-указателя.\n");
    }
    if (search_in_files(&reports, "hello", -1) != STATUS_OK) {
        printf("Ошибка отрицательного количества.\n");
    }

    printf("\n6. Подстрока \" \"(пробел) в test1.txt:\n");
    search_in_files(&reports, " ", 1, "test1.txt");
    print_reports(reports, 1);
    free_search_reports(reports, 1);

    return 0;
}