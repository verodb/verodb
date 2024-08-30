#ifndef AOF_H
#define AOF_H

void initAof(const char *filename);
void appendToAof(const char *operation);
void closeAof();

#endif // AOF_H
