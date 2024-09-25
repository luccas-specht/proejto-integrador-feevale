#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYSLOG_PATH "/var/log/syslog"
#define MAX_LINE_LENGTH 1024

typedef struct {
    char** events;
    int size;
    int capacity;
} EventVector;

void init_event_vector(EventVector* vector){
    vector->size = 0;
    vector->capacity = 10;
    vector->events = (char**)malloc(vector->capacity * sizeof(char*));

    if(vector->events == NULL){
        exit(EXIT_FAILURE);
    }
}

void add_event(EventVector* vector, const char* log_line) {
  if(vector->size == vector->capacity){
    vector->capacity *=2;
    vector->events = (char**)realloc(vector->events, vector->capacity * sizeof(char*));
    if(vector->events == NULL){
        exit(EXIT_FAILURE);
    }
  }


  vector->events[vector->size] = (char*)malloc((strlen(log_line) +1 ) * sizeof(char));
  if(vector->events[vector->size] == NULL){
    exit(EXIT_FAILURE);
  }

  strcpy(vector->events[vector->size], log_line);
  vector->size++;
}

void read_events_from_syslog(EventVector* vector) {
    FILE* syslog_file = fopen(SYSLOG_PATH, "r");

    char line[MAX_LINE_LENGTH];

    if(syslog_file == NULL){ 
        perror("Erro ao abrir");
        return;
    }

    while(fgets(line, sizeof(line), syslog_file) != NULL){
        if(strstr(line, "NetworkManager") != NULL){
            add_event(vector, line);
        }
    }

    fclose(syslog_file);
}


void filter_events(const EventVector* vector, const char* keyword){
    for(int i =0; i < vector->size; i++){
        if(strstr(vector->events[i], keyword) != NULL){
            printf("%s", vector->events[i]);
        }
    }
}


void display_events(EventVector* vector) {
    for(int i =0; i < vector->size; i++){
        printf("%s", vector->events[i]);
    }
}

void clear_events(EventVector* vector) {
    for(int i =0; i < vector->size; i++){
        free(vector->events[i]);
    }
    vector->size = 0;
}

void free_event_vector(EventVector* vector){
    clear_events(vector);
    free(vector->events);
}


int display_menu() {
    int choice;

    printf("\n Menu: \n");
    printf("1. Exibir todos os eventos\n");
    printf("2. Filtrar eventos por palavra\n");
    printf("3. Excluir eventos\n");
    printf("4. Sair\n");
    printf("5. Recarregar eventos\n");
    printf("Escolha uma das acoes acima\n");

    scanf("%d", &choice);
    return choice; 
}


int main() {
   EventVector event_list;
   init_event_vector(&event_list);
   char keyword[MAX_LINE_LENGTH];
   int choice;

   read_events_from_syslog(&event_list);

   while(1){
    choice = display_menu();

    switch(choice) {
        case 1:
            display_events(&event_list);
            break;
        case 2:
            printf("Digite a palavara:");
            getchar();
            fgets(keyword, sizeof(keyword), stdin);
            keyword[strcspn(keyword, "\n")] = 0;
            filter_events(&event_list, keyword);
            break; 
        case 3:
            clear_events(&event_list);
            printf("Eventos excluidos com sucesso");
            break;
        case 4:
            clear_events(&event_list);
            return (EXIT_SUCCESS);
        case 5:
            clear_events(&event_list);
            read_events_from_syslog(&event_list);
            printf("Eventos recarregados com sucesso");
            break; 
        default:
            printf("Acao nao permitida");    
      }
   }

   free_event_vector(&event_list);
   return (EXIT_SUCCESS);
}
