#include "drivers/ring_buffer.h"

AudioRingBuffer::AudioRingBuffer()
    : m_buffer(nullptr)
    , m_capacity(0)
    , m_head(0)
    , m_tail(0)
    , m_count(0)
    , m_mutex(nullptr)
{
}

AudioRingBuffer::~AudioRingBuffer() {
    if (m_buffer != nullptr) {
        vPortFree(m_buffer);
        m_buffer = nullptr;
    }
    if (m_mutex != nullptr) {
        vSemaphoreDelete(m_mutex);
        m_mutex = nullptr;
    }
}

bool AudioRingBuffer::init(size_t capacity) {
    m_capacity = capacity;
    m_buffer = (uint8_t*) pvPortMalloc(capacity);
    m_mutex = xSemaphoreCreateMutex();
    
    if (m_buffer == nullptr || m_mutex == nullptr) {
        return false;
    }
    
    clear();
    return true;
}

void AudioRingBuffer::write(const uint8_t* data, size_t len) {
    if (m_buffer == nullptr || m_mutex == nullptr) return;

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        for (size_t i = 0; i < len; i++) {
            if (m_count >= m_capacity) {
                m_tail = (m_tail + 1) % m_capacity;
                m_count--;
            }
            m_buffer[m_head] = data[i];
            m_head = (m_head + 1) % m_capacity;
            m_count++;
        }
        xSemaphoreGive(m_mutex);
    }
}

size_t AudioRingBuffer::read(uint8_t* out_buffer, size_t max_len) {
    if (m_buffer == nullptr || m_mutex == nullptr) return 0;

    size_t read_len = 0;
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        read_len = (max_len < m_count) ? max_len : m_count;
        for (size_t i = 0; i < read_len; i++) {
            out_buffer[i] = m_buffer[m_tail];
            m_tail = (m_tail + 1) % m_capacity;
        }
        m_count -= read_len;
        xSemaphoreGive(m_mutex);
    }
    return read_len;
}

size_t AudioRingBuffer::available() {
    if (m_buffer == nullptr || m_mutex == nullptr) return 0;

    size_t avail = 0;
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        avail = m_count;
        xSemaphoreGive(m_mutex);
    }
    return avail;
}

void AudioRingBuffer::clear() {
    if (m_mutex != nullptr) {
        if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
            m_head = 0;
            m_tail = 0;
            m_count = 0;
            xSemaphoreGive(m_mutex);
        }
    }
}
