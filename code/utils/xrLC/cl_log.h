#pragma	once

void clMsg			(const char *format, ...);
void Status			(const char *format, ...);
static float		progress = 0.0f;
inline void Progress(const float F) noexcept { progress = F; }
void Phase			(const char* phase_name);
//void Phase			(const char* phase_name, bool setcapt = false);

void logThread	    (void *dummy);
void logCallback	(LPCSTR c);
