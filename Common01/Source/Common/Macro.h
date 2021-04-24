#pragma once

#define ARRAY_LITERAL_SIZE(DATA) (sizeof(DATA)/sizeof(DATA[0]))

#define TOKEN_PAIR(TOKEN) TOKEN,#TOKEN

