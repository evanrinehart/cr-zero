#include <stdio.h>
#include <ruby.h>
#include <raylib.h>

static VALUE top_self;
static ID setup_id;
static ID update_id;
static ID redraw_id;
static ID quit_id;
static ID input_key_id;

static VALUE call_setup(VALUE unused) {
    return rb_funcall(top_self, setup_id, 0);
}

static VALUE call_update(VALUE unused) {
    return rb_funcall(top_self, update_id, 0);
}

static VALUE call_redraw(VALUE unused) {
    return rb_funcall(top_self, redraw_id, 0);
}

static VALUE call_quit(VALUE unused) {
    return rb_funcall(top_self, quit_id, 0);
}

static long call_input_key_arg1;
static long call_input_key_arg2;
static VALUE call_input_key(VALUE unused) {
    VALUE arg1 = LONG2NUM(call_input_key_arg1);
    VALUE arg2 = LONG2NUM(call_input_key_arg2);
    return rb_funcall(top_self, input_key_id, 2, arg1, arg2);
}

static VALUE c_draw_rectangle(VALUE self, VALUE xx, VALUE yy, VALUE ww, VALUE hh, VALUE ccolor) {
    float x = NUM2DBL(xx);
    float y = NUM2DBL(yy);
    float w = NUM2DBL(ww);
    float h = NUM2DBL(hh);
    Color color;
    color.r = NUM2UINT(rb_struct_aref(ccolor, INT2FIX(0)));
    color.g = NUM2UINT(rb_struct_aref(ccolor, INT2FIX(1)));
    color.b = NUM2UINT(rb_struct_aref(ccolor, INT2FIX(2)));
    color.a = NUM2UINT(rb_struct_aref(ccolor, INT2FIX(3)));
    DrawRectangle(x,y,w,h,color);
    return Qnil;
}

int main(int argc, char *argv[]) {

    // ruby
    ruby_init();
    ruby_init_loadpath();
    ruby_incpush("./lib");

    int state = 0;
    rb_load_protect(rb_str_new_cstr("./setup.rb"), 0, &state);
    if (state != 0) {
        printf("rb_load_protect: failed\n");
        ruby_cleanup(1);
        return 1;
    }

    top_self = rb_eval_string_protect("self", &state);
    if (state != 0) {
        printf("rb_eval_string_protect: failed\n");
        ruby_cleanup(1);
        return 1;
    }

    setup_id = rb_intern("setup");
    update_id = rb_intern("update");
    redraw_id = rb_intern("redraw");
    quit_id = rb_intern("quit");
    input_key_id = rb_intern("input_key");

    rb_define_global_function("c_draw_rectangle", c_draw_rectangle, 5);

    rb_protect(call_setup, Qnil, &state);
    if (state != 0) {
        printf("rb_funcall: setup failed\n");
        ruby_cleanup(1);
        return 1;
    }

    // raylib
    InitWindow(640,480,"basic window");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_ALWAYS_RUN);

    while (WindowShouldClose() == 0) {
        // input
        int state = 0;
        if (IsKeyPressed(KEY_ENTER)) {
            call_input_key_arg1 = KEY_ENTER;
            call_input_key_arg2 = 1;
            VALUE result = rb_protect(call_input_key, Qnil, &state);
            if (state != 0) {
                printf("rb_funcall: input_key failed\n");
                ruby_cleanup(1);
                CloseWindow();
                return 1;
            }
            if (result == Qtrue) {
                ruby_cleanup(0);
                CloseWindow();
                return 0;
            }
        }

        // update
        VALUE result = rb_protect(call_update, Qnil, &state);
        if (state != 0) {
            printf("rb_funcall: update failed\n");
            ruby_cleanup(1);
            CloseWindow();
            return 1;
        }
        if (result == Qtrue) {
            ruby_cleanup(0);
            CloseWindow();
            return 0;
        }

        // draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        rb_protect(call_redraw, Qnil, &state);
        if (state != 0) {
            printf("rb_funcall: redraw failed\n");
            ruby_cleanup(1);
            CloseWindow();
            return 1;
        }

        EndDrawing();
    }

    // quit
    rb_protect(call_quit, Qnil, &state);
    if (state != 0) {
        printf("rb_funcall: quit failed\n");
        ruby_cleanup(1);
        CloseWindow();
        return 1;
    }

    CloseWindow();
    ruby_cleanup(0);

    return 0;
}
