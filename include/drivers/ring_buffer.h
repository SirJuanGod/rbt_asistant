#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class AudioRingBuffer {
public:
    AudioRingBuffer();
    ~AudioRingBuffer();

    bool init(size_t capacity);
    void write(const uint8_t* data, size_t len);
    size_t read(uint8_t* out_buffer, size_t max_len);
    size_t available();
    void clear();

private:
    uint8_t* m_buffer;
    size_t m_capacity;
    size_t m_head;
    size_t m_tail;
    size_t m_count;
    SemaphoreHandle_t m_mutex;
};

#endif // RING_BUFFER_H
