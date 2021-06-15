// #define FIFTH_INDEX_PAIR struct { VAR* aEnd; VAR* bEnd; }
typedef struct {
    VAR* aEnd;
    VAR* bEnd;
} FUNC(fifthIndexPair);

static void FUNC(fifthInsertionSort)(VAR* a, VAR* b, FIFTHCMP cmp) {
    VAR* i = a + 1, *j, *k, tmp, *pos, *pos1;
    if (cmp(i - 1, i) > 0) {
        i++;
        while (i < b && cmp(i - 1, i) > 0) i++;
        j = a;
        k = i - 1;
        while (j < k) {
            tmp = *j;
            *j++ = *k;
            *k-- = tmp;
        }
    } else {
        i++;
        while (i < b && cmp(i - 1, i) <= 0) i++;
    }

    while (i < b) {
        tmp = *i;
        pos = i - 1;
        pos1 = i;
        while (pos1 > a && cmp(pos, &tmp) > 0) {
            *pos1-- = *pos--;
        }
        *pos1 = tmp;
        i++;
    }
}

static FUNC(fifthIndexPair) FUNC(fifthMergeInPlaceBackwards)(VAR* array, VAR* buffer, VAR* mid, VAR* end, FIFTHCMP cmp) {
    VAR *left = mid - 1, *right = end - 1;
    while (buffer > right && right >= mid) {
        if (cmp(left, right) > 0) {
            *buffer-- = *left--;
        } else {
            *buffer-- = *right--;
        }
    }

    if (right == left) {
        array--;
        while (right > array) {
            *buffer-- = *right--;
        }
    } else if (right < mid) {
        array--;
        while (left > array) {
            *buffer-- = *left--;
        }
    }
    return (FUNC(fifthIndexPair)) { left + 1, right + 1 };
}

static void FUNC(fifthMergeForwardsWithBuffer)(VAR* dest, VAR* left, VAR* leftEnd, VAR* mid, VAR* end, FIFTHCMP cmp) {
    VAR* right = mid;
    while (left < leftEnd && right < end) {
        if (cmp(left, right) > 0) {
            *dest++ = *right++;
        } else {
            *dest++ = *left++;
        }
    }

    while (left < leftEnd) {
        *dest++ = *left++;
    }
}

static void FUNC(fifthMerge)(VAR* dest, VAR* start, VAR* mid, VAR* end, FIFTHCMP cmp) {
    VAR *left = start, *right = mid;
    while (left < mid && right < end) {
        if (cmp(left, right) > 0) {
            *dest++ = *right++;
        } else {
            *dest++ = *left++;
        }
    }

    while (left < mid) {
        *dest++ = *left++;
    }
    while (right < end) {
        *dest++ = *right++;
    }
}

static void FUNC(fifthPingPong)(VAR* start, VAR* buffer, VAR* end, FIFTHCMP cmp) {
    VAR *i, *j;
    for (i = start; i + 8 < end; i += 8) {
        FUNC(fifthInsertionSort)(i, i + 8, cmp);
    }
    if (end - i > 1) {
        FUNC(fifthInsertionSort)(i, end, cmp);
    }

    size_t length = end - start;
    bool fromBuffer = false;
    for (size_t gap = 8; gap < length; gap *= 2) {
        size_t fullMerge = gap * 2;
        if (fromBuffer) {
            for (i = start, j = buffer; i + fullMerge < end; i += fullMerge, j += fullMerge) {
                FUNC(fifthMerge)(i, j, j + gap, j + fullMerge, cmp);
            }
            if (i + gap < end) {
                FUNC(fifthMerge)(i, j, j + gap, end - i + j, cmp);
            } else {
                memcpy(i, j, sizeof(VAR) * (end - i));
            }
        } else {
            for (i = start, j = buffer; i + fullMerge < end; i += fullMerge, j += fullMerge) {
                FUNC(fifthMerge)(j, i, i + gap, i + fullMerge, cmp);
            }
            if (i + gap < end) {
                FUNC(fifthMerge)(j, i, i + gap, end, cmp);
            } else {
                memcpy(j, i, sizeof(VAR) * (end - i));
            }
        }
        fromBuffer = !fromBuffer;
    }
    if (fromBuffer) {
        memcpy(start, buffer, sizeof(VAR) * length);
    }
}

static void FUNC(fifthMergeSort)(VAR* array, size_t length, FIFTHCMP cmp) {
    size_t fifthLen = length / 5;
    size_t bufferLen = length - fifthLen * 4;
    VAR* buffer;
    if (bufferLen < 1025) {
        VAR stackBuffer[1024];
        buffer = stackBuffer;
    } else {
        buffer = (VAR*)malloc(sizeof(VAR) * bufferLen);
    }

    FUNC(fifthPingPong)(array, buffer, array + bufferLen, cmp);
    uint_fast8_t i;
    VAR* start = array + bufferLen;
    for (i = 0; i < 4; i++, start += fifthLen) {
        FUNC(fifthPingPong)(start, buffer, start + fifthLen, cmp);
    }

    memcpy(buffer, array, sizeof(VAR) * bufferLen);
    size_t twoFifths = 2 * fifthLen;
    start = array + bufferLen;
    for (i = 0; i < 2; i++, start += twoFifths) {
        FUNC(fifthMerge)(start - bufferLen, start, start + fifthLen, start + twoFifths, cmp);
    }

    FUNC(fifthIndexPair) finalMerge = FUNC(fifthMergeInPlaceBackwards)(array, array + length - 1, array + twoFifths, array + 2 * twoFifths, cmp);
    if (finalMerge.bEnd > array) {
        FUNC(fifthMergeForwardsWithBuffer)(array + bufferLen, array, finalMerge.aEnd, array + twoFifths, array + length, cmp);
    }

    FUNC(fifthMergeForwardsWithBuffer)(array, buffer, buffer + bufferLen, array + bufferLen, array + length, cmp);

    if (bufferLen > 1024) {
        free(buffer);
    }
}
