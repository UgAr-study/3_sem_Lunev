#include "header.h"

void free_all(struct Channel *channels, size_t size) {
    for (int i = 0; i < size; ++i) {
        char *buff = NULL;
        if ((buff = channels[i].buffer)) {
            free (buff);
        }
    }
}
