#ifndef OGSE_CONFIG_H
#define OGSE_CONFIG_H

#include "configurator_defines.h"
// эффекты со включением через консоль
// Саншафты								r2_sunshafts [qt_off/qt_low/qt_medium/qt_high/qt_extreme]
// Объемные саншафты					r2_sunshafts_mode volumetric
// Плоские саншафты аля Crysis			r2_sunshafts_mode screen_space

// PARAMETERS 
// Используйте параметры для настройки соответствующего эффекта
/////////////////////////////////////////////////////////////
// Плоские саншафты
	#define SUNSHAFTS_QUALITY 4
	#define SS_DUST
	#define SS_INTENSITY float(1.0)			// яркость лучей, работает совместно с настройками из конфига
	#define SS_BLEND_FACTOR float(0.8)		// фактор смешивания с рассеянным светом. Чем меньше значение, тем меньше "засветка" от эффекта, но теряется "объемность"
	#define SS_LENGTH float(1.0)			// длина лучей. Чем больше длина, тем меньше производительность
	#define SS_DUST_SPEED float(0.4)		// скорость частиц пыли 
	#define SS_DUST_INTENSITY float(2.0)	// яркость пылинок
	#define SS_DUST_DENSITY float(1.0)		// плотность частиц пыли 
	#define SS_DUST_SIZE float(0.7)			// размер пылинок

#endif