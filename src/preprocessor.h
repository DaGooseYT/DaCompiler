#pragma once

#include "lexer.h"

enum State {
    ENABLED,
    DISABLED,
    INVALID,
};

Vector* preprocess(const Vector* vec);
