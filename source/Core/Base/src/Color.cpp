#include <NexBase.h>
#include <Color.h>

namespace nextar {

const Color Color::Black(1, 0, 0, 0);
const Color Color::White(1, 1, 1, 1);
const Color Color::Red(1, 1, 0, 0);
const Color Color::Green(1, 0, 1, 0);
const Color Color::Blue(1, 0, 0, 1);

const Color32 Color32::Black((uint8)255, (uint8)0, (uint8)0, (uint8)0);
const Color32 Color32::White((uint8)255, (uint8)255, (uint8)255, (uint8)255);
const Color32 Color32::Red((uint8)255, (uint8)255, (uint8)0, (uint8)0);
const Color32 Color32::Green((uint8)255, (uint8)0, (uint8)255, (uint8)0);
const Color32 Color32::Blue((uint8)255, (uint8)0, (uint8)0, (uint8)255);

}
