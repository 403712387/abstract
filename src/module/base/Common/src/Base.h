#ifndef BASE_H
#define BASE_H

#ifdef WIN32
#define ABSTRACT_EXPORT __declspec(dllexport)
#else
#define ABSTRACT_EXPORT
#endif

#endif
