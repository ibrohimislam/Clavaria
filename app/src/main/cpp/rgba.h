#include <jni.h>
#include <cstdint>

#ifndef CLAVARIA_RGBA_H
#define CLAVARIA_RGBA_H


class rgba {

    union color {
        uint8_t component[4];
        uint32_t rgba;
    };

    color c;

public:

    rgba() {
        c.rgba = 0;
    }

    uint32_t to_int() {
        return c.rgba;
    }

    uint32_t getR(){
        return c.component[2];
    }
    uint32_t getG(){
        return c.component[1];
    }
    uint32_t getB(){
        return c.component[0];
    }
    uint32_t getA(){
        return c.component[3];
    }
    void setR(uint32_t _r){
        c.component[2] = uint8_t(_r);
    }
    void setG(uint32_t _g){
        c.component[1] = uint8_t(_g);
    }
    void setB(uint32_t _b){
        c.component[0] = uint8_t(_b);
    }
    void setA(uint32_t _a){
        c.component[3] = uint8_t(_a);
    }
    void setAll(uint32_t _c) {
        c.component[0] = uint8_t(_c);
        c.component[1] = uint8_t(_c);
        c.component[2] = uint8_t(_c);
    }
    void setRGBA(uint32_t _rgba) {
        c.rgba = _rgba;
    }

};
#endif //CLAVARIA_RGBA_H
