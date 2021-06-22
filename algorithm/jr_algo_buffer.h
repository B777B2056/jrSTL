#ifndef JR_ALGO_BUFFER_H
#define JR_ALGO_BUFFER_H

#include <cstddef>

namespace panzer {
    /* RAII方式管理algorithm中分配的缓存空间 */
    template< class T >
    class _buffer {
        private:
            T *_b;
            size_t _len;

        public:
            _buffer(size_t len)
                : _b(new T[len]) {
                _len = _b ? len : 0;
            }

            ~_buffer() {
                if(_b)
                    delete []_b;
            }

            bool is_valid() const {
                return _b != nullptr;
            }

            size_t length() const {
                return _len;
            }

            T& operator[](size_t n) {
                return _b[n];
            }

            const T& operator[](size_t n) const {
                return _b[n];
            }
    };
}

#endif // JR_ALGO_BUFFER_H
