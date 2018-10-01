#pragma once
// ForserX to All: Править хейдеры dxInput я не буду из-за нового предупреждения по оффсетофу

#undef DIMOFS_X
#undef DIMOFS_Y
#undef DIMOFS_Z

#define DIMOFS_X offsetof(DIMOUSESTATE, lX)
#define DIMOFS_Y offsetof(DIMOUSESTATE, lY)
#define DIMOFS_Z offsetof(DIMOUSESTATE, lZ)