#ifndef _EMOTION_HPP_
#define _EMOTION_HPP_

#include <Message.hpp>


typedef struct EmotionType
{
    //  face
    int8_t face;
} emotion_t;

//  create message
typedef sb::Message<emotion_t> Emotion;

#endif