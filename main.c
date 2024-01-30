#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#define WIDTH 900
#define HEIGHT 600
#define SIZE 300
#define ADD_WIDTH 140
#define ADD_HEIGHT 40
#define DELETE_WIDTH 170
#define DELETE_HEIGHT 40
#define NEW_RECT_WIDTH 250
#define NEW_RECT_HEIGHT 40
#define FPS 400
#define MAX_TASKS 60

int main(int argc, char* argv[]) {

    /* Initializes the timer, audio, video, joystick, haptic, gamecontroller and events subsystems */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }


    /* Create a window */
    SDL_Window* wind = SDL_CreateWindow("Todo List",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WIDTH, HEIGHT, 0);

    if (!wind) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }


    /* Create a renderer */
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, render_flags);
    if (!rend) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }


    /* Init of ttf */
    if (TTF_Init() != 0) {
        printf("Error initializing TTF: %s\n", TTF_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }


    /* For the font */
    TTF_Font* font = TTF_OpenFont("C:\\Users\\agath\\Documents\\SDL\\Baloo-Regular.ttf", 25);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }


    /* Main loop */
    bool running = true;
    bool mouse_clicked = false;
    bool delete_mode = false;
    bool dragging[MAX_TASKS];
    float x_pos = (WIDTH-(ADD_WIDTH+20)), y_pos = (HEIGHT-(ADD_HEIGHT+20)), x_vel = 0, y_vel = 0;
    SDL_Rect rect = {(int) x_pos, (int) y_pos, ADD_WIDTH, ADD_HEIGHT};
    float x_delete = (WIDTH-(DELETE_WIDTH+20)), y_delete = (HEIGHT-(DELETE_HEIGHT+20)), x_v = 0, y_v = 0;
    SDL_Rect delete_button = {20, HEIGHT - DELETE_HEIGHT - 20, DELETE_WIDTH, DELETE_HEIGHT};
    SDL_Rect new_rect = {x_pos - NEW_RECT_WIDTH - 20, y_pos, NEW_RECT_WIDTH, NEW_RECT_HEIGHT};
    SDL_Rect column_to_do = {0, 0, SIZE, HEIGHT};
    SDL_Rect column_working_on_that = {SIZE, 0, SIZE, HEIGHT};
    SDL_Rect column_done = {SIZE+SIZE, 0, SIZE, HEIGHT};
    SDL_Rect tasks[MAX_TASKS];
    SDL_Event event;
    int tasks_number = 0;


    /* Load the save file */
    FILE* loadFile = fopen("tasks.txt", "r");
    if (loadFile != NULL) {
        while (fscanf(loadFile, "%d %d %d %d\n", &tasks[tasks_number].x, &tasks[tasks_number].y, &tasks[tasks_number].w, &tasks[tasks_number].h) == 4) {
            tasks_number++;
        }
        fclose(loadFile);
    } else {
        printf("Error opening the save file.\n");
    }


    while (running) {

        /* Process events */
        while (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    int x_mouse;
                    int y_mouse;
                    SDL_GetMouseState(&x_mouse, &y_mouse);

                    if (event.button.button == SDL_BUTTON_LEFT) {

                        /* Check if the click in inside the ADD TASK button */
                        if (x_mouse >= rect.x && x_mouse <= rect.x + ADD_WIDTH && y_mouse >= rect.y &&
                            y_mouse <= rect.y + ADD_HEIGHT) {
                            if (tasks_number < MAX_TASKS) {
                                tasks[tasks_number] = (SDL_Rect) {x_pos - (NEW_RECT_WIDTH + 20), y_pos, NEW_RECT_WIDTH,
                                                                  NEW_RECT_HEIGHT};
                                dragging[tasks_number] = false;
                                printf("Ajout d'une nouvelle tâche, nombre total de tâches : %d\n", tasks_number + 1);
                                tasks_number++;
                            }
                        }

                        /* Check if the click is inside the DELETE TASK button */
                        if (x_mouse >= delete_button.x && x_mouse <= delete_button.x + delete_button.w &&
                            y_mouse >= delete_button.y && y_mouse <= delete_button.y + delete_button.h) {
                            delete_mode = true;
                        }


                        /* Check if the click is on a task */
                        for (int i = 0; i < tasks_number; i++) {
                            if (x_mouse >= tasks[i].x && x_mouse <= tasks[i].x + tasks[i].w && y_mouse >= tasks[i].y &&
                                y_mouse <= tasks[i].y + tasks[i].h) {
                                if (delete_mode) {
                                    for (int j = i; j < tasks_number - 1; j++) {
                                        tasks[j] = tasks[j + 1];
                                    }
                                    tasks_number--;
                                    delete_mode = false;
                                } else {
                                    dragging[i] = true;
                                    printf("Début de l'édition de la tâche %d\n", i + 1);
                                }
                                break;
                            }
                        }
                    }
                    break;

                    /* Stop dragging and editing when you're not selecting a task */
                case SDL_MOUSEBUTTONUP:
                    printf("SDL_MOUSEBUTTONUP ok\n");
                    for (int i = 0; i < tasks_number; i++) {
                        if (dragging[i]) {
                            printf("Fin du déplacement ou de l'édition de la tâche %d\n", i + 1);
                        }
                        dragging[i] = false;
                    }

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        mouse_clicked = false;
                    }
                    break;


                case SDL_MOUSEMOTION:
                    for (int i = 0; i < tasks_number; i++) {
                        if (dragging[i]) {
                            int x_mouse;
                            int y_mouse;
                            SDL_GetMouseState(&x_mouse, &y_mouse);
                            tasks[i].x = x_mouse - tasks[i].w / 2;
                            tasks[i].y = y_mouse - tasks[i].h / 2;
                        }
                    }

                default:
                    break;
            }


            /* Clear screen */
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            SDL_RenderClear(rend);


            /* Draw the columns */
            SDL_SetRenderDrawColor(rend, 63, 138, 196 , 255);
            SDL_RenderFillRect(rend, &column_to_do);

            SDL_SetRenderDrawColor(rend, 229, 143, 58, 255);
            SDL_RenderFillRect(rend, &column_working_on_that);

            SDL_SetRenderDrawColor(rend, 147, 224, 78, 255);
            SDL_RenderFillRect(rend, &column_done);


            /* Draw columns' tittles */
            SDL_Color textColor = {255, 255, 255};
            SDL_Surface* textSurfaceToDo = TTF_RenderText_Blended(font, "TO DO", textColor);
            SDL_Texture* textTextureToDo = SDL_CreateTextureFromSurface(rend, textSurfaceToDo);
            SDL_Rect textRectToDo = {column_to_do.x + SIZE / 2 - textSurfaceToDo->w / 2, 10, textSurfaceToDo->w, textSurfaceToDo->h};
            SDL_RenderCopy(rend, textTextureToDo, NULL, &textRectToDo);

            SDL_Surface* textSurfaceWorking = TTF_RenderText_Blended(font, "WORKING ON THAT", textColor);
            SDL_Texture* textTextureWorking = SDL_CreateTextureFromSurface(rend, textSurfaceWorking);
            SDL_Rect textRectWorking = {column_working_on_that.x + SIZE / 2 - textSurfaceWorking->w / 2, 10, textSurfaceWorking->w, textSurfaceWorking->h};
            SDL_RenderCopy(rend, textTextureWorking, NULL, &textRectWorking);

            SDL_Surface* textSurfaceDone = TTF_RenderText_Blended(font, "DONE", textColor);
            SDL_Texture* textTextureDone = SDL_CreateTextureFromSurface(rend, textSurfaceDone);
            SDL_Rect textRectDone = {column_done.x + SIZE / 2 - textSurfaceDone->w / 2, 10, textSurfaceDone->w, textSurfaceDone->h};
            SDL_RenderCopy(rend, textTextureDone, NULL, &textRectDone);

            SDL_FreeSurface(textSurfaceToDo);
            SDL_DestroyTexture(textTextureToDo);
            SDL_FreeSurface(textSurfaceWorking);
            SDL_DestroyTexture(textTextureWorking);
            SDL_FreeSurface(textSurfaceDone);
            SDL_DestroyTexture(textTextureDone);


            /* Draw the rectangle > the button */
            SDL_SetRenderDrawColor(rend, 197, 190, 188 , 255);
            SDL_RenderFillRect(rend, &rect);


            /* Hover on the ADD button */
            int x_mouse;
            int y_mouse;
            SDL_GetMouseState(&x_mouse, &y_mouse);

            if (x_mouse >= rect.x && x_mouse <= rect.x + ADD_WIDTH && y_mouse >= rect.y && y_mouse <= rect.y + ADD_HEIGHT) {
                SDL_SetRenderDrawColor(rend, 158, 153, 153 , 255);
            } else {
                SDL_SetRenderDrawColor(rend, 197, 190, 188, 255);
            }
            SDL_RenderFillRect(rend, &rect);


            /* Hover on the DELETE button */
            SDL_GetMouseState(&x_mouse, &y_mouse);

            if (x_mouse >= delete_button.x && x_mouse <= delete_button.x + DELETE_WIDTH && y_mouse >= delete_button.y && y_mouse <= delete_button.y + DELETE_HEIGHT) {
                SDL_SetRenderDrawColor(rend, 158, 153, 153 , 255);
            } else {
                SDL_SetRenderDrawColor(rend, 197, 190, 188, 255);
            }
            SDL_RenderFillRect(rend, &delete_button);


            /* ADD TASK text on the button */
            char bottomText[] = "ADD TASK";
            SDL_Surface* bottomTextSurface = TTF_RenderText_Blended(font, bottomText, textColor);
            SDL_Texture* bottomTextTexture = SDL_CreateTextureFromSurface(rend, bottomTextSurface);
            SDL_Rect bottomTextRect = {WIDTH - 150, HEIGHT - 55, bottomTextSurface->w, bottomTextSurface->h};
            SDL_RenderCopy(rend, bottomTextTexture, NULL, &bottomTextRect);

            SDL_FreeSurface(bottomTextSurface);
            SDL_DestroyTexture(bottomTextTexture);

            /* DELETE TASK text on the button */
            char deleteText[] = "DELETE TASK";
            SDL_Surface* deleteTextSurface = TTF_RenderText_Blended(font, deleteText, textColor);
            SDL_Texture* deleteTextTexture = SDL_CreateTextureFromSurface(rend, deleteTextSurface);
            SDL_Rect deleteTextRect = {30, HEIGHT - 55, deleteTextSurface->w, deleteTextSurface->h};
            SDL_RenderCopy(rend, deleteTextTexture, NULL, &deleteTextRect);

            SDL_FreeSurface(deleteTextSurface);
            SDL_DestroyTexture(deleteTextTexture);

        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);


        for (int i = 0; i < tasks_number; i++) {
            SDL_RenderFillRect(rend, &tasks[i]);

            /* Text on the tasks */
            SDL_Color textColor = {255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, "a task", textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(rend, textSurface);
            SDL_Rect textRect = {tasks[i].x, tasks[i].y, textSurface->w, textSurface->h};
            SDL_RenderCopy(rend, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }

        /* Draw to window and loop */
        SDL_RenderPresent(rend);
        SDL_Delay(1000/FPS);

    }


    /* Save before quit */
    FILE* saveFile = fopen("tasks.txt", "w");
    if (saveFile != NULL) {
        for (int i = 0; i < tasks_number; i++) {
            fprintf(saveFile, "%d %d %d %d\n", tasks[i].x, tasks[i].y, tasks[i].w, tasks[i].h);
        }
        fclose(saveFile);
    } else {
        printf("Error opening the save file.\n");
    }

    /* Release resources */
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}