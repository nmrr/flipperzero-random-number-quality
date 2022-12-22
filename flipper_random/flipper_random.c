// CC0 1.0 Universal (CC0 1.0)
// Public Domain Dedication
// https://github.com/nmrr

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <furi_hal_random.h>

#include <storage/storage.h>
#include <stream/buffered_file_stream.h>

typedef enum {
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} EventApp;

static void draw_callback(Canvas* canvas, void* ctx) 
{
    UNUSED(ctx);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignBottom, "Copy 256MB of random");
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, "data on /random.dat");

    canvas_draw_str_aligned(canvas, 64, 52, AlignCenter, AlignBottom, "Please wait...");
}

int32_t flipper_random_app() 
{
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    //////////////////

    Storage* storage = furi_record_open(RECORD_STORAGE);

    furi_record_close(RECORD_STORAGE);
    Stream* file_stream = buffered_file_stream_alloc(storage);
    buffered_file_stream_open(file_stream, EXT_PATH("/random.dat"), FSAM_WRITE, FSOM_CREATE_ALWAYS);

    uint8_t randomuint8[8];
    FuriString* data_str = furi_string_alloc();

    for (unsigned int i=0;i<1024*1024*32;i++)
    {
        furi_hal_random_fill_buf(randomuint8,8);
        furi_string_printf(data_str, "%c%c%c%c%c%c%c%c", randomuint8[0], randomuint8[1], randomuint8[2], randomuint8[3], randomuint8[4], randomuint8[5], randomuint8[6], randomuint8[7]);
        stream_write_string(file_stream, data_str);
    }

    buffered_file_stream_close(file_stream);
    stream_free(file_stream);
    furi_string_free(data_str);

    //////////////////

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    furi_record_close(RECORD_NOTIFICATION);

    return 0;
}