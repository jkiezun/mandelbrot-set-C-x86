#include <stdio.h>
#include "allegro5/allegro.h"

extern void mandelbrot_x86(unsigned *pPixelBuffer, int width, int height, double x, double y, double step);

int main()
{
    ALLEGRO_BITMAP *bitmap;
    ALLEGRO_EVENT_QUEUE *eventQueue;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_LOCKED_REGION *locked_region;
    int *pixelBuffer = NULL;

    int width = 1000;
    int height = 1000;

    double x = -2.0;
    double y = -1.25;

    double step = 2.5 / width; // initial image is for x e <-2.0, 0.5> , y e <-1.25, 1.25>

    if (!al_init())
    {
        fprintf(stderr, "failed to initialize allegro!\n");
        return 1;
    }

    if (!al_install_keyboard())
    {
        fprintf(stderr, "failed to initialize keyboard!\n");
        return 1;
    }

    if (!al_install_mouse())
    {
        fprintf(stderr, "failed to initialize mouse!\n");
        return 1;
    }

    bitmap = al_create_bitmap(width, height);
    if (!bitmap)
    {
        fprintf(stderr, "failed to create bitmap!\n");
        return 1;
    }

    eventQueue = al_create_event_queue();

    if (!eventQueue)
    {
        fprintf(stderr, "failed to initialize event queue!\n");
        al_destroy_bitmap(bitmap);
        return 1;
    }

    display = al_create_display(width, height);

    if (!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_event_queue(eventQueue);
        al_destroy_bitmap(bitmap);
        return 1;
    }

    al_register_event_source(eventQueue, al_get_keyboard_event_source());

    al_register_event_source(eventQueue, al_get_mouse_event_source());

    al_register_event_source(eventQueue, al_get_display_event_source(display));

    al_set_target_bitmap(al_get_backbuffer(display)); //hardware acceleration

    locked_region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE); // need to lock bitmap to access it directly

    pixelBuffer = (int *)locked_region->data;

    pixelBuffer -= width * (height - 1); // locked region data starts at most-left 0 row

    mandelbrot_x86(pixelBuffer, width, height, x, y, step);

    al_unlock_bitmap(bitmap);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(bitmap, 0, 0, 0);
    al_flip_display();

    while (1)
    {
        ALLEGRO_EVENT event;

        al_wait_for_event(eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) // x button
        {
            al_destroy_event_queue(eventQueue);
            al_destroy_bitmap(bitmap);
            al_destroy_display(display);
            return 0;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) // use left mouse button to zoom in and right to zoom out
        {
            ALLEGRO_MOUSE_STATE state;

            al_get_mouse_state(&state);

            if (state.buttons & 1) //left button
            {
                x += step * event.mouse.x;
                y += step * (height - event.mouse.y); // inverted Y axis
                step = step / 2.0;                    // zoom 4x (2x X and 2x Y)
                x -= step * width / 2.0;              // center the zoom on x axis
                y -= step * height / 2.0;             // center the zoom on y axis

                locked_region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);

                pixelBuffer = (int *)locked_region->data;

                pixelBuffer -= width * (height - 1);

                mandelbrot_x86(pixelBuffer, width, height, x, y, step);

                al_unlock_bitmap(bitmap);
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_bitmap(bitmap, 0, 0, 0);
                al_flip_display();
            }
            else if (state.buttons & 2) //right button
            {
                x -= step * width / 2.0;
                y -= step * height / 2.0;
                step *= 2;

                locked_region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);

                pixelBuffer = (int *)locked_region->data;

                pixelBuffer -= width * (height - 1);

                mandelbrot_x86(pixelBuffer, width, height, x, y, step);

                al_unlock_bitmap(bitmap);
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_bitmap(bitmap, 0, 0, 0);
                al_flip_display();
            }
        }
    }
}