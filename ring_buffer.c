#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int* start;
    int capacity;
    int writeIndex;
    int readIndex;
} RingBuffer;

// create a new ring buffer
RingBuffer* rb_new(int capacity) {
    RingBuffer* buffer = malloc(sizeof(RingBuffer));
    buffer->start = calloc(capacity, sizeof(int));
    buffer->capacity = capacity;
    buffer->writeIndex = 0;
    buffer->readIndex = 0;
    return buffer;
}

// free ring buffer safely
void rb_free(RingBuffer* buffer) {
    if (buffer) {
        free(buffer->start);
        free(buffer);
    }
}

// check if the buffer is full (should not be written to)
bool rb_is_full(RingBuffer buffer) {
    return (buffer.writeIndex + 1) % buffer.capacity == buffer.readIndex;
}

// check if the buffer is empty (should not be read from)
bool rb_is_empty(RingBuffer buffer) {
    return buffer.readIndex == buffer.writeIndex;
}

// write to the buffer if there is space
bool rb_write(RingBuffer* buffer, int value) {
    if (rb_is_full(*buffer)) return false;
    buffer->start[buffer->writeIndex] = value;
    buffer->writeIndex = (buffer->writeIndex + 1) % buffer->capacity;
    return true;
}

// read from the buffer if there is a value to be read
bool rb_read(RingBuffer* buffer, int* out) {
    if (rb_is_empty(*buffer)) return false;
    *out = buffer->start[buffer->readIndex];
    buffer->readIndex = (buffer->readIndex + 1) % buffer->capacity;
    return true;
}

// a value is "garbage" if it has already been read or was never initialised
bool rb_value_is_garbage(RingBuffer buffer, int index) {
    if (rb_is_empty(buffer)) return true;
    if (rb_is_full(buffer)) return false;
    // check for wrap-around
    return (buffer.readIndex <= buffer.writeIndex
        ? index < buffer.readIndex || index >= buffer.writeIndex
        : index < buffer.readIndex && index >= buffer.writeIndex
    );
}

// pretty print the whole thing
void rb_dump_state(RingBuffer buffer) {
    printf("[");
    for (int i = 0; i < buffer.capacity; i++) {
        if (i == buffer.readIndex) printf("\e[1;32mr\e[m"); // green 'r'
        if (i == buffer.writeIndex) printf("\e[1;31mw\e[m"); // red 'w'
        if (rb_value_is_garbage(buffer, i)) printf("\e[2;90m"); // grey
        printf("%d", buffer.start[i]);
        printf("\e[m, ");
    }
    printf("\b\b  \b\b]");
}

int main() {
    printf("create a new ring buffer with capacity 5:\n");
    RingBuffer* buffer = rb_new(5);
    rb_dump_state(*buffer);

    printf("\n\nwrite 3 values:\n");
    for (int i = 1; i < 4; i++) {
        printf("write %d\t\t", i);
        rb_write(buffer, i);
        rb_dump_state(*buffer);
        printf("\n");
    }

    printf("\nread a value:\n");
    int value;
    rb_read(buffer, &value);
    printf("read: %d\t\t", value);
    rb_dump_state(*buffer);
    printf("\n");

    printf("\nread until empty:\n");
    while (rb_read(buffer, &value)) {
        printf("read: %d\t\t", value);
        rb_dump_state(*buffer);
        printf("\n");
    }
    printf("empty\n");

    printf("\nwrite until full, wrapping around:\n");
    for (int i = 4; rb_write(buffer, i); i++) {
        printf("write %d\t\t", i);
        rb_dump_state(*buffer);
        printf("\n");
    }
    printf("full\n");

    printf("\nread until empty, wrapping around:\n");
    while (rb_read(buffer, &value)) {
        printf("read: %d\t\t", value);
        rb_dump_state(*buffer);
        printf("\n");
    }
    printf("empty\n");

    rb_free(buffer);
    return 0;
}
