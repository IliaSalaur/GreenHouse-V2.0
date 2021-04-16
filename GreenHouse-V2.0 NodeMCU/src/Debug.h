#pragma once
#ifndef DEBUG_H
#define DEBUG_H
#define DEBUG_

#ifdef DEBUG_
#define DEBUG(x) Serial.println(x);
#else
#define DEBUG(x)
#endif
#endif