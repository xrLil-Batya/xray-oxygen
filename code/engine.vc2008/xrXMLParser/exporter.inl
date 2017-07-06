#ifdef XRXMLPARSER_EXPORTS
#define XRXMLPARSER_API __declspec(dllexport)
#define TINYXML2_EXPORT
#else
#define XRXMLPARSER_API __declspec(dllimport)
#define TINYXML2_IMPORT
#endif
