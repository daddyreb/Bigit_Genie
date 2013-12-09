#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#ifdef __cplusplus
extern "C"{
#endif

    int lockfile(int);
    int already_running(const char *lockfile);

#ifdef __cplusplus
}
#endif

#endif // SINGLEINSTANCE_H
